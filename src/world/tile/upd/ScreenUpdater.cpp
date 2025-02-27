#include "ScreenUpdater.h"
#include <nch/cpp-utils/log.h>
#include <nch/math-utils/vec3.h>
#include <nch/cpp-utils/timer.h>
#include "DebugScreen.h"
#include "Grid.h"
#include "RegTexInfo.h"
using namespace nch;

void ScreenUpdater::init(SDLHandler* sh, TileMap* tm, Canvas* cs)
{
	sdlHandler = sh;
	tileMap = tm;
    csTileMap = cs;
	cam = csTileMap->getCamera();
}

void ScreenUpdater::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "RegLoadCountMax=" << loadCountMax << "; ";
	ss << "RegLoadTime (avg out of last " << infoRegLoadDivisor << ")=" << infoRegLoadTimeAvg << "ms; ";
	DebugScreen::newLine(ss);
}

TileMap* ScreenUpdater::getTileMap() { return tileMap; }

ScreenUpdater::t_regUpdates* ScreenUpdater::getRegUpdates() { return &regUpdates; }
ScreenUpdater::t_tileUpdates* ScreenUpdater::getTileUpdates() { return &tileUpdates; }

/*
 * Return the set of all tile updates (set<pair<int64_t, int64_t>>) within a specific region. If region doesn't exist, return nullptr.
 */
ScreenUpdater::t_updates* ScreenUpdater::getTUsByRXY(int64_t csRX, int64_t csRY) 
{
    t_tileUpdates::iterator itr = tileUpdates.find( std::make_pair(csRX, csRY) );
    if( itr!=tileUpdates.end() )
        return &(itr->second);
    return nullptr;
}

int ScreenUpdater::addTileUpdate(int64_t csX, int64_t csY)
{
	int csRX = TileMap::getRegRXYZ(csX);
	int csRY = TileMap::getRegRXYZ(csY);
	int cssx = TileMap::getRegSubPos(csX);
	int cssy = TileMap::getRegSubPos(csY);

	t_tileUpdates::iterator itr = tileUpdates.find( std::make_pair(csRX, csRY) );
	if( itr!=tileUpdates.end() ) {
		//Add the update inside of an existing update region
		t_updates* upd = &(itr->second);
		upd->insert( std::make_pair(cssx, cssy) );
		return 0;
	} else {
		//Add the update region with the update.
		t_updates upd;
		upd.insert( std::make_pair(cssx, cssy) );
		tileUpdates.insert( std::make_pair(std::make_pair(csRX, csRY), upd ) );
		return 1;
	}
}

int ScreenUpdater::addTileUpdates(int64_t csX0, int64_t csY0, int64_t csX1, int64_t csY1)
{
	int result = 0;
	for( int icsX = csX0; icsX<=csX1; icsX++ ) {
		for( int icsY = csY0; icsY<=csY1; icsY++ ) {
			result += addTileUpdate(icsX, icsY);
		}
	}
	
	return result;
}

/*
 * Create a 3x3 of tile updates at the specified position
 */
int ScreenUpdater::addTileUpdates(int64_t csX, int64_t csY) { return addTileUpdates(csX-1, csY-1, csX+1, csY+1); }

/*
 * Fully update a region by ensuring that all tile columns in the region (32*32) get an update
 */ 
int ScreenUpdater::addRegionUpdate(int64_t csRX, int64_t csRY)
{
	//Find (rX, rY) within regUpdates, store in iterator
	t_regUpdates::iterator itr = regUpdates.find( std::make_pair(csRX, csRY) );
	
	//If (rX, rY) was not found, mark it for an update by adding it to regUpdates
	if( itr==regUpdates.end() ) {
		regUpdates.insert(std::make_pair(csRX, csRY));
		return 0;
	}

	//If the region exists in regUpdates (already marked for update), do nothing
	return -1;
}

/*
 * Stop updating a region by erasing all updates within it
 */
int ScreenUpdater::stopRegionUpdate(int64_t csRX, int64_t csRY)
{
	//Find (rX, rY) within regUpdates, store in iterator
	t_regUpdates::iterator itr = regUpdates.find( std::make_pair(csRX, csRY) );

	//If (rX, rY) exists, delete it.
	if( itr!=regUpdates.end() ) {
		regUpdates.erase(std::make_pair(csRX, csRY));
		return 0;
	}

	return -1;
}

/**
	Delete all elements in 'tileUpdates' and 'regUpdates,' which clears all cached updates.
 */
void ScreenUpdater::stopAllUpdates()
{
	tileUpdates.clear();
	regUpdates.clear();
}

/**
 	Mark every region that is loaded and onscreen as FINISHED_GENERATING.
 */
void ScreenUpdater::updateMapVisible(bool blackout, int loadDist)
{
	if(blackout) {
		stopAllUpdates();
	}

	//Iterate through all regions which should be loaded
	for(int64_t iRX = cam->getRX()-loadDist; iRX<=cam->getRX()+loadDist; iRX++) {
		for(int64_t iRY = cam->getRY()-loadDist; iRY<=cam->getRY()+loadDist; iRY++) {
			for(int64_t iRZ = cam->getRZ()-loadDist; iRZ<=cam->getRZ()+loadDist; iRZ++) {
				//If this region is visible
				if(RegTexInfo::isRegOnScreen(cam, iRX, iRY, iRZ)) {
					TileRegion* tr = tileMap->getRegByRXYZ(Vec3<int64_t>(iRX, iRY, iRZ));
					if(tr!=nullptr) {
						tr->setRegTexState(TileRegion::FINISHED_GENERATING);
					}

					if(blackout) {
						Texture* tex = csTileMap->getTex(iRX, iRY);
						RegTexProcessor::clearRegionArea(csTileMap, iRX, iRY);
					}
				}
			}
		}
	}
}

void ScreenUpdater::updateMapVisible(int loadDist) { updateMapVisible(false, loadDist); }

/**
	Load a given region as fast as the computer can handle it (based on loadCountMax, infoRegLoadTime).
*/
void ScreenUpdater::regPillarGenAttempt(FileHandler* fileHandler, int64_t csRX, int64_t csRY, int64_t csRZ, int loadDepth)
{
	//Timer for debugging
	nch::Timer rlt;

	//Check pillar of regions at (rX, rY)
	for(int64_t neg = -1; neg<=1; neg += 2) {
		for(int64_t diff = 0; diff<=loadDepth; diff++) {
			int64_t icsRZ = csRZ+neg*diff;
			
			//If new region load was successful (regions already loaded are not == 0)
			if( tileMap->loadRegionByCsRXYZ(cam, Vec3<int64_t>(csRX, csRY, icsRZ))==0 ) {
				//Increment loadCount
				loadCount++;

				//Set debug info
				infoRegLoadTimeLatest = rlt.getElapsedTimeMS();
				infoRegLoadCount++;
				infoRegLoadTime += infoRegLoadTimeLatest;

				//Exit out of function upon single successful region load
				return;
			}
		}
	}
}

void ScreenUpdater::updateMapToFINISHED_GENERATING(FileHandler* fileHandler, int loadDist)
{
	//Reset loadCount
	loadCount = 0;

	Grid grid0;
	grid0.updateOrderFinder(sdlHandler, tileMap, cam, loadDist, -1);
	std::pair<int, int> leXY = grid0.getLowestElementXY();
	if(leXY.first!=-1) {
		for(int i = 0; i<loadCountMax; i++)
		if(loadCount<=loadCountMax) {
			Grid grid;
			grid.updateOrderFinder(sdlHandler, tileMap, cam, loadDist, -1);
			std::pair<int, int> leXY = grid.getLowestElementXY();
			if(leXY.first!=-1) {
				int64_t csRX = cam->getCsRX()-loadDist+leXY.first;
				int64_t csRY = cam->getCsRY()-loadDist+leXY.second;
				
				regPillarGenAttempt(fileHandler, csRX, csRY, cam->getCsRZ(), loadDist);
			}
		}
	}



	/** Performance gauging */
	if(infoRegLoadCount>infoRegLoadDivisor) {
		//Calculate avg reg load time
		infoRegLoadTimeAvg = infoRegLoadTime/(double)infoRegLoadCount;
		//Reset regionsLoaded count and regionLoadTime
		infoRegLoadCount = 0;
		infoRegLoadTime = 0.0;
	}

	//Regulate loadCountMax depending on performance
	loadCountMax = std::ceil(allowedLoadTimeMS/infoRegLoadTimeAvg);	//allowedLoadTimeMS => a max of that # of ms on average that we should allow for loading of regions
	if(loadCountMax<1) loadCountMax = 1;
	if(loadCountMax>15) loadCountMax = 15;
}

void ScreenUpdater::updateMapToSHOULD_UPDATE(int loadDist)
{
	//Iterate through all regions which should be loaded
	for(int64_t icsRX = cam->getCsRX()-loadDist; icsRX<=cam->getCsRX()+loadDist; icsRX++) {
		for(int64_t icsRY = cam->getCsRY()-loadDist; icsRY<=cam->getCsRY()+loadDist; icsRY++) {
			int64_t icsRZ = cam->getCsRZ();
			//If this region is visible
			if(RegTexInfo::isCsRegOnScreen(cam, icsRX, icsRY)) {
				//Mark region as "SHOULD UPDATE" if a few conditions are met:
				// If that column of regions is "FINISHED GENERATING".
				// If that csTileMap texture is not nullptr.
				bool shouldUpdate = true;

				//If texture could not be found within canvas, shouldUpdate is automatically false.
				Texture* tex = csTileMap->getTex( icsRX, icsRY );
				if(tex==nullptr) {
					shouldUpdate = false;
				}

				//Check 3x3x3 area of regions centered on the current region.
				//If a region is found which is not FINISHED_GENERATING or which is nullptr => should not update.
				for(int64_t jcsRX = icsRX-1; jcsRX<=icsRX+1; jcsRX++) {
					for(int64_t jcsRY = icsRY-1; jcsRY<=icsRY+1; jcsRY++) {
						for(int64_t jcsRZ = icsRZ-1; jcsRZ<=icsRZ+1; jcsRZ++) {
							TileRegion* ttr = tileMap->getRegByCsRXYZ(cam, Vec3<int64_t>(jcsRX, jcsRY, jcsRZ));
							if(ttr==nullptr || ttr->getRegTexState()<TileRegion::FINISHED_GENERATING) {
								shouldUpdate = false;
							}
						}
					}
				}

				//If a tile region is FINISHED_GENERATING and shouldUpdate==true, mark it as SHOULD_UPDATE
				TileRegion* tr = tileMap->getRegByCsRXYZ(cam, Vec3<int64_t>(icsRX, icsRY, icsRZ));
				if(tr!=nullptr && shouldUpdate) {
					if(tr->getRegTexState()==TileRegion::FINISHED_GENERATING) {
						tr->setRegTexState(TileRegion::SHOULD_UPDATE);
					}
				}
			}
		}
	}
}

void ScreenUpdater::updateMapToUPDATED(int loadDist)
{
	for(int i = 0; i<2; i++) {
		Grid grid;
		grid.updateOrderFinder(sdlHandler, tileMap, cam, loadDist, TileRegion::RegTexState::UPDATED-1);
		std::pair<int, int> leXY = grid.getLowestElementXY();

		if(leXY.first!=-1) {
			int64_t csRX = cam->getCsRX()-loadDist+leXY.first;
			int64_t csRY = cam->getCsRY()-loadDist+leXY.second;
			TileRegion* tr = tileMap->getRegByCsRXYZ(cam, Vec3<int64_t>(csRX, csRY, cam->getCsRZ()));
			
			if(tr!=nullptr) {
				if(tr->getRegTexState()==TileRegion::SHOULD_UPDATE) {
					addRegionUpdate(csRX, csRY);
					tr->setRegTexState(tr->UPDATED);
				}
			}
		}
	}


}

void ScreenUpdater::updateMapMoved(FileHandler* fileHandler, int loadDist)
{
	int64_t camRX = cam->getRX();
	int64_t camRY = cam->getRY();
	int64_t camRZ = cam->getRZ();

	updateMapVisible(false, loadDist);

	/** Region unloading (nearby region immediately out of range) */
	int outlineH = loadDist+1;
	int outlineV = loadDist+1;

	for( int dRX = -outlineH; dRX<=outlineH; dRX++) {
		for( int dRY = -outlineH; dRY<=outlineH; dRY++) {
			//Get rX and rY
			int rX = camRX+dRX;
			int rY = camRY+dRY;
			//Set dRZ (delta RZ)
			int dRZ = outlineV*2;
			// If we are at a horizontal edge of the cubic area
			if( abs(dRX)==outlineH || abs(dRY)==outlineH ) { dRZ = 1; }

			for( int rZ = camRZ-outlineV; rZ<=camRZ+outlineV; rZ += dRZ ) {
				//Unload regions
				tileMap->saveRegion(Vec3<int64_t>(rX, rY, rZ));
				tileMap->unloadRegion(Vec3<int64_t>(rX, rY, rZ));
			}
		}
	}
}