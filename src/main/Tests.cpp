#include "Tests.h"
#include <map>
#include "Color.h"
#include "Text.h"
#include "Timer.h"
#include "Window.h"
#include "Real.h"
#include "Noise.h"

#include <math.h>
#include <bitset>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <io.h>
#include <fcntl.h>
using namespace std;
/**/

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);

    fh->getSettings()->kv(Settings::TextFiles::options, "asdf", "1234");
    fh->saveSettings(Settings::TextFiles::options);

    /*
    Timer t;
    fh->editFile("level123", "bte_tr");
    std::srand(99);
    int layers = 3;
    for(int i = 0; i<layers; i++ ) {
        fh->write("layer");
        fh->write(i);
        fh->write("=");
        int c = 2048;
        for(int j = 0; j<c; j++) {

            int val = std::rand()%256;
            if( val=='\n' || val=='=' || val=='' || val=='\0' ) {
                fh->write('');
            }

            fh->write((char)val);
        }
        fh->writeln();
    }
    fh->saveAndCloseFile();
    t.debugElapsedTimeMS();
    */

    //8 characters = 3 tiles
    //1 tile = 16 bits, each character = 6bits
    uint16_t arr[32][32] = {0};
    arr[0][0] = 0b0101101111110101;
    arr[1][0] = 0b1100000011111100;
    arr[2][0] = 0b0000111111111111;
    arr[3][0] = 0b1111111111111111;
    arr[4][0] = 0b1111111111111111;
    arr[5][0] = 0b1111111111111111;
    arr[6][0] = 0b1111111111111111;
    arr[7][0] = 0b1111111100000000;

    arr[31][0] = 0b1111111111111111;

    int x = 0;
    int y = 0;
    int amps[] = {
        0b1000000000000000,
        0b0100000000000000,
        0b0010000000000000,
        0b0001000000000000,
        0b0000100000000000,
        0b0000010000000000,
        0b0000001000000000,
        0b0000000100000000,
        0b0000000010000000,
        0b0000000001000000,
        0b0000000000100000,
        0b0000000000010000,
        0b0000000000001000,
        0b0000000000000100,
        0b0000000000000010,
        0b0000000000000001 };
    uint8_t pows2[] = { 32, 16, 8, 4, 2, 1 };

    Timer t2("level saving 2");

    uint8_t counter = 0;
    uint8_t bits = 0;
    char base64Char = 64;
    fh->editFile("level123", "bte_tr");
    fh->write("layer0=");
    while(true) {
        if( (arr[x][y]&amps[bits])>0 ) {
            base64Char += pows2[counter];
        }

        counter++;
        if(counter==6) {
            fh->write(base64Char);
            base64Char = 64;
            counter = 0;
        }

        bits++;
        if(bits==16) {
            bits = 0;
            x++;
            if(x>31) {
                fh->write(base64Char);
                base64Char = 64;
                counter = 0;
                x = 0;
                y++;
                if( y>31 ) {
                    y = 0;
                    break;
                } else {
                    fh->write("\nlayer");
                    fh->write(y+1);
                    fh->write("=");
                }
            }
        }
    }
    fh->saveAndCloseFile();



    t2.debugElapsedTimeMS();

    //Settings::t_kvMap kvMap = fh->readFileKVs("level123", "bte_tr");

    //std::string s = Settings::get(kvMap, "layera");

    //for(char c : s) {
        //c<<3;
    //}
}
Tests::~Tests(){}

/**/

void Tests::draw()
{

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
