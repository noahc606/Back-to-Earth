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

void save(FileHandler* fileHandler)
{
	TileRegion tr;
	Terrain terra;
	terra.populateRegion(tr, 0, 0, 0);
	
	//LTR = (32*16)x(32*16)x(32*4)=512x512x128	
	fileHandler->openFile("dump/test5.bte_ltr", FileHandler::WRITE, true);
	DataStream ds;
	//Magic Number
	ds.put64Bits(0xaf0e66d7af0e66d7);
	ds.put64Bits(0xce529030ce529030);
	ds.seekBitDelta(64*2);
	
	//File Header (32*32*32=32768 entries. each having 40 bits of data.
	//This data represents the location (in bytes) within the file for the save data.
	int numBits = 40;
	for(int i = 0; i<32*32*32; i++) {
		ds.putXBits(0b1111111111111111111111111111111111111111, numBits);
		ds.seekBitDelta(numBits);
	}
	
	uint8_t dsx = 0;	//0-1
	uint8_t dsy = 0;	//0-1
	uint8_t dsz = 4;	//0-7
	int psb = tr.getPaletteSizeBucket();
	std::cout << "Tile: " << tr.getTile(0, 0, 0).getVal() << "\n";
	//Total of 16*16*4=1024 tiles. Each tile = 'psb' bits. Two hex chars = 1 byte.
	for( uint8_t sx = dsx*16; sx<dsx*16+16; sx++ ) {
		for( uint8_t sy = dsy*16; sy<dsy*16+16; sy++ ) {
			for( uint8_t sz = dsz*4; sz<dsz*4+4; sz++ ) {
				ds.putXBits( tr.getTileKey(sx, sy, sz), psb );
				ds.seekBitDelta(psb);
			}
		}
	}
		
	ds.dumpBytestream(fileHandler);
	fileHandler->saveCloseFile();
}

void load()
{
	
}

void allocNew(DataStream& ds, uint8_t bitsize)
{
    int nhe = 16*16*4;  // (n)umber of (h)eader (e)ntries
    int she = 9;        // (s)ize of 1 (h)eader (e)ntry (in bytes)

    for( int i = 0; i<nhe; i++ ) {
        ds.seekByte(9*she);

        ds.peekHexDigitCell();
    }
}

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    sdlHandler = sh;
    fileHandler = fh;
    controls = ctrls;
}

Tests::~Tests(){}

/**/

void Tests::draw()
{
    counter++;

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
	std::cout << "test";
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
