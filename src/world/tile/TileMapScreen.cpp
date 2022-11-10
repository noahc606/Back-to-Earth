#include "TileMapScreen.h"
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include "Controls.h"
#include "DebugScreen.h"
#include "Log.h"
#include "Main.h"
#include "RegTexBuilder.h"
#include "SDLHandler.h"
#include "TileIterator.h"
#include "Timer.h"

void TileMapScreen::init( SDLHandler* sh, FileHandler* fh, Controls* ctrls, TileMap* p_tm, Player* p_pl )
{
    BTEObject::init(sh, fh, ctrls);

    tileMap = p_tm;

    if( p_pl!=nullptr ) {
        cam = p_pl->getCamera();
    }
}

void TileMapScreen::destroy()
{
}

TileMapScreen::~TileMapScreen() { destroy(); }

void TileMapScreen::tick()
{
    tickTime = 0.0;
    Timer localTimer;

    //Calculate screen width and screen height (in tiles)
    setScreenInfo();

    if( cam==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr )
        return;

    //Get current cam region
    int cRX = floor( cam->getX()/regionSize );
    int cRY = floor( cam->getY()/regionSize );
    int cRZ = floor( ((double)cam->getZ())/regionSize );
    int cL = cam->getLayer();

    //If current cam region is different than the last, update camRX, camRY and camRZ.
    if( cRX!=camRX || cRY!=camRY || cRZ!=camRZ ) {
        camRX = cRX;
        camRY = cRY;
        camRZ = cRZ;
    }

    if( cL!=camL ) {
        camL = cL;
        updateEntireScreen();
    }

    if( mapZoom!=cam->getZoom() ) {
        //Find tile scale
        mapZoom = cam->getZoom();
        tileScale = tileSize*mapZoom;

        updateEntireScreen();
    }

    //Map updates (regionMap, updatesMap)
    std::string* cmd = Commands::getString("stopMapUpdates");
    if( cmd!=nullptr ) {} else {
        mapUpdates();
    }

    //Translations depending on window width/height. Ensures that current camera's location is at the center of the window.
    //screenTX = sdlHandler->getWidth()/2;
    //screenTY = sdlHandler->getHeight()/2;

    tickTime = localTimer.getElapsedTimeMS();
}

void TileMapScreen::draw(Canvas* csTileMap)
{
    drawTime = 0.0;
    Timer localTimer;

    if( csTileMap==nullptr ) return;

    csTileMap->setTexAllocRadiusX(screenRWR+1);
    csTileMap->setTexAllocRadiusY(screenRHR+1);

    //Perform 'rtUpdates' number of regTexUpdates and do performance gauging
    rtUpdatesTime = 0.0;
    {
        std::stringstream ss; ss << "Performing " << rtUpdatesToDo << " regTexUpdates";
        Timer t(ss.str(), false);
        regTexUpdates(csTileMap, rtUpdatesToDo);
        rtUpdatesTime = t.getElapsedTimeMS();
    }
    rtUpdatesTimeTotal += rtUpdatesTime;

    double rtUpdatesTimeMax = 6;
    if(rtUpdatesTime < rtUpdatesTimeMax) {
        //Increase rtUpdatesToDo if rtUpdatesTimeMax is NOT exceeded
        rtUpdatesToDo = (double)rtUpdatesToDo*rtUpdatesTimeMax/rtUpdatesTime;
    } else {
        //Decrease rtUpdatesToDo if rtUpdatesTimeMax IS exceeded
        rtUpdatesToDo = (double)rtUpdatesToDo*rtUpdatesTimeMax/rtUpdatesTime;
    }

    int rtUpdatesLimit = 512;
    if(rtUpdatesToDo>rtUpdatesLimit) rtUpdatesToDo = rtUpdatesLimit;

    idleRegTexUpdates(rtUpdatesToDo/2);

    //Increment drawsThisSecond
    drawsThisSecond++;
    //When SDL_GetTicks() exceeds nextSecond
    if( SDL_GetTicks()>nextSecond ) {
        rtUpdatesTimeAvg = rtUpdatesTimeTotal/drawsThisSecond;    //Calculate average regTexUpdate time over the last second
        nextSecond += 1000;             //Update when the nextSecond is
        drawsThisSecond = 0;            //Reset drawsThisSecond
        rtUpdatesTimeTotal = 0.0;    //Reset regTexUpdate total time
    }

    csTileMap->draw();

    drawTime = localTimer.getElapsedTimeMS();
}

void TileMapScreen::info(std::stringstream& ss, int& tabs, TileMap::t_ll mouseX, TileMap::t_ll mouseY, TileMap::t_ll mouseZ)
{
    if( cam==nullptr ) return;

    //Performance profiling
    DebugScreen::newGroup(ss, tabs, "Performance");
        DebugScreen::indentLine(ss, tabs);
        ss << "Tick time=" << tickTime << "ms; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "Draw time=" << drawTime << "ms; ";
        DebugScreen::newLine(ss);

        DebugScreen::indentLine(ss, tabs);
        ss << rtUpdatesToDo << " rtUpdates (time elapsed)=" << rtUpdatesTime << "ms; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "Total time taken by rtUpdates (avg, last second)=" << rtUpdatesTimeAvg << "ms; ";
        DebugScreen::newLine(ss);

        DebugScreen::indentLine(ss, tabs);
        ss << "regLoadCount=" << regLoadCount << "; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "regLoadTime (latest)=" << regLoadTimeLatest << "ms; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "regLoadTime (avg out of last " << regLoadDivisor << ")=" << regLoadTimeAvg << "ms; ";
        DebugScreen::newLine(ss);

    DebugScreen::endGroup(tabs);

    //Camera
    DebugScreen::newGroup(ss, tabs, "Camera");
        //Zoom, scale
        DebugScreen::indentLine(ss, tabs);
        ss << "Screen(w, h)=(" << screenWT << ", " << screenHT << "); ";
        ss << "Screen(wR, hR)=(" << screenWR << ", " << screenHR << "); ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "Zoom=" << mapZoom << "; ";
        ss << "Tile Scale(px)=" << tileScale << "; ";
        DebugScreen::newLine(ss);

        //Location in world
        DebugScreen::indentLine(ss, tabs);
        ss << "Camera(rX, rY, rZ)=(" << camRX << ", " << camRY << ", " << camRZ << "); ";
        ss << "Camera(x, y, z)=(" << cam->getX() << std::setprecision(5) << ", " << cam->getY() << ", " << cam->getZ() << "); ";
        ss << "Camera Layer=" << cam->getLayer() << "; ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //Mouse
    DebugScreen::newGroup(ss, tabs, "Region Texture");

        int msx = TileMap::getRegSubPos(mouseX);
        int msy = TileMap::getRegSubPos(mouseY);
        int msz = TileMap::getRegSubPos(mouseZ);

        //Top tile info
        DebugScreen::indentLine(ss, tabs);
        TileIterator ti(tileMap);
        ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseX), TileMap::getRegRXYZ(mouseY), TileMap::getRegRXYZ(cam->getLayer()) );

        ti.setTrackerSub( msx, msy, TileMap::getRegSubPos(cam->getLayer()) );
        auto tttData = topTrackedTile(ti);
        TileMap::t_ll dZ = std::get<0>(tttData);

        ss << "Top tile(dZ)=" << dZ << ";";
        DebugScreen::newLine(ss);

        DebugScreen::newGroup(ss, tabs, "RegTexBuilder");
            DebugScreen::indentLine(ss, tabs);
            ss << "Tile (x, y, z)=(" << ti.getTrackerSub(0) << ", " << ti.getTrackerSub(1) << ", " << ti.getTrackerSub(2)+dZ << ");";
            DebugScreen::newLine(ss);
            RegTexBuilder::info(ss, tabs, ti, dZ);
        DebugScreen::endGroup(tabs);

        //Location in world
        DebugScreen::indentLine(ss, tabs);
        ss << "RegTex(rX, rY, rZ)=(" << TileMap::getRegRXYZ(mouseX) << ", " << TileMap::getRegRXYZ(mouseY) << ", " << TileMap::getRegRXYZ(mouseZ) << "); ";
        ss << "RegTex(x, y, z)=(" << mouseX << ", " << mouseY << ", " << mouseZ << "); ";
        ss << "RegTex(sx, sy, sz)=(" << msx << ", " << msy << ", " << msz << "); ";
        DebugScreen::newLine(ss);

    DebugScreen::endGroup(tabs);
}

std::tuple<TileMap::t_ll, TileMap::t_ll, TileType> TileMapScreen::topTrackedTile(TileIterator& ti)
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

void TileMapScreen::setScreenInfo()
{
    screenWT = sdlHandler->getWidth()/tileScale;
    if( screenWT==0 ) screenWT = 1;
    screenWR = screenWT/regionSize+1;
    screenRWR = screenWR/2+1;

    screenHT = sdlHandler->getHeight()/tileScale;
    if( screenHT==0 ) screenHT = 1;
    screenHR = screenHT/regionSize+1;
    screenRHR = screenHR/2+1;
}

void TileMapScreen::mapUpdates()
{
    if( cam==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

    int maxRegionInserts = 3;
    int numRegionInserts = 0;

    /** Region loading */
    //Load Regions, RegTexes, nearby in a ring
    int radius = 7;
    int verticalRadius = 4;

    for( int ring = 0; ring<=radius; ring++ ) {
        for( int deltaRX = -ring; deltaRX<=ring; deltaRX++ ) {
            //Set deltaRY
            int deltaRY = ring*2;
            if( abs(deltaRX)==ring ) { deltaRY = 1; }
            //Get rX
            int rX = camRX+deltaRX;
            //Get rY: Iterate from camRY-ring to camRY+ring based on deltaRY
            for( int rY = camRY-ring; rY<=camRY+ring; rY += deltaRY ) {
                for(int rZ = camRZ-verticalRadius; rZ<=camRZ+verticalRadius; rZ++) {

                    //Check if this region at (rX, rY) is onscreen (vertical coordinate doesn't matter)
                    bool onscreen = false;
                    if( rX>=camRX-screenRWR && rX<=camRX+screenRWR ) {
                        if( rY>=camRY-screenRHR && rY<=camRY+screenRHR ) {
                            onscreen = true;
                        }
                    }

                    //Load region if numRegionInserts<maxRegionInserts
                    if( numRegionInserts<maxRegionInserts ) {
                        //If region has not been loaded yet, load the region.
                        Timer rlt;
                        bool loadedReg = false;
                        if( tileMap->loadRegion(rX, rY, rZ)==0 ) {
                            loadedReg = true;
                            ++numRegionInserts;
                        }
                        if( loadedReg ) {
                            regLoadTimeLatest = rlt.getElapsedTimeMS();
                            regLoadCount++;
                            regLoadTime += regLoadTimeLatest;
                        }
                    }

                    //Check if this region is on screen and its Z coordinate (height in world) == the camera's Z region coordinate.
                    if( onscreen && rZ==camRZ ) {
                        //The region tex needs an update if the region is marked for an RTU.
                        TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, rZ);
                        if( tr!=nullptr ) {
                            if( tr->getRegTexState()==tr->RegTexState::FINISHED_GENERATING ) {
                                tr->setRegTexState(tr->RegTexState::SHOULD_UPDATE);
                            }
                        }

                        if( tr->getRegTexState()==tr->RegTexState::SHOULD_UPDATE ) {
                            tileMap->addRegionUpdate(rX, rY, cam->getLayer());
                            tr->setRegTexState(tr->RegTexState::UPDATED);
                        }
                    }
                }
            }
        }
    }

    /** Region unloading */
    /*
        Region unloading:
        TileRegions will be deleted from the map if:
        -they are > 'radius' regions away from the current camera region horizontally
        -they are > 'verticalRadius' regions away from the current camera region vertically
    */

    TileMap::t_regionMap::iterator itrRM = tileMap->getRegionMap()->begin();
    while( itrRM!=tileMap->getRegionMap()->end() ) {
        /** Check if this tileRegion is not null */
        //If this tileRegion is null...
        //... stop this iteration and continue to the next one
        TileRegion* tr = &itrRM->second;
        if(tr==nullptr) {
            continue;
        }

        /** Get basic info */
        //Get rX, rY, and rZ of itr.
        int rX = std::get<0>(itrRM->first);
        int rY = std::get<1>(itrRM->first);
        int rZ = std::get<2>(itrRM->first);
        int sz = TileMap::getRegSubPos( cam->getLayer() );

        /** Manage regTexState */
        //If a tileRegion's regTexState is not NONE (aka not reset)...
        //... we need to check if it does need to be reset.
        if( tr->getRegTexState()!=TileRegion::RegTexState::NONE ) {
            //If a region is far offscreen or not on the same rZ as the camera
            if( rX<camRX-screenRWR || rX>camRX+screenRWR ||
                rY<camRY-screenRHR || rY>camRY+screenRHR ||
                rZ!=camRZ
            ) {
                //Reset region state and stop all region updates
                tr->resetRegTexState();
                for(int i = 0; i<32; i++) {
                    tileMap->stopRegionUpdate( rX, rY, rZ*32+i );
                }
            }

            //Stop region updates wherever their Z!=cam->getLayer()
            if( rZ==camRZ ) {
                for(int i = 0; i<32; i++) {
                    if( i!=sz ) {
                        tileMap->stopRegionUpdate( rX, rY, rZ*32+i );
                    }
                }
            }
        }

        /** Manage regionMap */
        //If a region is outside of a rectangular prism of regions defined by (rX, rY, and rZ) and radius + verticalRadius
        if( rX<camRX-radius || rX>camRX+radius ||
            rY<camRY-radius || rY>camRY+radius ||
            rZ<camRZ-verticalRadius || rZ>camRZ+verticalRadius
        ) {
            //Unload region
            tileMap->saveRegion(fileHandler, rX, rY, rZ);
            itrRM = tileMap->getRegionMap()->erase(itrRM);
        } else {
            itrRM++;
        }
    }

    /** Performance gauging */
    if(regLoadCount>regLoadDivisor) {
        //Calculate avg reg load time
        regLoadTimeAvg = regLoadTime/(double)regLoadCount;
        //Reset regionsLoaded count and regionLoadTime
        regLoadCount = 0;
        regLoadTime = 0.0;
    }

}

void TileMapScreen::updateEntireScreen()
{
    for(TileMap::t_regionMap::iterator itrRM = tileMap->getRegionMap()->begin(); itrRM!=tileMap->getRegionMap()->end(); itrRM++ ) {
        TileRegion* tr = &itrRM->second;
        if( tr==nullptr ) continue;
        tr->setRegTexState(tr->RegTexState::SHOULD_UPDATE);
    }
    tileMap->stopAllUpdates();
}

/**
    Place regTexUpdates randomly throughout the map on any tile that is onscreen
*/
void TileMapScreen::idleRegTexUpdates(int updates)
{
    if(tileMap==nullptr) return;

    TileMap::t_ll scrRW2T = screenWT/4; if( scrRW2T==0 ) scrRW2T = 1;
    TileMap::t_ll scrRH2T = screenWT/4; if( scrRH2T==0 ) scrRH2T = 1;
    TileMap::t_ll scrRW4T = screenWT/8;
    TileMap::t_ll scrRH4T = screenHT/8;
    TileMap::t_ll scrCX = floor( cam->getX()/4 )*4;
    TileMap::t_ll scrCY = floor( cam->getY()/4 )*4;

    for( int i = 0; i<updates/16+1; i++ ) {

        TileMap::t_ll updatesX = scrCX-4*scrRW4T+4*(rand())%(4*scrRW2T);
        TileMap::t_ll updatesY = scrCY-4*scrRH4T+4*(rand())%(4*scrRH2T);

        for(TileMap::t_ll sx = 0; sx<4; sx++) {
            for(TileMap::t_ll sy = 0; sy<4; sy++) {
                tileMap->addTileUpdate( updatesX+sx, updatesY+sy, cam->getLayer() );
            }
        }
    }
}

void TileMapScreen::regTexUpdates(Canvas* csTileMap, int maxUpdates)
{
    if( csTileMap==nullptr || cam==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

    TileIterator ti(tileMap);
    ti.logWarnings(false);
    ti.setTrackerMode(ti.FULL);

    int numUpdates = 0;

    int z = cam->getLayer();
    int sz = TileMap::getRegSubPos(z);

    int radius = 4;
    //We want to load region texes in a rectangular ring pattern.
    //This rectangular area of regions should barely cover the entire screen (visible area based on camera zoom, x, y, and z).
    //Iterate through regions
    for( int ring = 0; ring<=radius; ring++ ) {
        for( int deltaRX = -ring; deltaRX<=ring; deltaRX++ ) {
            //Set deltaRY
            int deltaRY = ring*2;
            if( abs(deltaRX)==ring ) { deltaRY = 1; }
            //Get rX
            int rX = camRX+deltaRX;
            //Get rY: Iterate from camRY-ring to camRY+ring based on deltaRY
            for( int rY = camRY-ring; rY<=camRY+ring; rY += deltaRY ) {
                //If (rX, rY) is onscreen
                if( rX>=camRX-screenRWR && rX<=camRX+screenRWR && rY>=camRY-screenRHR && rY<=camRY+screenRHR ) {
                    /** Get 'rtu' (region texture updates vector) located at this region */
                    TileMap::t_updates* rtu = tileMap->getRTUsByRXYz(rX, rY, z);
                    //If 'rtu' is null or rtu->size()==0
                    if(rtu==nullptr || rtu->size()==0) {
                        //Make sure to delete this update collection.
                        tileMap->getUpdatesMap()->erase(std::make_tuple(rX, rY, z));
                        //Continue to the next loop iteration
                        continue;
                    }

                    /** Get 'tex' located at this region from csTileMap */
                    Texture* tex = csTileMap->getTex(rX, rY);
                    //If 'tex' is null
                    if(tex==nullptr) {
                        //Continue to next region location in loop.
                        continue;
                    }

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
                        regTexUpdate(ti, tex);
                        //Erase this rtu
                        rtu->erase(itr);
                        //If max number of updates has been reached, stop the method
                        if(numUpdates>=maxUpdates) return;
                    }
                }
            }
        }
    }
}

void TileMapScreen::regTexUpdate(TileIterator& ti, Texture* tex)
{
    //If tex is nullptr for some reason, stop method
    if( tex==nullptr ) return;

    //Get data of the top tile from the camera
    auto tttData = topTrackedTile(ti);              //Pair of a t_ll object and a TileType object
    TileMap::t_ll dZ = std::get<0>(tttData);        //Relative height of top tile ( topmost==0, one below==-1, etc. )
    TileType topTileFromCam = std::get<2>(tttData); //TileType of top tile from camera

    int blitScalePx = tileSize*mapZoom;
    if( blitScalePx>tileSize )
        blitScalePx = tileSize;

    //Get the destination of texture blit on the canvas region
    int dstX = blitScalePx*ti.getTrackerSub(0);  //32*X
    int dstY = blitScalePx*ti.getTrackerSub(1);  //32*Y

    RegTexBuilder rtb(tex, ti, dstX, dstY, blitScalePx, topTileFromCam, dZ);
}
