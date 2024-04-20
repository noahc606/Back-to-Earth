#include "TileMapUpdater.h"
#include "DebugScreen.h"
#include "Grid.h"
#include "RegTexUpdater.h"
#include "Timer.h"

void TileMapUpdater::init(SDLHandler* sh, TileMap* tm, Canvas* cs)
{
	sdlHandler = sh;
	tileMap = tm;
    csTileMap = cs;
	cam = csTileMap->getCamera();
}

void TileMapUpdater::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "Region Loading Time (avg out of last " << infoRegLoadDivisor << ")=" << infoRegLoadTimeAvg << "ms; ";
	DebugScreen::newLine(ss);
}

TileMap* TileMapUpdater::getTileMap() { return tileMap; }

TileMapUpdater::t_regUpdates* TileMapUpdater::getRegUpdates() { return &regUpdates; }
TileMapUpdater::t_tileUpdates* TileMapUpdater::getTileUpdates() { return &tileUpdates; }

/*
 * Return the set of all tile updates (set<pair<int64_t, int64_t>>) within a specific region. If region doesn't exist, return nullptr.
 */
TileMapUpdater::t_updates* TileMapUpdater::getTUsByRXY(int64_t rX, int64_t rY)
{
    t_tileUpdates::iterator itr = tileUpdates.find( std::make_pair(rX, rY) );
    if( itr!=tileUpdates.end() )
        return &(itr->second);
    return nullptr;
}

int TileMapUpdater::addTileUpdate(int64_t x, int64_t y)
{
	int rX = TileMap::getRegRXYZ(x);
	int rY = TileMap::getRegRXYZ(y);
	int sx = TileMap::getRegSubPos(x);
	int sy = TileMap::getRegSubPos(y);

	t_tileUpdates::iterator itr = tileUpdates.find( std::make_pair(rX, rY) );
	if( itr!=tileUpdates.end() ) {
		//Add the update inside of an existing update region
		t_updates* upd = &(itr->second);
		upd->insert( std::make_pair(sx, sy) );
		return 0;
	} else {
		//Add the update region with the update.
		t_updates upd;
		upd.insert( std::make_pair(sx, sy) );
		tileUpdates.insert( std::make_pair(std::make_pair(rX, rY), upd ) );
		return 1;
	}
}

int TileMapUpdater::addTileUpdates(int64_t x0, int64_t y0, int64_t x1, int64_t y1)
{
	int result = 0;
	for( int ix = x0; ix<=x1; ix++ ) {
		for( int iy = y0; iy<=y1; iy++ ) {
			result += addTileUpdate(ix, iy);
		}
	}
	
	return result;
}

/*
 * Create a 3x3 of tile updates at the specified position
 */
int TileMapUpdater::addTileUpdates(int64_t x, int64_t y)
{
    return addTileUpdates(x-1, y-1, x+1, y+1);
}

/*
 * Fully update a region by ensuring that all tile columns in the region (32*32) get an update
 */ 
int TileMapUpdater::addRegionUpdate(int64_t rX, int64_t rY)
{
	//Find (rX, rY) within regUpdates, store in iterator
	t_regUpdates::iterator itr = regUpdates.find( std::make_pair(rX, rY) );
	
	//If (rX, rY) was not found, mark it for an update by adding it to regUpdates
	if( itr==regUpdates.end() ) {
		regUpdates.insert(std::make_pair(rX, rY));
		return 0;
	}

	//If the region exists in regUpdates (already marked for update), do nothing
	return -1;
}

/*
 * Stop updating a region by erasing all updates within it
 */
int TileMapUpdater::stopRegionUpdate(int64_t rX, int64_t rY)
{
	//Find (rX, rY) within regUpdates, store in iterator
	t_regUpdates::iterator itr = regUpdates.find( std::make_pair(rX, rY) );

	//If (rX, rY) exists, delete it.
	if( itr!=regUpdates.end() ) {
		regUpdates.erase(std::make_pair(rX, rY));
		return 0;
	}

	return -1;
}

/**
	Delete all elements in 'tileUpdates' and 'regUpdates,' which clears all cached updates.
 */
void TileMapUpdater::stopAllUpdates()
{
	tileUpdates.clear();
	regUpdates.clear();
}

/**
 	Mark every region that is loaded and onscreen for an update.
 */
void TileMapUpdater::updateMapVisible(bool blackout, int loadDistH, int loadDistV)
{
	//Iterate through all regions which should be loaded
	for(int64_t iRX = cam->getRX()-loadDistH; iRX<=cam->getRX()+loadDistH; iRX++) {
		for(int64_t iRY = cam->getRY()-loadDistH; iRY<=cam->getRY()+loadDistH; iRY++) {
			for(int64_t iRZ = cam->getRZ()-loadDistV; iRZ<=cam->getRZ()+loadDistV; iRZ++) {
				//If this region is visible
				if(RegTexUpdater::isRegOnScreen(sdlHandler, cam, iRX, iRY, iRZ)) {
					TileRegion* tr = tileMap->getRegByRXYZ(iRX, iRY, iRZ);
					if(tr!=nullptr && tr->getRegTexState()==TileRegion::UPDATED) {
						tr->setRegTexState(TileRegion::SHOULD_UPDATE);
					}
				}
			}
		}
	}
}

void TileMapUpdater::updateMapVisible(int loadDistH, int loadDistV) { updateMapVisible(false, loadDistH, loadDistV); }

void TileMapUpdater::updateMap103(int loadDistH, int loadDistV)
{
	//Iterate through all regions which should be loaded
	for(int64_t iRX = cam->getRX()-loadDistH; iRX<=cam->getRX()+loadDistH; iRX++) {
		for(int64_t iRY = cam->getRY()-loadDistH; iRY<=cam->getRY()+loadDistH; iRY++) {
			for(int64_t iRZ = cam->getRZ()-loadDistV; iRZ<=cam->getRZ()+loadDistV; iRZ++) {
				//If this region is visible
				if(RegTexUpdater::isRegOnScreen(sdlHandler, cam, iRX, iRY, iRZ)) {
					//Mark region as "SHOULD UPDATE" if a few conditions are met:
					// If that column of regions is "FINISHED GENERATING".
					// If that csTileMap texture is not nullptr.
					bool shouldUpdate = true;

					Texture* tex = csTileMap->getTex( iRX, iRY );
					if(tex==nullptr) {
						shouldUpdate = false;
					}

					int fg = TileRegion::FINISHED_GENERATING;

					for(int jRX = iRX-1; jRX<=iRX+1; jRX++) {
						for(int jRY = iRY-1; jRY<=iRY+1; jRY++) {
							for(int jRZ = iRZ-1; jRZ<=iRZ+1; jRZ++) {
								TileRegion* ttr = tileMap->getRegByRXYZ(jRX, jRY, jRZ);
								if(ttr==nullptr || ttr->getRegTexState()<fg) shouldUpdate = false;
							}
						}
					}

					TileRegion* tr = tileMap->getRegByRXYZ(iRX, iRY, iRZ);
					if(shouldUpdate) {
						if(tr->getRegTexState()==fg) {
							tr->setRegTexState(TileRegion::SHOULD_UPDATE);
						}
					}
				}
			}
		}
	}
}

void TileMapUpdater::updateMap104(int loadDistH, int loadDistV)
{
	Grid grid(sdlHandler, tileMap, cam, loadDistH, loadDistV, TileRegion::RegTexState::UPDATED-1);
	std::pair<int, int> leXY = grid.getLowestElementXY();

	if(leXY.first!=-1) {
		int64_t rX = cam->getRX()-loadDistH+leXY.first;
		int64_t rY = cam->getRY()-loadDistH+leXY.second;
		
		TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, cam->getRZ());
		if(tr!=nullptr) {
			if(tr->getRegTexState()==TileRegion::SHOULD_UPDATE) {
				addRegionUpdate(rX, rY);
				tr->setRegTexState(tr->UPDATED);
			}
		}
	}
}

/**
	Load a given region as fast as the computer can handle it (based on loadCountMax, infoRegLoadTime).
*/
void TileMapUpdater::updateRegTicked(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t camRZ, int loadDistV)
{
	//Load region if loadCount<loadCountMax
	int loadCount = 0;
	if( loadCount<loadCountMax ) {
		//Timer for debugging
		Timer rlt;

		//If new region load was successful (regions already loaded are not == 0)
		if( tileMap->loadRegion(fileHandler, rX, rY, camRZ)==0 ) {
			//Increment loadCount
			loadCount++;

			//Load all regions within this column (relative to camera depth) that are within the loadDistV.
			for(int iRZ = camRZ-loadDistV; iRZ<=camRZ+loadDistV; iRZ++) {
				tileMap->loadRegion(fileHandler, rX, rY, iRZ);
			}
			
			//Set debug info
			infoRegLoadTimeLatest = rlt.getElapsedTimeMS();
			infoRegLoadCount++;
			infoRegLoadTime += infoRegLoadTimeLatest;
		}
	}
}

void TileMapUpdater::updateMapTicked(FileHandler* fileHandler, int loadDistH, int loadDistV)
{
	//Try to load the nearest null regions
	Grid grid(sdlHandler, tileMap, cam, loadDistH, loadDistV, -1);
	std::pair<int, int> leXY = grid.getLowestElementXY();
	if(leXY.first!=-1) {
		int64_t rX = cam->getRX()-loadDistH+leXY.first;
		int64_t rY = cam->getRY()-loadDistH+leXY.second;
		
		updateRegTicked(fileHandler, rX, rY, cam->getRZ(), loadDistV);
	}

	/** Performance gauging */
	if(infoRegLoadCount>infoRegLoadDivisor) {
		//Calculate avg reg load time
		infoRegLoadTimeAvg = infoRegLoadTime/(double)infoRegLoadCount;
		//Reset regionsLoaded count and regionLoadTime
		infoRegLoadCount = 0;
		infoRegLoadTime = 0.0;
	}
}

void TileMapUpdater::updateMapMoved(FileHandler* fileHandler, std::string currentDimPath, int loadDistH, int loadDistV)
{
	int64_t camRX = cam->getRX();
	int64_t camRY = cam->getRY();
	int64_t camRZ = cam->getRZ();

	updateMapVisible(false, loadDistH, loadDistV);

	/** Region unloading (nearby region immediately out of range) */
	int outlineH = loadDistH+1;
	int outlineV = loadDistV+1;

	for( int dRX = -outlineH; dRX<=outlineH; dRX++) {
		for( int dRY = -outlineH; dRY<=outlineH; dRY++) {
			//Get rX and rY
			int rX = camRX+dRX;
			int rY = camRY+dRY;
			//Set dRZ (delta RZ)
			int dRZ = outlineV*2;
			// If we are at a horizontal edge of the cubic area
			if( abs(dRX)==outlineH || abs(dRY)==outlineH ) {
				dRZ = 1;
				//regTexUpdates->blackOutRegionArea(rX, rY);
			}

			for( int rZ = camRZ-outlineV; rZ<=camRZ+outlineV; rZ += dRZ ) {
				//Unload regions
				tileMap->saveRegion(fileHandler, "world1", rX, rY, rZ);
				tileMap->unloadRegion(fileHandler, currentDimPath, rX, rY, rZ);
			}
		}
	}

	/** Region priority calculation (horizontal manhattan distance from camera region) */
	TileMap::t_regionMap::iterator itrRM = tileMap->getRegionMap()->begin();
	while( itrRM!=tileMap->getRegionMap()->end() ) {
		/** Null pointer check */
		//If this TileRegion is null, stop this iteration and continue to the next one
		TileRegion* tr = &itrRM->second;
		if(tr==nullptr) {
			itrRM++;
			continue;
		}

		//Calculate region's 2D manhattan distance, where the 2 dimensions are perpendicular to the camera's direction (by default, down)
		int rX = std::get<0>(itrRM->first);
		int rY = std::get<1>(itrRM->first);
		int manhattanDist = std::abs(camRX-rX) + std::abs(camRY-rY);
		//Set the TR's Region Texture update priority to be the above value.
		tr->setRegTexPriority(manhattanDist);
		//Increment iterator
		itrRM++;
	}
}

