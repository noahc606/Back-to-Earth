#include "Tests.h"
#include "Color.h"
#include "Text.h"
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

    tex.init(sdlHandler);
    tex.setTexDimensions(514, 514);

    Noise noise;
    tex.rect(0, 0, 514, 514, 255, 0, 0);

    float zoom = 256.0;
    float tx = 0.0;
    float ty = 0.0;

    for( int x = 0; x<512; x++ ) {
        for( int y = 0; y<512; y++ ) {
            float n = noise.noise2D((float)x/zoom+tx, (float)y/zoom+ty);
            int c = (n*0.5d+0.5d)*256.0;
            tex.px(x+1, y+1, c, c, c );
        }
    }


}
Tests::~Tests(){}

/**/

void Tests::draw()
{
    tex.draw();
}

void Tests::tick()
{

}

/**/

void Tests::info(std::stringstream& ss, int& tabs)
{
    //DebugScreen::newGroup(ss, tabs, "Noise");
    //noise.info(ss, tabs);
    //DebugScreen::endGroup(tabs);
}
