#include "RegTexInfo.h"
#include <math.h>
#include <nch/cpp-utils/log.h>
#include "DebugScreen.h"
#include "RegTexBuilder.h"
#include "TileMapScreen.h"
#include "Timer.h"

void RegTexInfo::init(SDLHandler* sh, ScreenUpdater* su, Canvas* cs)
{
    RegTexInfo::sdlHandler = sh;
    RegTexInfo::tileMap = su->getTileMap();
	RegTexInfo::csTileMap = cs;

	RegTexInfo::cam = csTileMap->getCamera();

	regTexProcessor.init(sdlHandler, csTileMap, su);
}

RegTexInfo::~RegTexInfo(){}

/*
*
*/
std::vector<std::pair<int64_t, Tile>> RegTexInfo::camTilesToDrawHere(TileIterator& ti, int camDirection, int maxChecks)
{
	std::vector<std::pair<int64_t, Tile>> res;
	int sign = 1; if(camDirection%2==0) sign = -1;
	int axis = Camera::getAxisFromDirection(camDirection);

	int depth = -1;
	Tile ttfc;	//TrackedTileFromCamera
	for(int i = 0; i<maxChecks; i++) {
		switch(axis) {
			case Camera::X: ttfc = ti.peekTrackedTile(sign*i, 0, 0); break;
			case Camera::Y: ttfc = ti.peekTrackedTile(0, sign*i, 0); break;
			case Camera::Z: ttfc = ti.peekTrackedTile(0, 0, sign*i); break;
		}

		//If tile is transparent/translucent but not completely invisible
		if( !ttfc.isVisionBlocking(camDirection) && !ttfc.skipRendering ) {
			res.push_back(std::make_pair(i, ttfc));
		}
		
		//If tile is completely opaque
		if( ttfc.isVisionBlocking(camDirection) ) {
			res.push_back(std::make_pair(i, ttfc));
			break;
		}
	}
	
	return res;
}

std::pair<int64_t, Tile> RegTexInfo::camTopVisionBlockingTile(TileIterator& ti, int camDirection, int maxChecks)
{
	//Find top-most visionBlocking tile at current x, y
	int sign = 1; if(camDirection%2==0) sign = -1;
	int axis = Camera::getAxisFromDirection(camDirection);
	int depth = -1;
	Tile topTileFromCam;
	for(int i = 0; i<maxChecks; i++) {
		switch(axis) {
			case Camera::X: topTileFromCam = ti.peekTrackedTile(sign*i, 0, 0); break;
			case Camera::Y: topTileFromCam = ti.peekTrackedTile(0, sign*i, 0); break;
			case Camera::Z: topTileFromCam = ti.peekTrackedTile(0, 0, sign*i); break;
		}
		
		if( topTileFromCam.isVisionBlocking(camDirection) ) { depth = i; break; }
	}
	
	return std::make_pair( depth, topTileFromCam );
}

std::pair<int64_t, Tile> RegTexInfo::camTopSolidTile(TileIterator& ti, int camDirection, int maxChecks)
{
	//Find top-most solid tile at current x, y
	int sign = 1; if(camDirection%2==0) sign = -1;
	int axis = Camera::getAxisFromDirection(camDirection);
	int depth = -1;
	Tile topTileFromCam;
	bool foundNew = false;
	for(int i = 0; i<maxChecks; i++) {
		switch(axis) {
			case Camera::X: { topTileFromCam = ti.peekTrackedTile(sign*i, 0, 0); } break;
			case Camera::Y: { topTileFromCam = ti.peekTrackedTile(0, sign*i, 0); } break;
			case Camera::Z: { topTileFromCam = ti.peekTrackedTile(0, 0, sign*i); } break;
		}
		
		if( topTileFromCam.solid ) { depth = i; break; }
	}
	
	return std::make_pair( depth, topTileFromCam );
}


std::pair<int64_t, Tile> RegTexInfo::camTopVisionBlockingTile(TileIterator& ti, int camDirection)
{
	return camTopVisionBlockingTile(ti, camDirection, 32);
}


/*
 * Depending on the TileIterator's (ti) position (x/y), find the top tile visible from the camera as if it were looking down.
 */
std::pair<int64_t, Tile> RegTexInfo::topVisionBlockingTile(TileIterator& ti)
{
	return camTopVisionBlockingTile(ti, Camera::DOWN);
}

/*
 * Populate a stringstream with this class's info.
 */
void RegTexInfo::info(std::stringstream& ss, int& tabs)
{
	//DebugScreen::indentLine(ss, tabs);
	//ss << rtUpdatesToDo << " rtUpdates (time elapsed)=" << rtUpdatesTime << "ms; ";
	//DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "Total time taken by rtUpdates (avg, last second)=" << rtUpdatesTimeAvg << "ms; ";
	DebugScreen::newLine(ss);
	
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
bool RegTexInfo::isRegOnScreen(Camera* cam, int64_t rX, int64_t rY, int64_t rZ)
{
	int64_t csRX = rX;
	int64_t csRY = rY;
	switch(cam->getAxis()) {
		case Camera::X: { csRX = rY; csRY = rZ; } break;
		case Camera::Y: { csRX = rX; csRY = rZ; } break;
	}

	return isCsRegOnScreen(cam, csRX, csRY);	
}

bool RegTexInfo::isCsRegOnScreen(Camera* cam, int64_t csRX, int64_t csRY)
{
	//Check if this region at (rX, rY) is onscreen (vertical Z coordinate doesn't matter). Store in 'onScreen'.
	bool onScreen = false;
	if( csRX >= cam->getCsRX()-cam->getScreenSemiWidthReg() &&
		csRX <= cam->getCsRX()+cam->getScreenSemiWidthReg() &&
		csRY >= cam->getCsRY()-cam->getScreenSemiHeightReg() &&
		csRY <= cam->getCsRY()+cam->getScreenSemiHeightReg() ) {
		onScreen = true;
	}
	
	//Return result
	return onScreen;
}


Camera* RegTexInfo::getCamera() { return cam; }
Canvas* RegTexInfo::getTileMapCanvas() { return csTileMap; }

void RegTexInfo::updateTimeAvg(int drawsThisSecond)
{
	rtUpdatesTimeAvg = rtUpdatesTimeTotal/drawsThisSecond;  //Calculate average regTexUpdate time over the last second
	rtUpdatesTimeTotal = 0.0;                               //Reset regTexUpdate total time
}

void RegTexInfo::draw(int64_t camSRX, int64_t camSRY, int64_t camSRZ, int loadDistH)
{
    if( csTileMap==nullptr ) return;

	// Set canvas texture radius to screen radius
	csTileMap->setTexAllocRadiusX(cam->getScreenSemiWidthReg()+2);
	csTileMap->setTexAllocRadiusY(cam->getScreenSemiHeightReg()+2);

	//Perform 'rtUpdates' number of RegTexUpdates and do performance gauging
	rtUpdatesTime = 0.0;
	{
		std::stringstream ss; ss << "Performing " << rtUpdatesToDo;
		nch::Timer t(ss.str(), false);
		
		regTexProcessor.processRegions(cam, loadDistH, rtUpdatesToDo);
		
		rtUpdatesTime = t.getElapsedTimeMS();
	}
	rtUpdatesTimeTotal += rtUpdatesTime;
}

void RegTexInfo::drawDebugOverlay(Canvas* csDebug, int64_t csRX, int64_t csRY, int64_t csRZ, int loadDist)
{
	csDebug->setTexAllocRadiusX(loadDist);
	csDebug->setTexAllocRadiusY(loadDist);
	csDebug->setTexAllocCount(10);

	//Iterate through all loaded regions within the camera's region layer.
	for(int64_t icsRX = csRX-loadDist; icsRX<csRX+loadDist; icsRX++) {
		for(int64_t icsRY = csRY-loadDist; icsRY<csRY+loadDist; icsRY++) {
				//Get TileRegion
				TileRegion* tr = tileMap->getRegByCsRXYZ(cam, icsRX, icsRY, csRZ);

				//Get fill color which depends on the TileRegion's current regTexState
				nch::Color fillColor(255, 255, 255);	//White == nullptr
				if(tr!=nullptr) {
					switch(tr->getRegTexState()) {
						case TileRegion::NONE: 					fillColor = nch::Color(255, 127, 255); break;	//Pink
						case TileRegion::UNGENERATED: 			fillColor = nch::Color(255,   0,   0); break;	//Red
						case TileRegion::GENERATING: 			fillColor = nch::Color(255, 255,   0); break;	//Yellow
						case TileRegion::FINISHED_GENERATING: 	fillColor = nch::Color(  0, 255,   0); break;	//Green
						case TileRegion::SHOULD_UPDATE: 		fillColor = nch::Color(  0, 255, 255); break;	//Cyan
						case TileRegion::UPDATED: 				fillColor = nch::Color(  0,   0, 255); break;	//Blue
					}
				}
				
				//Fill the region with a translucent version of the chosen color
				regTexProcessor.colorFillRegionArea(csDebug, icsRX, icsRY, fillColor.r, fillColor.g, fillColor.b, 80);
		}
	}
}