#include "RegTexUpdates.h"
#include <math.h>
#include "DebugScreen.h"
#include "Log.h"
#include "RegTexBuilder.h"
#include "TileMapScreen.h"
#include "Timer.h"

RegTexUpdates::RegTexUpdates(SDLHandler* sdlHandler, Camera* camera, TileMap* tileMap, Canvas* csTileMap)
{
    RegTexUpdates::sdlHandler = sdlHandler;
    RegTexUpdates::camera = camera;
    RegTexUpdates::tileMap = tileMap;
    RegTexUpdates::csTileMap = csTileMap;
}

RegTexUpdates::~RegTexUpdates(){}

/*
 * 
 *	6 possible values for direction: [012345] = [WENSUD]
 * 	West/East 	= -X/+X
 * 	North/South = -Y/+Y
 * 	Up/Down 	= -Z/+Z
 */
std::tuple<int64_t, TileType> RegTexUpdates::camTrackedTile(TileIterator& ti, int camDirection)
{
	int sign = 1;
	if( camDirection%2==0 ) {
		sign = -1;
	}

	//0=X, 1=Y, 2=Z
	int axis = camDirection/2;
	
	//Find top-most tile at current x, y
	int depth = 0;
	TileType topTileFromCam;
	for(int i = 0; i<32; i++) {
		
		switch( axis ) {
		case 0: topTileFromCam = ti.peekTrackedTile(sign*i, 0, 0); break;
		case 1: topTileFromCam = ti.peekTrackedTile(0, sign*i, 0); break;
		case 2: topTileFromCam = ti.peekTrackedTile(0, 0, sign*i); break;
		default: {
			Log::error(__PRETTY_FUNCTION__, "Invalid axis", "returning null tile");
		} break;
		}
		
		if( topTileFromCam.isVisionBlocking() ) {
			depth = i;
			break;
		}
	}
	
	//Return tuple:
	//1st object: equal to the depth of the top tile from the camera (topmost would be = 0)
	//2nd object: TileType which represents the top tile from the camera.
	return std::make_tuple( depth, topTileFromCam );
}

std::tuple<int64_t, TileType> RegTexUpdates::topTrackedTile(TileIterator& ti)
{
	return camTrackedTile(ti, 5);
}

void RegTexUpdates::info(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << rtUpdatesToDo << " rtUpdates (time elapsed)=" << rtUpdatesTime << "ms; ";
	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "Total time taken by rtUpdates (avg, last second)=" << rtUpdatesTimeAvg << "ms; ";
	DebugScreen::newLine(ss);
	
	DebugScreen::indentLine(ss, tabs);
	ss << "Screen(w, h)=(" << screenWT << ", " << screenHT << "); ";
	ss << "Screen(wR, hR)=(" << screenWR << ", " << screenHR << "); ";
	DebugScreen::newLine(ss);
}

int RegTexUpdates::getScreenSemiWidth() { return screenSWR; }
int RegTexUpdates::getScreenSemiHeight() { return screenSHR; }
bool RegTexUpdates::isOnScreen(int rX, int rY)
{
	//Get camRX and camRY
	int camRX = floor( camera->getX()/TileMapScreen::regionSize );
	int camRY = floor( camera->getY()/TileMapScreen::regionSize );
	
	//Check if this region at (rX, rY) is onscreen (vertical Z coordinate doesn't matter). Store in 'onScreen'.
	bool onScreen = false;
	if( rX >= camRX-getScreenSemiWidth() &&
		rX <= camRX+getScreenSemiWidth() &&
		rY >= camRY-getScreenSemiHeight() &&
		rY <= camRY+getScreenSemiHeight() )
	{
		onScreen = true;
	}
	
	//Return result
	return onScreen;
}
int RegTexUpdates::getTileScale() { return tileScale; }
int RegTexUpdates::getBlitScale() { return blitScale; }

void RegTexUpdates::setScreenInfo()
{
	// Store screen width (in tiles). Is always at least 1
	screenWT = sdlHandler->getWidth()/tileScale;
	if( screenWT==0 ) screenWT = 1;
	// Store screen height (in tiles). Is always at least 1
	screenHT = sdlHandler->getHeight()/tileScale;
	if( screenHT==0 ) screenHT = 1;

	// Store screen width and semi width (in regions).
	screenWR = screenWT/TileMapScreen::regionSize+1;
	screenSWR = screenWR/2+1;
	// Store screen height and semi height (in regions).
	screenHR = screenHT/TileMapScreen::regionSize+1;
	screenSHR = screenHR/2+1;
}

void RegTexUpdates::updateTimeAvg(int drawsThisSecond)
{
	rtUpdatesTimeAvg = rtUpdatesTimeTotal/drawsThisSecond;  //Calculate average regTexUpdate time over the last second
	rtUpdatesTimeTotal = 0.0;                               //Reset regTexUpdate total time
}

void RegTexUpdates::updateScaling(int ts, int bs)
{
	tileScale = ts;
	blitScale = bs;
}

void RegTexUpdates::draw(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH)
{
	// Set canvas texture radius to screen radius
	if( csTileMap==nullptr ) return;
	csTileMap->setTexAllocRadiusX(screenSWR+1);
	csTileMap->setTexAllocRadiusY(screenSHR+1);

	//Perform 'rtUpdates' number of regTexUpdates and do performance gauging
	rtUpdatesTime = 0.0;
	{
		std::stringstream ss; ss << "Performing " << rtUpdatesToDo << " regTexUpdates";
		Timer t(ss.str(), false);
		processRegions(camRX, camRY, camRZ, loadRadiusH);
		rtUpdatesTime = t.getElapsedTimeMS();
	}
	rtUpdatesTimeTotal += rtUpdatesTime;
}

/**
    Debugging tool: useful for seeing where regions of interest are on the current screen.
*/
void RegTexUpdates::colorFillRegionArea(int64_t rX, int64_t rY, uint8_t r, uint8_t g, uint8_t b)
{
	/** Get 'tex' located at this region from csTileMap */
	Texture* tex = csTileMap->getTex(rX, rY);
	if(tex==nullptr) {
		return;
	}

	// Fill the entire texture (AKA region area) with black (or the appropriate bkgd color)
	tex->rect(0, 0, blitScale*32, blitScale*32, r, g, b);
}

/**
    Blacks out regions that can't be seen anymore
*/
void RegTexUpdates::blackOutRegionArea(int64_t rX, int64_t rY) { colorFillRegionArea(rX, rY, 0, 0, 0); }

void RegTexUpdates::placeEntireScreen()
{
	// Stop all updates
	tileMap->stopAllUpdates();

	// Place updates @ every tile position on screen.
	int margin = 4;
	tileMap->addTileUpdates(
		camera->getX()-(double)screenWT/2.0-margin,
		camera->getY()-(double)screenHT/2.0-margin,
		camera->getLayer(),
		camera->getX()+(double)screenWT/2.0+margin,
		camera->getY()+(double)screenHT/2.0+margin,
		camera->getLayer()
	);
}

void RegTexUpdates::processRegions(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH)
{
	if( csTileMap==nullptr || camera==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

	TileIterator ti(tileMap);
	ti.logWarnings(false);
	ti.setTrackerMode(ti.FULL);

	for( int outline = 0; outline<=loadRadiusH; outline++ ) {
		//Iterate dRX (delta RX) from -outline to +outline
		for( int dRX = -outline; dRX<=outline; dRX++ ) {
			int dRY = outline*2;                                //Set dRY (delta RY)
			if( abs(dRX)==outline ) { dRY = 1; }
			int rX = camRX+dRX;                                 //Get rX
			//Get rY: Iterate from camRY-outline to camRY+outline based on dRY
			for( int rY = camRY-outline; rY<=camRY+outline; rY += dRY ) {
				//If this region at (rX, rY) is onscreen (vertical coordinate doesn't matter)
				if( rX>=camRX-screenSWR && rX<=camRX+screenSWR ) {
					if( rY>=camRY-screenSHR && rY<=camRY+screenSHR ) {
						//Update regtex at this regionlayer
						processRegionLayer(ti, rX, rY, camRZ, camera->getLayer());
					}
				}
			}
		}
	}

}

void RegTexUpdates::processRegions2(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH)
{
    if( csTileMap==nullptr || camera==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

    TileIterator ti(tileMap);
    ti.logWarnings(false);
    ti.setTrackerMode(ti.FULL);
}

void RegTexUpdates::processRegionLayer(TileIterator& ti, int64_t rX, int64_t rY, int64_t camRZ, int64_t camLayer)
{
    int numUpdates = 0;
    int sz = TileMap::getRegSubPos(camLayer);

    /** Get 'rtu' (region texture updates vector) located at this region */
    TileMap::t_updates* rtu = tileMap->getRTUsByRXYz(rX, rY, camLayer);
    //If 'rtu' is null or rtu->size()==0
    if(rtu==nullptr || rtu->size()==0) {
        //Make sure to delete this update collection.
        tileMap->getUpdatesMap()->erase(std::make_tuple(rX, rY, camLayer));
        return;
    }

    /** Get 'tex' located at this region from csTileMap */
    Texture* tex = csTileMap->getTex(rX, rY);
    //If 'tex' is null
    if(tex==nullptr) return;

    /** Iterate through updates */
    //Set bounds of the TileIterator to this region
    ti.setBoundsByRXYZ(rX, rY, camRZ);
    //Go through each individual update in 'rtu' and perform the regTexUpdate().
    for( TileMap::t_updates::iterator itr = rtu->begin(); itr!=rtu->end(); itr=rtu->begin() ) {
        //Increment updates.
        numUpdates++;
        //Set TileIterator to the position of the rtu
        ti.setTrackerSub( itr->first, itr->second, sz );
        //Perform the region texture update.
        processTileArea(ti, tex);
        //Erase this rtu
        rtu->erase(itr);
        //If max number of updates has been reached, stop the method
        if(numUpdates>=rtUpdatesToDo) return;
    }
}

void RegTexUpdates::processTileArea(TileIterator& ti, Texture* tex)
{
	//If tex is nullptr for some reason, stop method
	if( tex==nullptr ) return;

	//Get data of the top tile from the camera
	auto tttData = topTrackedTile(ti);              //Pair of a t_ll object and a TileType object
	int64_t dZ = std::get<0>(tttData);        //Relative height of top tile ( topmost==0, one below==-1, etc. )
	TileType topTileFromCam = std::get<1>(tttData); //TileType of top tile from camera

	//Create RegTexBuilder
	RegTexBuilder rtb(tex, ti, blitScale, topTileFromCam, dZ);
}
