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
#include <SDL2/SDL_net.h>
#include "Color.h"
#include "DataStream.h"
#include "Grid.h"
#include "Log.h"
#include "MainLoop.h"
#include "Noise.h"
#include "Terrain.h"
#include "Real.h"
#include "TextOld.h"
#include "TileMap.h"
#include "TileMapScreen.h"
#include "Timer.h"
#include "Window.h"

Tests::Tests(){}

float dist(float x0, float y0, float z0, float x1, float y1, float z1)
{
    return std::sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) + (z1-z0)*(z1-z0) );
}

void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;

    float noiseCol[34][34][34];
    for(int x = 0; x<34; x++)
    for(int y = 0; y<34; y++)
    for(int z = 0; z<34; z++) {
        noiseCol[x][y][z] = 0;
    }

    Noise noiseGen(1000);


    double avg = 0.0;
    int numTests = 100;
    int a = 3;
    for(int c = 0; c<numTests; c++) {
        {
            Timer t1("noisetest");

            double avg2 = 0.0;
            for(int x = 0; x<34; x+=a)
            for(int y = 0; y<34; y+=a)
            for(int z = 0; z<34; z+=a) {
                noiseCol[x][y][z] = noiseGen.clampedSNoise3D(x/64.0, y/64.0, z/64.0);
            }

            float dc[8] = {0};
            if(false)
            for(int x = 0; x<34-a; x+=a)
            for(int y = 0; y<34-a; y+=a)
            for(int z = 0; z<34-a; z+=a) {
                //i, j, k all in {0, 1, 2}
                for(int i = 0; i<a; i++)
                for(int j = 0; j<a; j++)
                for(int k = 0; k<a; k++) {
                    dc[0] = dist(x  , y  , z  , x+i, y+j, z+k); //Distance from corner 0
                    dc[1] = dist(x  , y  , z+a, x+i, y+j, z+k); //...
                    dc[2] = dist(x  , y+a, z  , x+i, y+j, z+k);
                    dc[3] = dist(x  , y+a, z+a, x+i, y+j, z+k);
                    dc[4] = dist(x+a, y  , z  , x+i, y+j, z+k);
                    dc[5] = dist(x+a, y  , z+a, x+i, y+j, z+k);
                    dc[6] = dist(x+a, y+a, z  , x+i, y+j, z+k); //...
                    dc[7] = dist(x+a, y+a, z+a, x+i, y+j, z+k); //Distance from corner 7



                    //Grid::taxiDist()
                    //Grid::tax
                    //noiseCol[x+a][y+a][z+a]

                    

                    noiseCol[x+i][y+j][z+k] = noiseCol[x][y][z];
                }
            }

            avg += t1.getElapsedTimeMS();
        }
    }
    std::cout << "AVG: " << (avg/numTests) << "\n";


    /*
    
    
    */

    tex.init(sdlHandler, 34, 34);
    for(int x = 0; x<34; x++) {
        for(int y = 0; y<34; y++) {
            Color c(noiseCol[x][y][0]*255.0, noiseCol[x][y][0]*255.0, noiseCol[x][y][0]*255.0, 255);
            tex.pixel(x, y, c.getRGBA());
        }
    }
    
    //tex.pixel()
}

Tests::~Tests(){}

/**/

void drawlevel()
{
    /*
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
    */
	

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

void Tests::draw()
{
    tex.draw();

    double scale = 5;
    tex.setDrawScale(scale);
    tex.setDrawPos(sdlHandler->getWidth()/2-17*scale, sdlHandler->getHeight()/2-17*scale);
    //Text txt;
    //txt.init(sdlHandler);
    //txt.setString("The quick brown fox jumps over the lazy dog.");
    //Texture* tex = txt.getTexture();
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
