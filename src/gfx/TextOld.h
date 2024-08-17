#pragma once
#include <nch/cpp-utils/gfx/Color.h>
#include <string>
#include "SDLHandler.h"
#include "Texture.h"
#include "TextureLoader.h"

class TextOld
{
public:
    /**/
    TextOld();
    void init(SDLHandler* sh);
    virtual ~TextOld();
    /**/
    void draw(const NCH_Color& fg);
    void draw();
    static int draw(SDLHandler* sdlHandler, Texture* tex, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg, int fontType);
    static int draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg, int fontType);
    static int draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg);
    static int draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale);
    /**/
    std::string getString();
    int getInsertionPoint();
    static void getCharTexRect(char c, int& srcX, int& srcY, int& srcW, int& srcH);
    int getWidth();
    /**/
    void setString(std::string text);
    void setMaxWidth(int maxWidth);
    void setInsertionPointByPx(int dx);
    void setInsertionPoint(int x);
    void setSelected(bool selected);
    void setPos(int sX, int sY);
    void setFontType(int ft);
    /**/


    NCH_Color foreground;
    NCH_Color background;
    int sX = 0; int sY = 0; int scale = 2;
    Texture txtTex;
    Texture insertionBarTex;
    int fontType = TextureLoader::GUI_FONT_bte;

    std::string text = "null";
    int width = 0; int maxWidth = -1;
    int insertionPoint = -1;
    int insertionPointPx = 0;
    bool selected = false;

protected:

private:
    /**/
    void modifyCurrentCharX(char& c, int& x, bool p_scale);
    /**/

    SDLHandler* sdlHandler;
};
