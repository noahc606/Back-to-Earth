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

void TileMapScreen::init( SDLHandler* sh, Controls* ctrls, TileMap* p_tm, Player* p_pl )
{
    BTEObject::init(sh, ctrls);

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

    //Calculate screen width and screen height
    screenWidth = sdlHandler->getWidth()/tileScale;
    if( screenWidth==0 ) screenWidth = 1;
    screenHeight = sdlHandler->getHeight()/tileScale;
    if( screenHeight==0 ) screenHeight = 1;

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
        for(TileMap::t_regionMap::iterator itrRM = tileMap->getRegionMap()->begin(); itrRM!=tileMap->getRegionMap()->end(); itrRM++ ) {
            TileRegion* tr = &itrRM->second;
            if( tr==nullptr ) continue;
            tr->setRegTexState(tr->RegTexState::SHOULD_UPDATE);
        }

        tileMap->stopAllUpdates();
    }

    //Map updates (regionMap, updatesMap)
    std::string* cmd = Commands::getString("stopMapUpdates");
    if( cmd!=nullptr ) {} else {
        mapUpdates();
    }

    //Find tile scale
    mapZoom = cam->getZoom();
    tileScale = tileSize*mapZoom;

    //Translations depending on window width/height. Ensures that current camera's location is at the center of the window.
    screenTX = sdlHandler->getWidth()/2;
    screenTY = sdlHandler->getHeight()/2;

    tickTime = localTimer.getElapsedTimeMS();
}

void TileMapScreen::draw(Canvas* csTileMap)
{
    drawTime = 0.0;
    Timer localTimer;

    if( csTileMap==nullptr ) return;

    //Perform 'rtUpdates' number of regTexUpdates and do performance gauging
    rtUpdatesTime = 0.0;
    {
        std::stringstream ss; ss << "Performing " << rtUpdatesToDo << " regTexUpdates";
        Timer t(ss.str(), false);
        regTexUpdates(csTileMap, rtUpdatesToDo);
        rtUpdatesTime = t.getElapsedTimeMS();
    }
    rtUpdatesTimeTotal += rtUpdatesTime;

    double rtUpdatesTimeMax = 8;
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
    DebugScreen::endGroup(tabs);

    //Camera
    DebugScreen::newGroup(ss, tabs, "Camera");
        //Zoom, scale
        DebugScreen::indentLine(ss, tabs);
        ss << "Screen (width, height)=(" << screenWidth << ", " << screenHeight << "); ";
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

void TileMapScreen::mapUpdates()
{
    if( cam==nullptr || tileMap==nullptr || tileMap->getRegionMap()==nullptr ) return;

    int maxRegionInserts = 2;
    int numRegionInserts = 0;

    //Load Regions, RegTexes, nearby in a ring
    int radius = 5;
    int verticalRadius = 1;

    int scrRW = screenTX/tileScale/regionSize+2;
    int scrRH = screenTY/tileScale/regionSize+2;
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
                    if( rX>=camRX-scrRW && rX<=camRX+scrRW ) {
                        if( rY>=camRY-scrRH && rY<=camRY+scrRH ) {
                            onscreen = true;
                        }
                    }

                    //Load region if numRegionInserts<maxRegionInserts
                    if( numRegionInserts<maxRegionInserts ) {
                        //If region has not been loaded yet, load the region.
                        if( tileMap->loadRegion(rX, rY, rZ)==0 ) {
                            //If the region is onscreen and !=nullptr, mark it for a regTexUpdate (RTU).
                            TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, rZ);
                            if(tr!=nullptr ) {
                                tr->setRegTexState(tr->RegTexState::SHOULD_UPDATE);
                            }
                            //Increment numRegionInserts
                            ++numRegionInserts;
                        }
                    }

                    //Check if this region is on screen and its Z coordinate (height in world) == the camera's Z region coordinate.
                    if( onscreen && rZ==camRZ ) {
                        //The region tex needs an update if the region is marked for an RTU.
                        TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, rZ);
                        if( tr!=nullptr ) {
                            if( tr->getRegTexState()==tr->RegTexState::NONE ) {
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
            if( rX<camRX-scrRW || rX>camRX+scrRW ||
                rY<camRY-scrRH || rY>camRY+scrRH ||
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
            itrRM = tileMap->getRegionMap()->erase(itrRM);
        } else {
            itrRM++;
        }
    }
}

/**
    Place regTexUpdates randomly throughout the map on any tile that is onscreen
*/
void TileMapScreen::idleRegTexUpdates(int updates)
{
    if(tileMap==nullptr) return;

    TileMap::t_ll scrRW = screenTX/tileScale;   //Distance from left side of screen to center of screen.
    TileMap::t_ll scrRH = screenTY/tileScale;   //Distance from upper side of screen to center of screen.
    TileMap::t_ll scrCX = cam->getX();          //X pos of camera
    TileMap::t_ll scrCY = cam->getY();          //Y pos of camera

    for( int i = 0; i<updates; i++ ) {
        tileMap->addTileUpdate( scrCX-scrRW+rand()%screenWidth, scrCY-scrRH+rand()%screenHeight, cam->getLayer() );
        //tileMap->addTileUpdate( scrCX-scrRW+rand()%screenWidth, scrCY-scrRH+rand()%screenHeight, cam->getLayer() );
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

    int radius = 6;
    //We want to region texes in a rectangular ring pattern.
    //This rectangular area of regions should barely cover the entire screen (visible area based on camera zoom, x, y, and z).
    int scrRW = screenTX/tileScale/regionSize+3;
    int scrRH = screenTY/tileScale/regionSize+3;
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
                if( rX>camRX-scrRW && rX<camRX+scrRW && rY>camRY-scrRH && rY<camRY+scrRH ) {
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

    //Get the destination of texture blit on the canvas region
    int dstX = 32*ti.getTrackerSub(0);  //32*X
    int dstY = 32*ti.getTrackerSub(1);  //32*Y

    RegTexBuilder rtb(tex, ti, dstX, dstY, topTileFromCam, dZ);
}
