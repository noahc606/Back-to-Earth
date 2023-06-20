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

void TileMapScreen::init( SDLHandler* sh, FileHandler* fh, Controls* ctrls, Player* p_pl, TileMap* p_tm, Canvas* p_ctm )
{
    // Set SDLHandler, FileHandler, controls
    BTEObject::init(sh, fh, ctrls);
    // Set TileMap and TileMap's Canvas
    tileMap = p_tm;
    csTileMap = p_ctm;
    // Set Camera, RegTexUpdates objects if player is not null
    if( p_pl!=nullptr ) {
        cam = p_pl->getCamera();
        regTexUpdates = new RegTexUpdates(sdlHandler, cam, tileMap, csTileMap);
    }
}

void TileMapScreen::destroy()
{
    // Get rid of RegTexUpdates object when this is destroyed
    Log::log("Destroying tileMapScreen...");
    delete regTexUpdates;
}

TileMapScreen::~TileMapScreen() { destroy(); }

void TileMapScreen::tick()
{
    // Every time TMS is ticked, we need to decide what part of the map to update
    bool doUpdMapVisible = false;   // 1. Update entire screen when zoom is changed or when camera layer is changed (most expensive)
    bool doUpdMapTicked = true;     // 2. Map updates to do every tick
    bool doUpdMapMoved = false;     // 3. Map updates to do when crossing a region border: Unload regions that just went out of player range
    bool doUpdMapIdle = false;      // 4. Map updates to do while idle: Check all regions to see if they should be unloaded. Done every autosave (30s)

    // Track tick time (start at 0.0)
    infoTickTime = 0.0;
    Timer localTimer;
    {
        // Calculate screen width and screen height (in tiles)
        if(regTexUpdates!=nullptr) {
            regTexUpdates->setScreenInfo();
        }

        // Update camera
        if(cam!=nullptr) {
            // Get camera info
            int cRX = floor( cam->getX()/regionSize );
            int cRY = floor( cam->getY()/regionSize );
            int cRZ = floor( ((double)cam->getZ())/regionSize );

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
            }

            //If camera zoom has changed, update camZoom, tileScale, the entire screen, and blitScalePx.
            if( camZoom!=cam->getZoom() ) {
                // Update camZoom
                camZoom = cam->getZoom();
                doUpdMapVisible = true;

                // Update the entire screen
                regTexUpdates->placeEntireScreen();
            }
        }

        // Check if idle map updates should be done
        if( !true ) {
            umiTicks++;
            if( umiTicks>=umiTicksMax ) {
                umiTicks = 0;
                std::string* cmd = Commands::getString("stopIdleMapUpdates");
                if( cmd==nullptr ) {
                    doUpdMapIdle = false;
                }
            }
        }

        // Update different parts of the map under different conditions
        // Make sure to do it in the order of 1->2->3->4 (most expensive to least)
        if( doUpdMapVisible )   updateMapVisible();
        if( doUpdMapTicked )    updateMapTicked();
        if( doUpdMapMoved )     updateMapMoved();
        if( doUpdMapIdle )      updateMapIdle();
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
        //Region Texture Updates
        regTexUpdates->draw(camRX, camRY, camRZ, loadRadiusH);
        csTileMap->draw();
    }
    //Track draw time
    infoDrawTime = localTimer.getElapsedTimeMS();

    //Increment drawsThisSecond
    drawsThisSecond++;
    //When SDL_GetTicks() exceeds nextSecond
    if( SDL_GetTicks()>nextSecond ) {
        //rtu->updateTimeAvg(drawsThisSecond);
        nextSecond += 1000;             //Update when the nextSecond is
        drawsThisSecond = 0;            //Reset drawsThisSecond
    }
}

void TileMapScreen::info(std::stringstream& ss, int& tabs, TileMap::t_ll mouseX, TileMap::t_ll mouseY, TileMap::t_ll mouseZ)
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

        DebugScreen::indentLine(ss, tabs);
        ss << "infoRegLoadCount=" << infoRegLoadCount << "; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "infoRegLoadTime (latest)=" << infoRegLoadTimeLatest << "ms; ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "infoRegLoadTime (avg out of last " << infoRegLoadDivisor << ")=" << infoRegLoadTimeAvg << "ms; ";
        DebugScreen::newLine(ss);

    DebugScreen::endGroup(tabs);

    //Camera
    DebugScreen::newGroup(ss, tabs, "Camera");
        //Zoom, scale
        DebugScreen::indentLine(ss, tabs);
        ss << "camZoom=" << camZoom << "; ";
        ss << "tileScale=" << regTexUpdates->getTileScale() << "px; ";
        ss << "blitScale=" << regTexUpdates->getBlitScale() << "px; ";
        DebugScreen::newLine(ss);

        //Location in world
        DebugScreen::indentLine(ss, tabs);
        ss << "XYZ=(" << cam->getX() << std::setprecision(5) << ", " << cam->getY() << ", " << cam->getZ() << "); ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "RXYZ=(" << camRX << ", " << camRY << ", " << camRZ << "); ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "Layer=" << cam->getLayer() << "; ";
        DebugScreen::newLine(ss);


    DebugScreen::endGroup(tabs);

    //Mouse
    DebugScreen::newGroup(ss, tabs, "Region Texture");

        int msx = TileMap::getRegSubPos(mouseX);
        int msy = TileMap::getRegSubPos(mouseY);
        int msz = TileMap::getRegSubPos(mouseZ);

        //Location in world
        DebugScreen::indentLine(ss, tabs);
        ss << "XYZ=(" << mouseX << ", " << mouseY << ", " << mouseZ << "); ";
        DebugScreen::newLine(ss);
        DebugScreen::indentLine(ss, tabs);
        ss << "subXYZ=(" << msx << ", " << msy << ", " << msz << ") in (" << TileMap::getRegRXYZ(mouseX) << ", " << TileMap::getRegRXYZ(mouseY) << ", " << TileMap::getRegRXYZ(mouseZ) << "); ";
        DebugScreen::newLine(ss);

        // RegTexUpdates
        DebugScreen::newGroup(ss, tabs, "RegTexUpdates");
            regTexUpdates->info(ss, tabs);
        DebugScreen::endGroup(tabs);

        DebugScreen::newGroup(ss, tabs, "RegTexBuilder");
            //Top tile info
            DebugScreen::indentLine(ss, tabs);
            TileIterator ti(tileMap);
            ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseX), TileMap::getRegRXYZ(mouseY), TileMap::getRegRXYZ(cam->getLayer()) );
            ti.setTrackerSub( msx, msy, TileMap::getRegSubPos(cam->getLayer()) );
            auto tttData = RegTexUpdates::topTrackedTile(ti);
            TileMap::t_ll dZ = std::get<0>(tttData);
            ss << "Top tile(dZ)=" << dZ << ";";
            DebugScreen::newLine(ss);
            //Top tile(x, y, z) info
            DebugScreen::indentLine(ss, tabs);
            ss << "Tile (x, y, z)=(" << ti.getTrackerSub(0) << ", " << ti.getTrackerSub(1) << ", " << ti.getTrackerSub(2)+dZ << ");";
            DebugScreen::newLine(ss);
            //RTB info
            RegTexBuilder::info(ss, tabs, ti, dZ);
        DebugScreen::endGroup(tabs);

    DebugScreen::endGroup(tabs);
}

void TileMapScreen::updateMapVisible()
{
    regTexUpdates->placeEntireScreen();

    // Update scaling of region textures
    regTexUpdates->updateScaling( tileSize*camZoom, 32.0*Canvas::getTexLODBasedOnZoom(camZoom) );
}

void TileMapScreen::updateRegTicked(int rX, int rY, int rZ)
{
    int loadCount = 0;

    //Check if this region at (rX, rY) is onscreen (vertical coordinate doesn't matter)
    bool onscreen = false;
    if( rX>=camRX-regTexUpdates->getScreenSemiWidth() && rX<=camRX+regTexUpdates->getScreenSemiWidth() ) {
        if( rY>=camRY-regTexUpdates->getScreenSemiHeight() && rY<=camRY+regTexUpdates->getScreenSemiHeight() ) {
            onscreen = true;
            //std::cout << "{" << rX << ", " << rY << "} ";
        }
    }

    //Load region if loadCount<loadCountMax
    if( loadCount<loadCountMax ) {
        //If region has not been loaded yet, load the region.
        Timer rlt;

        int lr = -1;
        lr = tileMap->loadRegion(rX, rY, rZ);

        if( lr==0 ) {
            loadCount++;

            infoRegLoadTimeLatest = rlt.getElapsedTimeMS();
            infoRegLoadCount++;
            infoRegLoadTime += infoRegLoadTimeLatest;
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

            if( tr->getRegTexState()==tr->RegTexState::SHOULD_UPDATE ) {
                tileMap->addRegionUpdate(rX, rY, cam->getLayer());
                tr->setRegTexState(tr->RegTexState::UPDATED);
            }
        }
    }
}

void TileMapScreen::updateMapTicked()
{
    //Iterate through a rectangular prism region of TileRegions around the player
    for( int outline = 0; outline<=loadRadiusH; outline++ ) {
        //Iterate dRX (delta RX) from -outline to +outline
        for( int dRX = -outline; dRX<=outline; dRX++ ) {
            int dRY = outline*2;                                //Set dRY (delta RY)
            if( abs(dRX)==outline ) { dRY = 1; }
            int rX = camRX+dRX;                                 //Get rX
            //Get rY: Iterate from camRY-outline to camRY+outline based on dRY
            for( int rY = camRY-outline; rY<=camRY+outline; rY += dRY ) {
                //Get rZ: Iterate from camRZ-loadRadiusV to camRZ+loadRadiusV
                for(int rZ = camRZ-loadRadiusV; rZ<=camRZ+loadRadiusV; rZ++) {
                    updateRegTicked(rX, rY, rZ);
                }
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
}

void TileMapScreen::updateMapMoved()
{
    /** Region unloading (nearby region immediately out of range) */
    int outlineH = loadRadiusH+1;
    int outlineV = loadRadiusV+1;

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
                regTexUpdates->blackOutRegionArea(rX, rY);
            }

            for(int rZ = camRZ-outlineV; rZ<=camRZ+outlineV; rZ += dRZ) {
                tileMap->unloadRegion(fileHandler, currentDimPath, rX, rY, rZ);
            }
        }
    }
}

void TileMapScreen::updateMapIdle()
{
    /** Region unloading (all regions) */
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
            if( rX<camRX-regTexUpdates->getScreenSemiWidth() || rX>camRX+regTexUpdates->getScreenSemiWidth() ||
                rY<camRY-regTexUpdates->getScreenSemiHeight() || rY>camRY+regTexUpdates->getScreenSemiHeight() ||
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
        //If a region is outside of a rectangular prism of regions defined by (rX, rY, and rZ) and loadRadiusH + loadRadiusV
        if( rX<camRX-loadRadiusH || rX>camRX+loadRadiusH ||
            rY<camRY-loadRadiusH || rY>camRY+loadRadiusH ||
            rZ<camRZ-loadRadiusV || rZ>camRZ+loadRadiusV
        ) {
            //Unload region
            tileMap->saveRegion(fileHandler, currentDimPath, rX, rY, rZ);
            itrRM = tileMap->getRegionMap()->erase(itrRM);
        } else {
            itrRM++;
        }
    }
}


