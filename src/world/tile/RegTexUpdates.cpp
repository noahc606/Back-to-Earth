#include "RegTexUpdates.h"
#include <math.h>
#include "DebugScreen.h"
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

RegTexUpdates::~RegTexUpdates()
{
    //dtor
}

std::tuple<Defs::t_ll, Defs::t_ll, TileType> RegTexUpdates::topTrackedTile(TileIterator& ti)
{
    int height = 0;

    //Find top-most tile at current x, y
    TileType topTileFromCam;
    for(int i = 0; i<32; i++) {
        topTileFromCam = ti.peekTrackedTile(0, 0, i);
        if( topTileFromCam.isVisionBlocking() ) {
            height = i;
            break;
        }
    }

    //Return pair of two objects:
    //1st object: t_ll equal to the height of the top tile from the camera (topmost would be = 0)
    //2nd object: Depth of the toptile from the camera
    //3nd object: TileType which represents the top tile from the camera.
    return std::make_tuple( height, -height, topTileFromCam );
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

void RegTexUpdates::draw(Defs::t_ll camRX, Defs::t_ll camRY, Defs::t_ll camRZ, int loadRadiusH)
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
        processRegion(camRX, camRY, camRZ, loadRadiusH);
        rtUpdatesTime = t.getElapsedTimeMS();
    }
    rtUpdatesTimeTotal += rtUpdatesTime;

    //double rtUpdatesTimeMax = 6;
    //if(rtUpdatesTime < rtUpdatesTimeMax) {
        //Increase rtUpdatesToDo if rtUpdatesTimeMax is NOT exceeded
        //rtUpdatesToDo = (double)rtUpdatesToDo*rtUpdatesTimeMax/rtUpdatesTime;
    //} else {
        //Decrease rtUpdatesToDo if rtUpdatesTimeMax IS exceeded
        //rtUpdatesToDo = (double)rtUpdatesToDo*rtUpdatesTimeMax/rtUpdatesTime;
    //}

    //int rtUpdatesLimit = 512;
    //if(rtUpdatesToDo>rtUpdatesLimit) rtUpdatesToDo = rtUpdatesLimit;
    //placeRandom(rtUpdatesToDo/2);
}

/**
    Debugging tool: useful for seeing where regions of interest are on the current screen. Pretty bussin poggers tbqh famalam frfr no cap ongod in ohio
*/
void RegTexUpdates::colorFillRegionArea(Defs::t_ll rX, Defs::t_ll rY, uint8_t r, uint8_t g, uint8_t b)
{
    /** Get 'tex' located at this region from csTileMap */
    Texture* tex = csTileMap->getTex(rX, rY);
    // If 'tex' is null
    if(tex==nullptr) {
        return;
    }

    // Fill the entire texture (AKA region area) with black (or the appropriate bkgd color)
    tex->rect(0, 0, blitScale*32, blitScale*32, r, g, b);
}

/**
    Blacks out regions that can't be seen anymore (if region load distance is really low)
*/
void RegTexUpdates::blackOutRegionArea(Defs::t_ll rX, Defs::t_ll rY)
{
    colorFillRegionArea(rX, rY, 0, 0, 0);
}

void RegTexUpdates::placeEntireScreen()
{
    // Stop all updates
    tileMap->stopAllUpdates();

    // Place updates @ every tile position on screen.
    int margin = 4;

    for(int dx = -screenWT/2-margin; dx<screenWT/2+margin; dx++) {
        for(int dy = -screenHT/2-margin; dy<screenHT/2+margin; dy++) {
            tileMap->addTileUpdate( camera->getX()+dx, camera->getY()+dy, camera->getLayer());
        }
    }
}

/**
    Place regTexUpdates randomly throughout the map on any tile that is onscreen
*/
void RegTexUpdates::placeRandom(int updates)
{
    return;
    if(tileMap==nullptr) return;

    TileMap::t_ll scrRW2T = screenWT/4; if( scrRW2T==0 ) scrRW2T = 1;
    TileMap::t_ll scrRH2T = screenWT/4; if( scrRH2T==0 ) scrRH2T = 1;
    TileMap::t_ll scrRW4T = screenWT/8;
    TileMap::t_ll scrRH4T = screenHT/8;
    TileMap::t_ll scrCX = floor( camera->getX()/4 )*4;
    TileMap::t_ll scrCY = floor( camera->getY()/4 )*4;

    for( int i = 0; i<updates/16+1; i++ ) {

        TileMap::t_ll updatesX = scrCX-4*scrRW4T+4*(rand())%(4*scrRW2T);
        TileMap::t_ll updatesY = scrCY-4*scrRH4T+4*(rand())%(4*scrRH2T);

        for(TileMap::t_ll sx = 0; sx<4; sx++) {
            for(TileMap::t_ll sy = 0; sy<4; sy++) {
                tileMap->addTileUpdate( updatesX+sx, updatesY+sy, camera->getLayer() );
            }
        }
    }
}

void RegTexUpdates::processRegion(Defs::t_ll camRX, Defs::t_ll camRY, Defs::t_ll camRZ, int loadRadiusH)
{
    if( csTileMap==nullptr || camera==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

    TileIterator ti(tileMap);
    ti.logWarnings(false);
    ti.setTrackerMode(ti.FULL);

    /*
    ti.setBoundsByRXYZ(camRX, camRY, camRZ);
    //processRegionLayer(csTileMap, ti, -1, -1, camRZ, camera->getLayer());
    int sz = TileMap::getRegSubPos( camera->getLayer() );
    Texture* tex = csTileMap->getTex(camRX, camRY);
    if(tex==nullptr) {
        std::cout << "tex is null!\n";
        return;
    }

    std::cout << camRX << ", " << camRY << "\n";

    for(int x = 0; x<32; x++) {
        for(int y = 0; y<32; y++) {
            tex->lock(x*32, y*32, 32, 32);
            tex->blit(TextureLoader::WORLD_TILE_type_a, 32, 0, 32, 32);
        }
    }
    tex->draw();
    return;
    */

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

void RegTexUpdates::processRegionLayer(TileIterator& ti, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll camRZ, Defs::t_ll camLayer)
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
    TileMap::t_ll dZ = std::get<0>(tttData);        //Relative height of top tile ( topmost==0, one below==-1, etc. )
    TileType topTileFromCam = std::get<2>(tttData); //TileType of top tile from camera

    //Get the destination of texture blit on the canvas region
    int dstX = blitScale*ti.getTrackerSub(0);  //blit scale*X
    int dstY = blitScale*ti.getTrackerSub(1);  //blit scale*Y


    /*
    tex->lock(dstX, dstY, blitScalePx, blitScalePx);
    int r = std::get<0>(topTileFromCam.getRGB());
    int g = std::get<1>(topTileFromCam.getRGB());
    int b = std::get<2>(topTileFromCam.getRGB());
    tex->setColorMod(r, g, b);
    tex->fill();
    */

    RegTexBuilder rtb(tex, ti, dstX, dstY, blitScale, topTileFromCam, dZ);
}
