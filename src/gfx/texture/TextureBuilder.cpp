#include "TextureBuilder.h"
#include "TextureLoader.h"

TextureBuilder::TextureBuilder(SDLHandler* sh)
{
    sdlHandler = sh;
}

void TextureBuilder::buildWindow(Texture& tex, WindowData* winData, int texW, int texH)
{
    tex.init(sdlHandler);
    tex.setTexDimensions(texW+8, texH+8);

    int img = TextureLoader::GUI_window;

    /** Window corners/borders */
    //Set border color
    tex.setColorMod( winData->getBorderColor() );
    //4 corners of window
    tex.lock(     0,      0, 4, 4); tex.blit(img,  0,  0); //Top left
    tex.lock(texW+4,      0, 4, 4); tex.blit(img, 38,  0); //Top right
    tex.lock(     0, texH+4, 4, 4); tex.blit(img,  0, 38); //Bottom left
    tex.lock(texW+4, texH+4, 4, 4); tex.blit(img, 38, 38); //Bottom right
    //4 borders of window
    for(int i = 0; i<texH; i+=32) {
        tex.lock(     0,    4+i,  4, 32); tex.blit(img,  0,  5); //Left border
        tex.lock(texW+4,    4+i,  4, 32); tex.blit(img, 38,  5); //Right border
    }
    for(int i = 0; i<texW; i+=32) {
        tex.lock(   4+i,      0, 32,  4); tex.blit(img,  5,  0); //Top border
        tex.lock(   4+i, texH+4, 32,  4); tex.blit(img,  5, 38); //Bottom border
    }

    /** Window interior */
    //Middle 32x32 squares
    for(int i = 0; i<texW; i+=32) {
        for(int j = 0; j<texH; j+=32) {
            tex.setColorMod( winData->getPanelColor(i/32, j/32) );

            tex.lock(4+i, 4+j, 32, 32);
            tex.blit(img, 5, 5);
        }
    }

    tex.setColorMod(255, 255, 255);
}

void TextureBuilder::buildRadioButton(Texture& tex, const nch::Color& c, int srcX, int srcY)
{
    tex.init(sdlHandler);
    tex.setTexDimensions(10, 10);
    tex.setDrawScale(2);
    tex.setColorMod(c);
    tex.lock();
    tex.blit(TextureLoader::GUI_button, srcX, srcY);
}

void TextureBuilder::buildRadioButton(Texture& tex, int srcX, int srcY)
{
    buildRadioButton(tex, nch::Color(), srcX, srcY);
}

void TextureBuilder::buildCheckbox(Texture& tex, int srcX, int srcY)
{
    buildRadioButton(tex, nch::Color(), srcX, srcY);
}

void TextureBuilder::buildButton(Texture& tex, int srcX, int srcY, int width) {
	int height = 16;
    int btn = TextureLoader::Textures::GUI_button;

	tex.init(sdlHandler);
	tex.setTexRect(0, 0, width, height);
	tex.setDrawScale(2);
	tex.setBlendMode(SDL_BLENDMODE_BLEND);
	
    /* First 3 columns of pixels */
    tex.lock(0, 0, 3, height);
    tex.blit(btn, srcX, srcY);
	
	/* Middle columns of pixels */
	//Middle (width-6) columns of pixels
	int progress = 3;
	while( progress<=width-32 ) {
		tex.lock(progress, 0, 32, height);
		tex.blit(btn, srcX+4, srcY);
		progress += 32;
	}
	//Blit last few pixels of the middle
	tex.lock(progress, 0, width-progress, height);
	tex.blit(btn, srcX+4, srcY);
	
	/* Last 3 columns of pixels */
	//Last 3 columns of pixels
    tex.lock(width-3, 0, 3, height);
    tex.blit(btn, srcX+37, srcY);
}

void TextureBuilder::buildButtonShine(Texture& tex)
{
	/* Texture initialization */
	tex.init(sdlHandler);
	tex.setTexRect(0, 0, 18, 16);
	tex.setDrawScale(2);
	tex.setBlendMode(SDL_BLENDMODE_BLEND);

	/* Manipulating Texture */
	//image id
	int btn = TextureLoader::Textures::GUI_button;

	//Single rectangle
	tex.lock(0, 0, 18, 16);
	tex.blit(btn, 0, 51);
}

void TextureBuilder::buildDefaultTile(Texture& tex)
{
    //Build "default" tile.
    tex.init(sdlHandler);
    tex.setTexDimensions(32, 32);
    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            tex.pixel(x, y, 128, x*8, y*8);
        }
    }
    tex.setDrawScale(4);
}
