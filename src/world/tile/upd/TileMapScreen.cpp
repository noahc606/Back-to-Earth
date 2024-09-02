#include "TileMapScreen.h"
#include <iomanip>
#include <iostream>
#include <math.h>
#include <nch/cpp-utils/io/Log.h>
#include <nch/sdl-utils/Timer.h>
#include <sstream>
#include "Controls.h"
#include "DebugScreen.h"
#include "LevelSave.h"
#include "Main.h"
#include "RegTexBuilder.h"
#include "RegTexInfo.h"
#include "SDLHandler.h"
#include "TileIterator.h"

void TileMapScreen::init(SDLHandler* sh, FileHandler* fh, TileMap* tm, Canvas* cs)
{
	//Utils
	sdlHandler = sh;
	fileHandler = fh;
	tileMap = tm;
	csTileMap = cs;
	cam = csTileMap->getCamera();

	//TileMapUpdater: Governs the addition/removal of tile updates and region updates. Also reponsible for loading and unloading of regions.
	tileMapUpdater.init(sdlHandler, tileMap, csTileMap);
	//RegTexInfo: This object tracks useful information
	regTexInfo.init(sdlHandler, &tileMapUpdater, csTileMap);

	//tileMapUpdater.updateMapMoved(fileHandler, currentDimPath, loadDistH, loadDistV);
	tileMapUpdater.updateMapVisible(loadDist);
}

void TileMapScreen::destroy()
{
	nch::Log::log("Destroying tileMapScreen...");
	
	//Build a list of all tile region locations
	std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> regs;
	TileMap::t_regionMap::iterator itrRM;
	
	if( tileMap==nullptr || tileMap->getRegionMap()==nullptr ) {
		return;
	}
	for( itrRM = tileMap->getRegionMap()->begin(); itrRM!=tileMap->getRegionMap()->end(); itrRM++ ) {
		int rX = std::get<0>(itrRM->first);
		int rY = std::get<1>(itrRM->first);
		int rZ = std::get<2>(itrRM->first);
		regs.push_back( std::make_tuple(rX, rY, rZ) );
	}
	
	//Remove and unload all known tile regions
	for( int i = 0; i<regs.size(); i++ ) {
		auto thisReg = regs.at(i);
		uint64_t rX = std::get<0>(thisReg);
		uint64_t rY = std::get<1>(thisReg);
		uint64_t rZ = std::get<2>(thisReg);
		
		TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, rZ);
		tileMap->saveRegion(fileHandler, rX, rY, rZ);
		tileMap->unloadRegion(fileHandler, rX, rY, rZ);
	}
	
	// Get rid of RegTexUpdates object when this is destroyed
	tileMapUpdater.stopAllUpdates();
	//delete regTexUpdates;
}

TileMapScreen::~TileMapScreen() { destroy(); }

void TileMapScreen::tick()
{
	// Every time TMS is ticked, we need to decide what part of the map to update
	bool doUpdMapVisible = false;   // 1. Update entire screen when zoom is changed or when camera layer is changed (most expensive)
	bool doUpdMapTicked = true;     // 2. Map updates to do every tick
	bool doUpdMapMoved = false;     // 3. Map updates to do when crossing a region border: Unload regions that just went out of player range
	bool doUpdMapIdle = false;      // 4. Map updates to do while idle: add updates twice a second at an onscreen region with lowest priority
	bool doUpdMapAutosave = false;  // 5. Map updates to do every autosave (30s): Check all regions to see if they should be unloaded -> save to disk.

	bool doMapBlackout = false;


	if(cam==nullptr) {
		return;
	}

	// Track tick time (start at 0.0)
	infoTickTime = 0.0;
	nch::Timer localTimer;
	{
		// Update cam and regTexUpdates
		// Get camera info
		int cRX = floor( cam->getX()/regionSize );
		int cRY = floor( cam->getY()/regionSize );
		int cRZ = floor( cam->getZ()/regionSize );

		// If current cam region is different than the last, update camRX, camRY and camRZ.
		if( cRX!=camRX || cRY!=camRY || cRZ!=camRZ ) {
			// Update camRXYZ
			camRX = cRX;
			camRY = cRY;
			camRZ = cRZ;
			doUpdMapMoved = true;
		}

		//If current cam layer is different than the last, update camL and update the entire screen.
		int cL = cam->getLayer();
		if( cL!=camL ) {
			// Update camL
			camL = cL;
			doUpdMapVisible = true;
			doMapBlackout = false;
		}
		
		//If camera direction has changed...
		int cD = cam->getDirection();
		if(cD!=camD) {
			camD = cD;
			doUpdMapVisible = true;
			doMapBlackout = true;
		}

		//If camera zoom has changed, update camZoom, tileScale, the entire screen, and blitScalePx.
		if( cam->getZoom()!=camZoom ) {
			// Update camZoom
			camZoom = cam->getZoom();
			doUpdMapVisible = true;
		}
		
		if( daySegment!=tileMap->getPlanet()->getPartOfDay(24) ) {
			daySegment = tileMap->getPlanet()->getPartOfDay(24);
			doUpdMapVisible = true;
		}

		// Check if idle map updates should be done
		if( true ) {
			umiTicks++;
			if( umiTicks>=2 ) {
				doUpdMapIdle = true;
				umiTicks = 0;
			}
		}
		
		// Update different parts of the map under different conditions
		// Make sure to do it in the order of 1->2->3->4 (most expensive to least)
		if( doUpdMapVisible )   tileMapUpdater.updateMapVisible(doMapBlackout, loadDist);
		if( doUpdMapTicked )    tileMapUpdater.updateMapToFINISHED_GENERATING(fileHandler, loadDist);
		if( doUpdMapMoved )     tileMapUpdater.updateMapMoved(fileHandler, loadDist);
		if( doUpdMapIdle )      tileMapUpdater.updateMapToSHOULD_UPDATE(loadDist);
		if( doUpdMapIdle )      tileMapUpdater.updateMapToUPDATED(loadDist);
		//if( doUpdMapAutosave )  {}
	}
	// Get elapsed time in MS
	infoTickTime = localTimer.getElapsedTimeMS();
}

void TileMapScreen::draw()
{
	// Keep track of draw time
	infoDrawTime = 0.0;
	nch::Timer localTimer;
	{
		//draw: RegTexUpdater
		regTexInfo.draw(camRX, camRY, camRZ, loadDist);

		//draw: tileMap canvas
		csTileMap->draw();
	}
	//Track draw time
	infoDrawTime = localTimer.getElapsedTimeMS();

	//Increment drawsThisSecond
	drawsThisSecond++;
	
	//Run every second
	if( SDL_GetTicks()>nextSecond ) {
		regTexInfo.updateTimeAvg(drawsThisSecond);

		nextSecond += 1000;             //Update when the nextSecond is
		drawsThisSecond = 0;            //Reset drawsThisSecond
	}
}

void TileMapScreen::drawDebugOverlay(Canvas* csInteractions)
{
	regTexInfo.drawDebugOverlay(csInteractions, cam->getCsRX(), cam->getCsRY(), cam->getCsRZ(), loadDist);
}

void TileMapScreen::putInfo(std::stringstream& ss, int& tabs, int64_t mouseX, int64_t mouseY, int64_t mouseZ, bool mouseExists)
{
	if( cam==nullptr ) return;

	//Performance profiling
	DebugScreen::newGroup(ss, tabs, "Performance");
		DebugScreen::indentLine(ss, tabs);
		ss << "Tick time=" << infoTickTime << "ms; ";
		DebugScreen::newLine(ss);
		DebugScreen::indentLine(ss, tabs);
		ss << "Draw time=" << infoDrawTime << "ms; ";
		DebugScreen::newLine(ss);

		DebugScreen::newGroup(ss, tabs, "TileMapUpdater");
			tileMapUpdater.putInfo(ss, tabs);
		DebugScreen::endGroup(tabs);
	DebugScreen::endGroup(tabs);

	//Camera
	DebugScreen::newGroup(ss, tabs, "Camera");
		//Zoom, scale
		DebugScreen::indentLine(ss, tabs);
		ss << "zoom=" << camZoom << "; ";
		ss << "direction=" << cam->getDirectionString() << "; ";
		//ss << "tileScale=" << regTexUpdates->getTileScale() << "px; ";
		//ss << "blitScale=" << regTexUpdates->getBlitScale() << "px; ";
		DebugScreen::newLine(ss);

		//Location in world
		DebugScreen::indentLine(ss, tabs);
		ss << "XYZ=(" << cam->getX() << std::setprecision(5) << ", " << cam->getY() << ", " << cam->getZ() << "); ";
		ss << "RXYZ=(" << camRX << ", " << camRY << ", " << camRZ << "); ";
		ss << "Layer=" << cam->getLayer() << "; ";
		DebugScreen::newLine(ss);
	DebugScreen::endGroup(tabs);

	//Mouse
	if( mouseExists ) {
		
	}
	
	DebugScreen::newGroup(ss, tabs, "TileRegion");
		long selRX = TileMap::getRegRXYZ(mouseX);
		long selRY = TileMap::getRegRXYZ(mouseY);
		long selRZ = TileMap::getRegRXYZ(mouseZ);

		TileRegion* tr = tileMap->getRegByRXYZ(selRX, selRY, selRZ);
		int msx = TileMap::getRegSubPos(mouseX);
		int msy = TileMap::getRegSubPos(mouseY);
		int msz = TileMap::getRegSubPos(mouseZ);
		if( mouseExists ) {
			//Location in world
			DebugScreen::indentLine(ss, tabs);
			ss << "Selected Tile [" << msx << "][" << msy << "][" << msz << "]";
			ss << " in TileRegion(" << selRX << ", " << selRY << ", " << selRZ << "); ";
			DebugScreen::newLine(ss);
			
			// RegTex info
			DebugScreen::newGroup(ss, tabs, "Selected TileRegion");
			if(tr!=nullptr) {
				//TileRegion info
				tr->putInfo(ss, tabs, msx, msy, msz);
			} else {
				DebugScreen::indentLine(ss, tabs);
				ss << "Looking at a null TileRegion";
				DebugScreen::newLine(ss);
			}
			DebugScreen::endGroup(tabs);
		}

		// RegTexUpdates
		DebugScreen::newGroup(ss, tabs, "TileMapUpdater");
			DebugScreen::indentLine(ss, tabs);
			ss << "numRegUpdates=" << tileMapUpdater.getRegUpdates()->size() << "; ";
			int updateCount = 0;
			for( TileMapUpdater::t_tileUpdates::iterator itrTU = tileMapUpdater.getTileUpdates()->begin(); itrTU!=tileMapUpdater.getTileUpdates()->end(); itrTU++ ) {
				updateCount += itrTU->second.size();
			}
			ss << "numTileUpdates=" << updateCount << "; ";
			DebugScreen::newLine(ss);
		DebugScreen::endGroup(tabs);
		
		if(mouseExists) {
			DebugScreen::newGroup(ss, tabs, "RegTexBuilder");
				//Top tile info
				DebugScreen::indentLine(ss, tabs);
				TileIterator ti(tileMap);
				ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseX), TileMap::getRegRXYZ(mouseY), TileMap::getRegRXYZ(cam->getLayer()) );
				ti.setTrackerSub( msx, msy, TileMap::getRegSubPos(cam->getLayer()) );
				auto tttData = RegTexInfo::camTrackedTile(ti, cam->getDirection());
				int64_t dZ = std::get<0>(tttData);
				ss << "Top tile(dZ, Z)=(" << dZ << ", " << (ti.getTrackerSub(2)+dZ) << "); ";
				DebugScreen::newLine(ss);
				//RTB info
				RegTexBuilder::info(ss, tabs, ti, dZ);
				//CsTileMap
			DebugScreen::endGroup(tabs);
		}

	DebugScreen::endGroup(tabs);
}

void TileMapScreen::putInfo(std::stringstream& ss, int& tabs, int64_t mouseX, int64_t mouseY, int64_t mouseZ)
{
	putInfo(ss, tabs, mouseX, mouseY, mouseZ, true);
}

void TileMapScreen::putInfo(std::stringstream& ss, int& tabs)
{
	putInfo(ss, tabs, 0, 0, 0, false);
}

TileMapUpdater* TileMapScreen::getUpdater()
{
	return &tileMapUpdater;
}