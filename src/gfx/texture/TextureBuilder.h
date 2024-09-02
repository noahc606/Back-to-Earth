#pragma once
#include "SDLHandler.h"
#include "Texture.h"
#include "WindowData.h"

class TextureBuilder
{
public:
    /**/
    TextureBuilder(SDLHandler* sh);
    /**/
    void buildWindow(Texture& tex, WindowData* wd, int width, int height);
    void buildRadioButton(Texture& tex, const nch::Color& c, int srcX, int srcY);
    void buildRadioButton(Texture& tex, int srcX, int srcY);
    void buildCheckbox(Texture& tex, int srcX, int srcY);
	void buildButton(Texture& tex, int srcX, int srcY, int width);
	void buildButtonShine(Texture& tex);
    void buildDefaultTile(Texture& tex);
    /**/
protected:

private:
    /**/
    /**/
    /**/
    SDLHandler* sdlHandler;

};
