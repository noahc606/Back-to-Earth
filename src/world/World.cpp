#include "World.h"
#include <math.h>
#include "TextureBuilder.h"
#include "Log.h"
#include "MainLoop.h"
#include "Timer.h"

void World::init(SDLHandler* sh, Controls* ctrls)
{
    if( exists() ) return;

    BTEObject::init(sh, nullptr, ctrls);

    Commands::cKV("x1", -16);
    Commands::cKV("y1", -16);

    //Init player, tileMap, tileMapScreen.
    player.init(sh, ctrls);
    player.setPos(0, 0, -32);
    tileMap.init();
    tileMapScreen.init(sdlHandler, controls, &tileMap, &player);

    //Init csTileMap
    csTileMap.init(sdlHandler, controls, player.getCamera());
    csTileMap.setUsingDynamicLOD(true);

    //Init csInteractions
    csInteractions.init(sdlHandler, controls, player.getCamera());
    csInteractions.setMaximumFPS(20);

    //Init csEntities
    csEntities.init(sdlHandler, controls, player.getCamera());

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

void World::tick(bool paused)
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
    entityInteractions();

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

void World::entityInteractions()
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

    if( player.getAction()==player.Action::GM_Destroy_Tile ) {
        TileType tt;
        tt.init();
        tt.setVisionBlocking(false);
        tileMap.setTile(mouseXLL, mouseYLL, player.getCamera()->getLayer(), tt);
        tileMap.addTileUpdates(mouseXLL, mouseYLL, player.getCamera()->getLayer());
    } else
    if( player.getAction()==player.Action::GM_Place_Tile ) {
        TileType tt;
        tt.init();
        tt.setRGB(20, 255, 255);
        tt.setSolid(true);
        tt.setTextureXY(0, 2);
        tt.setVisionBlocking(true);

        tileMap.setTile(mouseXLL, mouseYLL, player.getCamera()->getLayer(), tt);
        tileMap.addTileUpdates(mouseXLL, mouseYLL, player.getCamera()->getLayer());
    }
}
