#pragma once
#include <nch/cpp-utils/color.h>
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
    void draw(const nch::Color& fg);
    void draw();
    static int draw(SDLHandler* sdlHandler, Texture* tex, std::string s, int x, int y, int scale, const nch::Color& fg, const nch::Color& bg, int fontType);
    static int draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const nch::Color& fg, const nch::Color& bg, int fontType);
    static int draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const nch::Color& fg, const nch::Color& bg);
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


    nch::Color foreground;
    nch::Color background;
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
