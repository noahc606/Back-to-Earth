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
    //Set SDLHandler, FileHandler, controls
    BTEObject::init(sh, fh, ctrls);
    //Set TileMap and TileMap's Canvas
    tileMap = p_tm;
    csTileMap = p_ctm;
    //If player is not null
    if( p_pl!=nullptr ) {
        //Set Camera, RegTexUpdates objects
        cam = p_pl->getCamera();
        regTexUpdates = new RegTexUpdates(sdlHandler, cam, tileMap, csTileMap);
        //Update map
        updateMapMoved();
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
    bool doUpdMapIdle = false;      // 4. Map updates to do while idle: add updates twice a second at an onscreen region with lowest priority
    bool doUpdMapAutosave = false;  // 5. Map updates to do every autosave (30s): Check all regions to see if they should be unloaded -> save to disk.

    // Track tick time (start at 0.0)
    infoTickTime = 0.0;
    Timer localTimer;
    {
        // Update cam and regTexUpdates
        if( cam!=nullptr && regTexUpdates!=nullptr ) {
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
            if( cam->getZoom()!=camZoom ) {
                // Update camZoom
                camZoom = cam->getZoom();
                doUpdMapVisible = true;
            }

            // Calculate screen width and screen height
            regTexUpdates->setScreenInfo();
        }

        // Check if idle map updates should be done
        if( true ) {
            umiTicks++;
            if( umiTicks>=umiTicksMax ) {
                doUpdMapIdle = true;
                umiTicks = 0;
            }
        }
		
        // Update different parts of the map under different conditions
        // Make sure to do it in the order of 1->2->3->4 (most expensive to least)
        if( doUpdMapVisible )   updateMapVisible();
        if( doUpdMapTicked )    updateMapTicked();
        if( doUpdMapMoved )     updateMapMoved();
        if( doUpdMapIdle )      updateMapIdle();
        if( doUpdMapAutosave )  updateMapAutosave();
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
        //Canvas drawing to screen
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
        ss << "Region Loading Time (avg out of last " << infoRegLoadDivisor << ")=" << infoRegLoadTimeAvg << "ms; ";
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
        ss << "RXYZ=(" << camRX << ", " << camRY << ", " << camRZ << "); ";
        ss << "Layer=" << cam->getLayer() << "; ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //Mouse
    DebugScreen::newGroup(ss, tabs, "TileRegion");

        long selRX = TileMap::getRegRXYZ(mouseX);
        long selRY = TileMap::getRegRXYZ(mouseY);
        long selRZ = TileMap::getRegRXYZ(mouseZ);

        TileRegion* tr = tileMap->getRegByRXYZ(selRX, selRY, selRZ);
        int msx = TileMap::getRegSubPos(mouseX);
        int msy = TileMap::getRegSubPos(mouseY);
        int msz = TileMap::getRegSubPos(mouseZ);

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
            ss << "Top tile(dZ, Z)=(" << dZ << ", " << (ti.getTrackerSub(2)+dZ) << "); ";
            DebugScreen::newLine(ss);
            //RTB info
            RegTexBuilder::info(ss, tabs, ti, dZ);
        DebugScreen::endGroup(tabs);

    DebugScreen::endGroup(tabs);
}

void TileMapScreen::updateMapVisible()
{
    /** Regions that are on screen should update */
    int outlineH = loadRadiusH+1;

    for( int dRX = -outlineH; dRX<=outlineH; dRX++) {
        for( int dRY = -outlineH; dRY<=outlineH; dRY++) {
            //Get rX and rY
            int rX = camRX+dRX;
            int rY = camRY+dRY;
            //Set dRZ (delta RZ)
            int rZ = camRZ;
            TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, rZ);
            if( tr==nullptr ) {
                continue;
            }
            tr->setRegTexState(tr->SHOULD_UPDATE);
        }
    }

    //Update scaling of region textures
    regTexUpdates->updateScaling( tileSize*camZoom, 32.0*Canvas::getTexLODBasedOnZoom(camZoom) );
}

void TileMapScreen::updateRegTicked(int rX, int rY, int rZ)
{
    //Load region if loadCount<loadCountMax
    int loadCount = 0;
    if( loadCount<loadCountMax ) {
        //Timer for debugging
        Timer rlt;

        //If region has not been loaded yet, load the region.
        int lr = -1;
        lr = tileMap->loadRegion(rX, rY, rZ);

        //If region load was successful
        if( lr==0 ) {
            //Increment loadCount
            loadCount++;
            //Set debug info
            infoRegLoadTimeLatest = rlt.getElapsedTimeMS();
            infoRegLoadCount++;
            infoRegLoadTime += infoRegLoadTimeLatest;
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
    updateMapVisible();

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

            for( int rZ = camRZ-outlineV; rZ<=camRZ+outlineV; rZ += dRZ ) {
                //Unload regions
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

void TileMapScreen::updateMapIdle()
{
    // Find the region texture with the minimum priority
    int minRTP = -1;
    for( int minRTPSet = 0; minRTPSet<2; minRTPSet++ ) {
        //Iterate through a rectangular area of regions at the camera's RZ
        for( int dRX = -loadRadiusH; dRX<=loadRadiusH; dRX++ ) {
            for( int dRY = -loadRadiusH; dRY<=loadRadiusH; dRY++ ) {
                int rX = camRX+dRX;
                int rY = camRY+dRY;
                //If this region is onscreen
                if(regTexUpdates->isOnScreen(rX, rY)) {
                    //Get tile region and its region texture priority ('rtp')
                    TileRegion* tr = tileMap->getRegByRXYZ(rX, rY, camRZ);
                    if(tr==nullptr) continue;

                    int rtp = tr->getRegTexPriority();
                    if( tr->getRegTexState()==tr->SHOULD_UPDATE ) {
                        //Set minRTP if it still needs to be set
                        if( minRTPSet==0 && (rtp<minRTP || minRTP==-1) ) {
                            minRTP = rtp;
                        }
                        //If minRTP is already set, processRegionLayer
                        if( minRTPSet!=0 && minRTP!=-1 && rtp==minRTP ) {
                            tileMap->addRegionUpdate(rX, rY, cam->getLayer());
                            tr->setRegTexState(tr->UPDATED);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void TileMapScreen::updateMapAutosave()
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
        /** Null pointer check */
        //If this TileRegion is null, stop this iteration and continue to the next one
        TileRegion* tr = &itrRM->second;
        if(tr==nullptr) {
            itrRM++;
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
            if( !regTexUpdates->isOnScreen(rX, rY) || rZ!=camRZ ) {
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


