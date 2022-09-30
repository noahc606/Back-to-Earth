#include "TextureBuilder.h"
#include "TextureLoader.h"

TextureBuilder::TextureBuilder(SDLHandler* sh)
{
    sdlHandler = sh;
}

void TextureBuilder::init(int type, Texture &tex, int arg1, int arg2, int arg3, int arg4)
{
    switch(type)
    {
        case BTN_Tex:       btn_tex(tex, arg1, arg2, arg3, arg4); break;
        case BTN_ShineTex:  btn_shineTex(tex); break;
        case WINDOW:        window(tex, arg1, arg2); break;

        case TILE_default:  tile(tex); break;
    }
}

void TextureBuilder::init(int type, Texture &tex, int arg1, int arg2, int arg3)
{ init(type, tex, arg1, arg2, arg3, -1); }

void TextureBuilder::init(int type, Texture &tex, int arg1, int arg2)
{ init(type, tex, arg1, arg2, -1, -1); }

void TextureBuilder::init(int type, Texture &tex, int arg1)
{ init(type, tex, arg1, -1, -1, -1); }

void TextureBuilder::init(int type, Texture &tex)
{ init(type, tex, -1, -1, -1, -1); }


/*
    arg1 = tex width
    arg2 = tex height
    arg3 = button type location X
    arg4 = button type location Y
*/
void TextureBuilder::btn_tex(Texture &tex, int arg1, int arg2, int arg3, int arg4)
{
    /* Texture initialization */
    tex.init(sdlHandler);
    tex.setTexRect(0, 0, arg1, 16);
    tex.setDrawScale(2);
    tex.setBlendMode(SDL_BLENDMODE_BLEND);

    /* Manipulating Texture */
    //image id
    int btn = TextureLoader::Textures::GUI_button;

    //First 3 columns of pixels
    tex.lock(0, 0, 3, arg2);
    tex.blit(btn, arg3, arg4);
    //Middle sets of 32 columns of pixels
    for(int i = 0; i<arg1-32; i+=32) {
        tex.lock(3+i, 0, 32, 16);
        tex.blit(btn, 4+arg3, arg4);
    }
    //Last 3 columns of pixels
    tex.lock(arg1-3, 0, 3, arg2);
    tex.blit(btn, 37+arg3, arg4);
}

void TextureBuilder::btn_shineTex(Texture &tex)
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

void TextureBuilder::window(Texture& tex, int texW, int texH)
{
    tex.init(sdlHandler);
    tex.setTexDimensions(texW+8, texH+8);

    int img = TextureLoader::GUI_window;

    /* Window */
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
    //Middle 32x32 squares
    for(int i = 0; i<texW; i+=32) {
        for(int j = 0; j<texH; j+=32) {
            tex.lock(4+i, 4+j, 32, 32);
            tex.blit(img, 5, 5);
        }
    }
}

void TextureBuilder::tile(Texture& tex)
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
