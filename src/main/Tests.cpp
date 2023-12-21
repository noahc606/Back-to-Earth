#include "Tests.h"
#include <map>
#include <math.h>
#include <bitset>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <fcntl.h>
#include <set>
#include <sstream>
#include "Color.h"
#include "DataStream.h"
#include "Log.h"
#include "MainLoop.h"
#include "Noise.h"
#include "Terrain.h"
#include "Real.h"
#include "Text.h"
#include "TileMap.h"
#include "TileMapScreen.h"
#include "Timer.h"
#include "Window.h"

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    sdlHandler = sh;
    fileHandler = fh;
    controls = ctrls;
	
	int rX = 4;
	int rY = 0;
	int rZ = -3;
	
	//Build test tile regions
	Terrain terra;
	TileRegion tr1; terra.testRegion(tr1, rX, rY, rZ, true);
	
	LevelSave ls(fh);
	
	std::cout << "Starting...\n";
	//ls.saveTileRegion(tr1, rX, rY, rZ);
	
	for( int i = 0; i<32; i++ ) {
		TileType tt; tt.setRGB(i*8, 50, i*8); tt.setTextureXY(i, 0);
		tr1.setTile(i, i, i, tt);
	}
	
	tr1.logInfo();
	
	//ls.saveTileRegion(tr1, rX, rY, rZ);
	
	ls.buildLevelDebugTex(sdlHandler, tex, rX, rY, rZ);
	
	lvlImgSrc.x = 0;
	lvlImgSrc.y = 0;
	lvlImgSrc.w = 32;
	lvlImgSrc.h = 128;
	
	std::cout << "Finished.\n";
}

Tests::~Tests(){}

/**/

void Tests::draw()
{
    counter++;
	
	SDL_Rect dst;
	dst.w = 32*6;
	dst.h = dst.w*4;
	dst.x = sdlHandler->getWidth()/2-dst.w/2;
	dst.y = sdlHandler->getHeight()/2-dst.h/2;
	
	tex.draw(&lvlImgSrc, &dst);
	
	int mw = controls->getMouseWheel();
	if( mw!=0 ) {
		
		if(mw<0) {
			lvlImgSrc.y += 32;
		}
		if (mw>0) {
			lvlImgSrc.y -= 32;
		}
		
		if(lvlImgSrc.y<0) {
			lvlImgSrc.y = 0;
		}
		
		controls->resetWheel(__PRETTY_FUNCTION__);
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

void Tests::putInfo(std::stringstream& ss, int& tabs)
{
	ss << "Test...\n";
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
