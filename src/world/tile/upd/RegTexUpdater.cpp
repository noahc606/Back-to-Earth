#include "RegTexUpdater.h"
#include <math.h>
#include "DebugScreen.h"
#include "Log.h"
#include "RegTexBuilder.h"
#include "TileMapScreen.h"
#include "Timer.h"

void RegTexUpdater::init(SDLHandler* sh, TileMapUpdater* tmu, Canvas* cs)
{
    RegTexUpdater::sdlHandler = sh;
    RegTexUpdater::tileMap = tmu->getTileMap();
	RegTexUpdater::csTileMap = cs;

	RegTexUpdater::cam = csTileMap->getCamera();

	regTexProcessor.init(sdlHandler, csTileMap, tmu);
}

RegTexUpdater::~RegTexUpdater(){}

/*
 * Depending on the TileIterator's (ti) position, find the top tile that is visible by the camera assuming it is looking in the 'camDirection' direction.
 */
std::pair<int64_t, TileType> RegTexUpdater::camTrackedTile(TileIterator& ti, int camDirection)
{
	int sign = 1;
	if( camDirection%2==0 ) {
		sign = -1;
	}

	//0=X, 1=Y, 2=Z
	int axis = Camera::getAxisFromDirection(camDirection);
	
	//Find top-most tile at current x, y
	int depth = 0;
	TileType topTileFromCam;
	for(int i = 0; i<32; i++) {
		
		switch( axis ) {
			case Camera::X: topTileFromCam = ti.peekTrackedTile(sign*i, 0, 0); break;
			case Camera::Y: topTileFromCam = ti.peekTrackedTile(0, sign*i, 0); break;
			case Camera::Z: topTileFromCam = ti.peekTrackedTile(0, 0, sign*i); break;
		}
		
		if( topTileFromCam.isVisionBlocking() ) {
			depth = i;
			break;
		}
	}
	
	//Return tuple:
	//1st object: equal to the depth of the top tile from the camera (topmost would be = 0)
	//2nd object: TileType which represents the top tile from the camera.
	return std::make_pair( depth, topTileFromCam );
}

/*
 * Depending on the TileIterator's (ti) position (x/y), find the top tile visible from the camera as if it were looking down.
 */
std::pair<int64_t, TileType> RegTexUpdater::topTrackedTile(TileIterator& ti)
{
	return camTrackedTile(ti, Camera::DOWN);
}

/*
 * Populate a stringstream with this class's info.
 */
void RegTexUpdater::info(std::stringstream& ss, int& tabs)
{
	//DebugScreen::indentLine(ss, tabs);
	//ss << rtUpdatesToDo << " rtUpdates (time elapsed)=" << rtUpdatesTime << "ms; ";
	//DebugScreen::newLine(ss);
	//DebugScreen::indentLine(ss, tabs);
	//ss << "Total time taken by rtUpdates (avg, last second)=" << rtUpdatesTimeAvg << "ms; ";
	//DebugScreen::newLine(ss);
	
	DebugScreen::indentLine(ss, tabs);
	ss << "Screen(w, h)=(" << cam->getScreenWidthTiles() << ", " << cam->getScreenHeightTiles() << "); ";
	ss << "Screen(wR, hR)=(" << cam->getScreenWidthReg() << ", " << cam->getScreenHeightReg() << "); ";
	DebugScreen::newLine(ss);
}

/*
 * Get whether a given region is on the screen.
 * 
 * Specifically:
 * Whichever axis is currently used for depth is not taken into consideration (example: camera looking up/down -> depth axis is Z).
 * Thus, only two of the coordinates matter in determining whether a region is considered "onscreen" (the ones that represent horizontal and vertical from the user's perspective).
 */
bool RegTexUpdater::isRegOnScreen(SDLHandler* sh, Camera* cam, int64_t rX, int64_t rY, int64_t rZ)
{
	//Get camRXZ
	int64_t camRX = cam->getRX();
	int64_t camRY = cam->getRY();
	int64_t camRZ = cam->getRZ();
	int64_t axis = cam->getAxis();
	
	int64_t sRX = rX;
	int64_t sRY = rY;
	int64_t camSRX = camRX;
	int64_t camSRY = camRY;
	
	switch(axis) {
		case Camera::X: {
			camSRX = camRY;
			camSRY = camRZ;
			sRX = rY;
			sRY = rZ;
		} break;
		case Camera::Y: {
			camSRX = camRX;
			camSRY = camRZ;
			sRX = rX;
			sRY = rZ;
		}
	}
	
	//Check if this region at (rX, rY) is onscreen (vertical Z coordinate doesn't matter). Store in 'onScreen'.
	bool onScreen = false;
	if( sRX >= camSRX-cam->getScreenSemiWidthReg() &&
		sRX <= camSRX+cam->getScreenSemiWidthReg() &&
		sRY >= camSRY-cam->getScreenSemiHeightReg() &&
		sRY <= camSRY+cam->getScreenSemiHeightReg() ) {
		onScreen = true;
	}
	
	//Return result
	return onScreen;
}

Camera* RegTexUpdater::getCamera() { return cam; }
Canvas* RegTexUpdater::getTileMapCanvas() { return csTileMap; }

void RegTexUpdater::updateTimeAvg(int drawsThisSecond)
{
	rtUpdatesTimeAvg = rtUpdatesTimeTotal/drawsThisSecond;  //Calculate average regTexUpdate time over the last second
	rtUpdatesTimeTotal = 0.0;                               //Reset regTexUpdate total time
}

void RegTexUpdater::draw(int64_t camSRX, int64_t camSRY, int64_t camSRZ, int loadRadiusH)
{
    if( csTileMap==nullptr ) return;

	// Set canvas texture radius to screen radius
	csTileMap->setTexAllocRadiusX(cam->getScreenSemiWidthReg()+2);
	csTileMap->setTexAllocRadiusY(cam->getScreenSemiHeightReg()+2);

	//Perform 'rtUpdates' number of RegTexUpdates and do performance gauging
	rtUpdatesTime = 0.0;
	{
		std::stringstream ss; ss << "Performing " << rtUpdatesToDo << " RegTexUpdater";
		Timer t(ss.str(), false);
		
		regTexProcessor.processRegions(cam, loadRadiusH, rtUpdatesToDo);
		
		rtUpdatesTime = t.getElapsedTimeMS();
	}
	rtUpdatesTimeTotal += rtUpdatesTime;
}

void RegTexUpdater::drawDebugOverlay(Canvas* csInteractions, int64_t camSRX, int64_t camSRY, int64_t camSRZ, int loadRadiusH)
{
	csInteractions->setTexAllocRadiusX(loadRadiusH);
	csInteractions->setTexAllocRadiusY(loadRadiusH);

	//Iterate through all loaded regions within the camera's region layer.
	for(int64_t irx = camSRX-loadRadiusH; irx<camSRX+loadRadiusH; irx++) {
		for(int64_t iry = camSRY-loadRadiusH; iry<camSRY+loadRadiusH; iry++) {
				//Get TileRegion
				TileRegion* tr = tileMap->getRegByRXYZ(irx, iry, camSRZ);

				//Get fill color which depends on the TileRegion's current regTexState
				Color fillColor(50, 50, 50);
				if(tr!=nullptr) {
					switch(tr->getRegTexState()) {
						case TileRegion::NONE: 					fillColor = Color(255, 127, 255); break;	//Pink
						case TileRegion::UNGENERATED: 			fillColor = Color(255,   0,   0); break;	//Red
						case TileRegion::GENERATING: 			fillColor = Color(255, 255,   0); break;	//Yellow
						case TileRegion::FINISHED_GENERATING: 	fillColor = Color(  0, 255,   0); break;	//Green
						case TileRegion::SHOULD_UPDATE: 		fillColor = Color(  0, 255, 255); break;	//Cyan
						case TileRegion::UPDATED: 				fillColor = Color(  0,   0, 255); break;	//Blue
					}
				}
				
				Texture infoCoords;

				infoCoords.init(sdlHandler);
								
				//TextOld infoText;
				//infoText.init(sdlHandler);
				//std::stringstream ss; ss << "(" << irx << ", " << iry << ", " << camSRZ << ")";
				//infoText.setString(ss.str());

				//csInteractions->setSourceTex(infoText.)
				//csInteractions->rcopy();

				//Fill the region with a translucent version of the chosen color
				regTexProcessor.colorFillRegionArea(csInteractions, irx, iry, fillColor.r, fillColor.g, fillColor.b, 80);
		}
	}
}

/*
 * Update all onscreen tile positions.
 */
void RegTexUpdater::placeEntireScreen()
{
	// Stop all updates
	//tileMap->stopAllUpdates();



	// Place updates @ every tile position on screen.
	int margin = 4;
	
	double c1 = cam->getX();
	double c2 = cam->getY();
	switch(cam->getAxis()) {
		case Camera::X: {
			c1 = cam->getY();
			c2 = cam->getZ();
		} break;
		case Camera::Y: {
			c1 = cam->getX();
			c2 = cam->getZ();
		} break;
	}
	
	//tileMap->addTileUpdates(
		//c1-(double)screenWT/2.0-margin,
		//c2-(double)screenHT/2.0-margin,
		//c1+(double)screenWT/2.0+margin,
		//c2+(double)screenHT/2.0+margin
	//);
}