#include "Tests.h"
#include <map>
#include <math.h>
#include <bitset>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include "Color.h"
#include "Log.h"
#include "Terrain.h"
#include "Real.h"
#include "Text.h"
#include "TileMap.h"
#include "Timer.h"
#include "Window.h"

using namespace std;
/**/

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    TileType tt; tt.init();
    tt.setRGB(128, 32, 10);
    tt.setTextureXYZ(5, 8, 10);

    std::stringstream ss;
    int tabs = 0;
    tt.info(ss, tabs);
    std::cout << "Tile info: " << ss.str();

    BTEObject::init(sh, fh, ctrls);
    std::cout << "before\n";

    TileRegion testReg;
    Terrain terra;
    terra.populateRegion( testReg, 0, 0, 0 );

    std::cout << "Saving...\n";
    Timer timer("Level save test", true);
    {
        testReg.save(sdlHandler, fileHandler, "test123456", 0, 0, 0, false);
    }
    std::cout << "Saved in: \n";
}

Tests::~Tests(){}

/**/

void Tests::draw()
{
    SDL_Rect dst; dst.x = 0; dst.y = 0; dst.w = 512; dst.h = 512;
    counter++;


    if(counter%100<50) {
        SDL_RenderCopy(
            sdlHandler->getRenderer(),
            sdlHandler->getTextureLoader()->getTexture(TextureLoader::WORLD_TILE_type_a, 0),
            NULL,
            &dst
        );
    } else {
        SDL_RenderCopy(
            sdlHandler->getRenderer(),
            sdlHandler->getTextureLoader()->getTexture(TextureLoader::WORLD_TILE_type_a, 1),
            NULL,
            &dst
        );
    }




    //sdlHandler->renderCopy(3, &dst, &dst);

    for(int i = 0; i<10; i++) {
        //stex.lock(rx*32, ry*32, 32, 32);
        //stex.
    }

    //test123.draw();
    //stex.draw();
    //ttex.draw();

    /*

    test123.lock(200, 200, 40, 200);
    test123.blit(TextureLoader::GUI_FONT_robot, 0, 0);
    test123.draw();*/
}

void Tests::tick()
{

}

/**/

void Tests::info(std::stringstream& ss, int& tabs)
{

}

/*
void test1()
{
    //SpriteSheet ss;
    //ColorPalette cpal; cpal.init(cpal.DEFAULT_PLAYER);

    //SpriteSheetBuilder ssb(sh);

    //ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

    stex.init(sdlHandler);
    stex.setTexDimensions(1024, 1024);

    SpriteSheet ss;
    ss.init(sdlHandler, TextureLoader::WORLD_TILE_type_a);

    stex.lock(0, 0, 512, 512);
    stex.fill(127, 127, 255);
    stex.update();
    Timer tim1("test1", false);
    {
        stex.setColorMod(255, 0, 0);
        stex.setBlendMode(SDL_BLENDMODE_MOD);
        stex.lock(128, 128, 128, 128);
        stex.sblit(&ss, 0, 0, 512, 512);
        stex.update();
    }

    tim1.debugElapsedTimeMS();


    ttex.init(sdlHandler, 1024, 1024);
    ttex.lock(0, 0, 512, 512);
    ttex.setColorMod(127, 127, 255);
    ttex.fill();
    Timer tim2("test2", false);
    for( int i = 0; i<1; i++ ) {
        ttex.setColorMod(255, 0, 0);
        ttex.setBlendMode(SDL_BLENDMODE_MOD);
        ttex.lock(128, 128, 128, 128);
        ttex.blit(TextureLoader::WORLD_TILE_type_a, 0, 0, 512, 512);
    }
    tim2.debugElapsedTimeMS();

    //stex.lock(10, 10, 32, 32);
    //stex.unlock();
    //stex.update();
    //stex.blit(TextureLoader::WORLD_TILE_type_a, 64, 64);
    //stex.unlock();

    std::string trPath = "saved\\games\\tutorial\\tr";

    Timer fw("file writing");

    TileRegion treg;
    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            for( int z = 0; z<32; z++ ) {
                treg.setTile(x, y, z, x*1024+y*32+z);
            }
        }
    }

    treg.save(sdlHandler, fileHandler, false);
}
*/
