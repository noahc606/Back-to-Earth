#include "Tests.h"
#include <map>
#include "Color.h"
#include "Text.h"
#include "Timer.h"
#include "Window.h"
#include "Real.h"
#include "Noise.h"
/**/

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);

    //SpriteSheet ss;
    //ColorPalette cpal; cpal.init(cpal.DEFAULT_PLAYER);

    //SpriteSheetBuilder ssb(sh);

    //ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

    stex.init(sdlHandler);
    stex.setTexDimensions(512, 512);


    Timer tim1("streaming texture manipulation");
    stex.lock(0, 0, 600, 600);
    stex.blit();
    stex.unlock();
    stex.update();

    stex.lock(40, 40, 300, 300);
    stex.update();

    tim1.debugElapsedTimeMS();

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
Tests::~Tests(){}

/**/

void Tests::draw()
{
    stex.draw();
}

void Tests::tick()
{

}

/**/

void Tests::info(std::stringstream& ss, int& tabs)
{

}
