#include "World.h"
#include <math.h>
#include "TextureBuilder.h"
#include "Log.h"
#include "MainLoop.h"
#include "Timer.h"

void World::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    if( exists() ) return;
    BTEObject::init(sh, fh, ctrls);

    Commands::cKV("x1", -16);
    Commands::cKV("y1", -16);

    //Init player, tileMap, tileMapScreen.
    player.init(sh, ctrls);
    player.setPos(0, 0, -32);
    tileMap.init(sdlHandler, fileHandler);
    tileMapScreen.init(sdlHandler, fileHandler, controls, &tileMap, &player);

    //Init csTileMap
    csTileMap.init(sdlHandler, controls, player.getCamera());
    csTileMap.setTexUsingDynamicLOD(true);
    csTileMap.setTexAllocCount(1);
    csTileMap.setTexAllocRadiusX(1);
    csTileMap.setTexAllocRadiusY(1);

    //Init csInteractions
    csInteractions.init(sdlHandler, controls, player.getCamera());
    csInteractions.setMaximumFPS(20);
    csInteractions.setTexAllocRadiusX(1);
    csInteractions.setTexAllocRadiusY(1);

    //Init csEntities
    csEntities.init(sdlHandler, controls, player.getCamera());
    csEntities.setTexAllocRadiusX(1);
    csEntities.setTexAllocRadiusY(1);

    //Build textures
    TextureBuilder tb(sdlHandler);
    tb.init(tb.TILE_default, defaultTile);
}

World::~World()
{
    /*
    player.destroy();
    tileMap.destroy();
    tileMapScreen.destroy();



    defaultTile.destroy();
    */

    csTileMap.destroy();
    csInteractions.destroy();
    csEntities.destroy();

}

void World::draw()
{
    /** Tile Canvas */
    tileMapScreen.draw(&csTileMap);

    csTileMap.draw();


    /** Entity Canvas */
    csEntities.clearCanvas();
    player.draw(&csEntities);
    csEntities.draw();


    /** Interactions Canvas */
    csInteractions.clearCanvas();
    csInteractions.setSourceTex(TextureLoader::GUI_player_interactions, 0, 0);
    csInteractions.rcopyNI( 32*mouseXLL, 32*mouseYLL, 32, 32 );
    csInteractions.draw();

}

void World::tick(bool paused, GUIHandler& guiHandler)
{
    performanceCounter = 0;
    Timer t("World tick timer", false);

    /** Tick world objects if not paused */
    if( !paused ) {
        /** World objects */
        player.tick();
        tileMapScreen.tick();
    }

    /** Canvases */
    csTileMap.tick();
    csInteractions.tick();
    csEntities.tick();

    /** Interactions with world */
    updateMouseAndCamInfo();
    playerInteractions(guiHandler);

    performanceCounter = t.getElapsedTimeMS();
}

void World::info(std::stringstream& ss, int& tabs)
{
    if(player.getCamera()==nullptr) return;

    //Mouse Info
    DebugScreen::indentLine(ss, tabs);
    ss << "Mouse(x, y, z)=(" << mouseX << ", " << mouseY << ", " << mouseZLL << ");";
    DebugScreen::newLine(ss);
    DebugScreen::indentLine(ss, tabs);
    ss << "World tick=" << performanceCounter << "ms; ";
    DebugScreen::newLine(ss);


    //Player
    DebugScreen::newGroup(ss, tabs, "World::player");
    player.info(ss, tabs);
    DebugScreen::endGroup(tabs);

    //TileMapScreen
    DebugScreen::newGroup(ss, tabs, "World::tileMapScreen");
    tileMapScreen.info(ss, tabs, mouseXLL, mouseYLL, mouseZLL);
    DebugScreen::endGroup(tabs);

    //TileMap
    DebugScreen::newGroup(ss, tabs, "World::tileMap");
    if(player.getCamera()!=nullptr)
        tileMap.info(ss, tabs, mouseXLL, mouseYLL, mouseZLL);
    DebugScreen::endGroup(tabs);
}

void World::updateMouseAndCamInfo()
{
    //Get mouseX and mouseY both in double and in long long (t_ll) form.
    mouseX = std::get<0>(csInteractions.getMouseXY());
    mouseXLL = (t_ll)floor(mouseX);
    mouseY = std::get<1>(csInteractions.getMouseXY());
    mouseYLL = (t_ll)floor(mouseY);

    //Get Z region of camera
    long cRZ = TileMap::getRegRXYZ(player.getCamera()->getLayer());

    //Get mouseZLL - the position of the topmost visible tile when looking down from (mouseXLL, mouseYLL, camera layer).
    TileIterator ti(&tileMap);
    ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseXLL), TileMap::getRegRXYZ(mouseYLL), cRZ);
    ti.setTrackerSub( TileMap::getRegSubPos(mouseXLL), TileMap::getRegSubPos(mouseYLL), TileMap::getRegSubPos(player.getCamera()->getLayer()) );
    mouseZLL = player.getCamera()->getLayer() +( std::get<0>(tileMapScreen.topTrackedTile(ti)) );
}

void World::playerInteractions(GUIHandler& guiHandler)
{
    /*
        //Get player x, y, and z both in double and in t_ll form.
        double pxd = std::get<0>(player.getPos());
        int px = floor(pxd);
        double pyd = std::get<1>(player.getPos());
        int py = floor(pyd);
        double pzd = std::get<2>(player.getPos());
        int pz = floor(pzd);
    */

    if( playerCharMenuOpen!=player.isCharMenuOpen() ) {
        playerCharMenuOpen = player.isCharMenuOpen();

        if( playerCharMenuOpen ) {
            guiHandler.setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open);
        } else {
            guiHandler.setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
        }

    }

    if( !playerCharMenuOpen ) {
        switch( player.getAction() )
        {
            case Player::Action::GM_Destroy_Tile: {
                TileType tt;
                tt.init();
                tt.setVisionBlocking(false);
                tileMap.setTile(mouseXLL, mouseYLL, player.getCamera()->getLayer(), tt);
                tileMap.addTileUpdates(mouseXLL, mouseYLL, player.getCamera()->getLayer());
            }; break;
            case Player::Action::GM_Place_Tile: {

                TileIterator ti(&tileMap);
                int squareSize = 1;
                ti.setBounds(mouseXLL-squareSize, mouseYLL-squareSize, player.getCamera()->getLayer(), mouseXLL+squareSize, mouseYLL+squareSize, player.getCamera()->getLayer());

                while( !ti.invalidIndex() ) {
                    TileRegion* tr = ti.peekRegion();
                    if( tr!=nullptr ) {
                        for( int sx = ti.gbs(0); sx<=ti.ges(0); sx++ ) {
                            for( int sy = ti.gbs(1); sy<=ti.ges(1); sy++ ) {
                                for( int sz = ti.gbs(2); sz<=ti.ges(2); sz++ ) {

                                    t_ll tileX = ti.getItrReg(0)+sx;
                                    t_ll tileY = ti.getItrReg(1)+sy;
                                    t_ll tileZ = ti.getItrReg(2)+sz;

                                    TileType tt;
                                    tt.init();
                                    if( std::abs(tileX+tileY)%3==0 ) {
                                        tt.setRGB(200, 100, 100);
                                    } else
                                    if ( std::abs(tileX+tileY)%3==1 ) {
                                        tt.setRGB(20, 255, 255);
                                    } else {
                                        tt.setRGB( std::abs(tileX)%64*4, std::abs(tileY)%64*4, std::abs(tileZ)%64*4 );
                                    }
                                    tt.setSolid(true);
                                    tt.setTextureXY(0, 2);
                                    tt.setVisionBlocking(true);

                                    tr->setTile( sx, sy, sz, tt );
                                }
                            }
                        }
                    }

                    ti.nextRegion();
                }

                tileMap.addTileUpdates(mouseXLL-squareSize, mouseYLL-squareSize, player.getCamera()->getLayer(), mouseXLL+squareSize, mouseYLL+squareSize, player.getCamera()->getLayer() );
            }; break;
        }
    }
}
