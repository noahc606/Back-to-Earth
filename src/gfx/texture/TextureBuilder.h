#pragma once
#include "SDLHandler.h"
#include "Texture.h"
#include "WindowData.h"

class TextureBuilder
{
public:
    /**/
    TextureBuilder(SDLHandler* sh);
    void init( int type, Texture& tex, int arg1, int arg2, int arg3, int arg4 );
    void init( int type, Texture& tex, int arg1, int arg2, int arg3 );
    void init( int type, Texture& tex, int arg1, int arg2 );
    void init( int type, Texture& tex, int arg1 );
    void init( int type, Texture& tex );
    /**/
    void buildWindow(Texture& tex, WindowData* wd, int width, int height);
    void buildRadioButton(Texture& tex, const Color& c, int srcX, int srcY);
    void buildRadioButton(Texture& tex, int srcX, int srcY);
    void buildCheckbox(Texture& tex, int srcX, int srcY);
    /**/
    enum TextureTypes
    {
        BTN_Tex, BTN_ShineTex,
        WINDOW,

        TILE_default,
    };
    /**/
protected:

private:
    /**/
    /**/
    void btn_tex(Texture &tex, int arg1, int arg2, int arg3, int arg4);
    void btn_shineTex(Texture &tex);
    void tile(Texture& tex);
    /**/
    SDLHandler* sdlHandler;

};
