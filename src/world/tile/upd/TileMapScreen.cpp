#include "TileMapScreen.h"
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include "Controls.h"
#include "DebugScreen.h"
#include "LevelSave.h"
#include "Log.h"
#include "Main.h"
#include "RegTexBuilder.h"
#include "RegTexUpdater.h"
#include "SDLHandler.h"
#include "TileIterator.h"
#include "Timer.h"

void TileMapScreen::init(SDLHandler* sh, FileHandler* fh, TileMap* tm, Canvas* cs)
{
	//Utils
	sdlHandler = sh;
	fileHandler = fh;
	tileMap = tm;
	csTileMap = cs;
	cam = csTileMap->getCamera();

	minimap.init(sdlHandler, 128, 128);
	minimap.lock();
	minimap.setColorMod(0, 0, 128);
	minimap.fill();
	minimap.setDrawScale(2);

	//TileMapUpdater: Governs the addition/removal of tile updates and region updates. Also reponsible for loading and unloading of regions.
	tileMapUpdater.init(sdlHandler, tileMap, cam);
	//RegTexUpdater: This object processes updates which decide where tiles/region blits need to happen. To do this, this object owns a RegTexProcessor.
	regTexUpdater.init(sdlHandler, &tileMapUpdater, csTileMap);

	//tileMapUpdater.updateMapMoved(fileHandler, currentDimPath, loadRadiusH, loadRadiusV);
	tileMapUpdater.updateMapVisible(loadRadiusH, loadRadiusV);
}

void TileMapScreen::destroy()
{
	Log::log("Destroying tileMapScreen...");
	
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
		tileMap->saveRegion(fileHandler, "world1", rX, rY, rZ);
		tileMap->unloadRegion(fileHandler, currentDimPath, rX, rY, rZ);
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
	bool doMapBlackout = false;
	bool doUpdMapIdle = false;      // 4. Map updates to do while idle: add updates twice a second at an onscreen region with lowest priority
	bool doUpdMapAutosave = false;  // 5. Map updates to do every autosave (30s): Check all regions to see if they should be unloaded -> save to disk.

	if(cam==nullptr) {
		return;
	}

	// Track tick time (start at 0.0)
	infoTickTime = 0.0;
	Timer localTimer;
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
			doMapBlackout = true;
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

			// Calculate screen width and screen height
			//regTexUpdates->setScreenInfo();

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
		if( doUpdMapVisible )   tileMapUpdater.updateMapVisible(doMapBlackout, loadRadiusH, loadRadiusV);
		if( doUpdMapTicked )    tileMapUpdater.updateMapTicked(fileHandler, loadRadiusH, loadRadiusV);
		if( doUpdMapMoved )     tileMapUpdater.updateMapMoved(fileHandler, currentDimPath, loadRadiusH, loadRadiusV);
		if( doUpdMapIdle )      tileMapUpdater.updateMapIdle(loadRadiusH, loadRadiusV);
		if( doUpdMapAutosave )  tileMapUpdater.updateMapAutosave(fileHandler, currentDimPath, loadRadiusH, loadRadiusV);
	}
	// Get elapsed time in MS
	infoTickTime = localTimer.getElapsedTimeMS();
}

void TileMapScreen::draw()
{
	// Keep track of draw time
	infoDrawTime = 0.0;
	Timer localTimer;
	{
		//draw: RegTexUpdater
		regTexUpdater.draw(camRX, camRY, camRZ, loadRadiusH);

		//draw: tileMap canvas
		csTileMap->draw();
	}
	//Track draw time
	infoDrawTime = localTimer.getElapsedTimeMS();

	//Increment drawsThisSecond
	drawsThisSecond++;
	//When SDL_GetTicks() exceeds nextSecond
	if( SDL_GetTicks()>nextSecond ) {
		regTexUpdater.updateTimeAvg(drawsThisSecond);

		//Update minimap
		//updateMinimap();

		//regTexProcessor.buildRegionArea( cam->getRX(), cam->getRY() );

		nextSecond += 1000;             //Update when the nextSecond is
		drawsThisSecond = 0;            //Reset drawsThisSecond
	}

	minimap.setDrawPos( sdlHandler->getWidth()-minimap.getTexWidth()*2, 0 );
	minimap.draw();
}

void TileMapScreen::drawDebugOverlay(Canvas* csInteractions)
{
	regTexUpdater.drawDebugOverlay(csInteractions, camRX, camRY, camRZ, loadRadiusH);
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
				auto tttData = RegTexUpdater::camTrackedTile(ti, cam->getDirection());
				int64_t dZ = std::get<0>(tttData);
				ss << "Top tile(dZ, Z)=(" << dZ << ", " << (ti.getTrackerSub(2)+dZ) << "); ";
				DebugScreen::newLine(ss);
				//RTB info
				RegTexBuilder::info(ss, tabs, ti, dZ);
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

void TileMapScreen::updateMinimap()
{
	minimap.lock();
	minimap.setColorMod(0, 0, 0);
	minimap.fill();

	TileIterator ti(tileMap);
	ti.setTrackerMode(TileIterator::RegionTrackingModes::FULL);

	int rad = 64;
	int64_t cMinX = cam->getLayer(0)-rad;
	int64_t cMinY = cam->getLayer(1)-rad;
	int64_t cMaxX = cam->getLayer(0)+rad;
	int64_t cMaxY = cam->getLayer(1)+rad;

	//Set bounds
	int status = ti.setBounds(
		cMinX, cMinY, cam->getLayer(2),
		cMaxX, cMaxY, cam->getLayer(2)
	);
	if(status==-1) {
		return;
	}

	//Loop through every region within bounds
	for(TileRegion* tr = ti.peekRegion(); !ti.atEnd(); ti.nextRegion()) {
		if(tr==nullptr) {
			continue;
		}

		//Loop through 32x32x1 area, using tracker to get the upmost tile at each pos
		for(int sx = 0; sx<32; sx++) {
			for(int sy = 0; sy<32; sy++) {
				ti.setTrackerSub(sx, sy, TileMap::getRegSubPos(cam->getLayer(2)));

				auto ctt = RegTexUpdater::topTrackedTile(ti);
				int64_t depth = ctt.first;
				TileType tt = ctt.second;
				auto ttRGB = tt.getRGB();

				minimap.pixel(
					ti.getItrReg(0)*32-cMinX+sx,
					ti.getItrReg(1)*32-cMinY+sy,
					std::get<0>(ttRGB),
					std::get<1>(ttRGB),
					std::get<2>(ttRGB)
				);
			}
		}
	}
}