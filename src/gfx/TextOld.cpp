#include "TextOld.h"
#include <nch/cpp-utils/io/Log.h>
#include <sstream>


/**/

TextOld::TextOld()
{
    foreground.set(255, 255, 255);
    background.set(0, 0, 0, 0);
}
void TextOld::init(SDLHandler* sh)
{
    sdlHandler = sh;

    //Build insertionBarTex
    insertionBarTex.init(sh);
    insertionBarTex.setTexRect(sX, sY, 5, 7);
    insertionBarTex.setDrawScale(scale);

    SDL_Rect sr;
    getCharTexRect( '|', sr.x, sr.y, sr.w, sr.h );
    insertionBarTex.lock(0, 0, sr.w, sr.h);
    insertionBarTex.blit(fontType, sr.x, sr.y);
}
TextOld::~TextOld(){}

/**/

void TextOld::draw(const NCH_Color& fg)
{
    if( insertionPoint>-1 && selected ) {

        insertionBarTex.setDrawPos(sX+insertionPointPx-scale, sY);
        if( SDL_GetTicks()%1000<500 ) {
            insertionBarTex.draw();
        }
    }

    draw(sdlHandler, text, sX, sY, scale, fg, background, fontType);
}

void TextOld::draw()
{
    NCH_Color c(255, 255, 255);
    draw(c);
}

/**
 *  Draw text to a texture, rather than directly to the screen (if tex!=nullptr, this function is broken - have to replace this entire class)
*/
int TextOld::draw(SDLHandler* sdlHandler, Texture* tex, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg, int fontType)
{
    //Width of text
    int res = 0;

    if(tex!=nullptr) {
        TextOld t;
        t.init(sdlHandler);
        t.setString(s);
        
        tex->setTexHeight(7);
        tex->setTexWidth(t.getWidth()/scale);
    }

    //Make sure x and y are divisible by the scale, in order to avoid misalignments of pixels
    x = x/scale*scale;
    y = y/scale*scale;
    int x0 = x;

    //Src and dst rectangles for drawing
    SDL_Rect src; SDL_Rect dst;
    //Track whether to draw the current character or not
    bool drawChar = true;
    //Iterate through each character in s
    for(char c : s) {
        //Should we draw this character or not?
        drawChar = true;
        switch(c) {
        case ' ':
        case '\t':
        case '\n':
            drawChar = false;
        break;
        }

        //Draw the character if it is not a special character.
        if( drawChar ) {
            TextOld::getCharTexRect(c, src.x, src.y, src.w, src.h);
            dst.x = x; dst.y = y; dst.w = 5*scale; dst.h = 7*scale;

            if(tex==nullptr) {
                sdlHandler->setColorMod(fontType, fg);
                sdlHandler->renderCopy(fontType, &src, &dst);
                if(dst.x+dst.w-x0>res) {
                    res = dst.x+dst.w-x0;
                }
            }
        }

        //Add 6 to x to get position ready for the next character
        x += 6*scale;

        int dX = 0;
        int dY = 0;

        //IF we are using the 'bte' font:
        if( fontType==TextureLoader::GUI_FONT_bte ) {
            //Based on the size of the character, change dX by a few pixels
            switch(c) {
                case 16:  dX = -1; break;
                case 17:  dX = -1; break;
                case 18:  dX = -2; break;
                case ' ': dX = -1; break;
                case '!': dX = -3; break;
                case '"': dX = -2; break;
                case '\'':dX = -4; break;
                case '(': dX = -3; break;
                case ')': dX = -3; break;
                case '*': dX = -2; break;
                case '+': dX = -2; break;
                case ',': dX = -3; break;
                case '-': dX = -2; break;
                case '.': dX = -4; break;
                case '/': dX = -2; break;
                case ':': dX = -4; break;
                case ';': dX = -3; break;
                case 'I': dX = -2; break;
                case '[': dX = -3; break;
                case '\\':dX = -3; break;
                case ']': dX = -3; break;
                case '^': dX = -2; break;
                case '`': dX = -3; break;
                case 'b': dX = -1; break;
                case 'd': dX = -1; break;
                case 'f': dX = -1; break;
                case 'i': dX = -4; break;
                case 'j': dX = -2; break;
                case 'k': dX = -1; break;
                case 'l': dX = -3; break;
                case 'p': dX = -1; break;
                case 'q': dX = -1; break;
                case 'r': dX = -1; break;
                case 't': dX = -1; break;
                case 'u': dX = -1; break;
                case '{': dX = -2; break;
                case '|': dX = -4; break;
                case '}': dX = -2; break;
            }
        }

        //Change dX and dY when encountering tabs, newlines, etc
        switch(c) {
            case '\t': dX = 11; break;
            case '\n': {
                dY = 10;
                x = x0;
            } break;
        }

        //Make sure the amount being added is scaled
        dX *= scale;
        dY *= scale;

        //Add dX and dY to x and y
        x += dX;
        y += dY;
    }

    return res;
}

/**
    Draws a string of length n in O(n) time and doesn't require the creation of any TextOld object.
    Perfectly fine for small to medium sized strings (n<~200).
    For large strings, it is recommended to create a TextOld object and change the string line by line.
*/
int TextOld::draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg, int fontType)
{
    return draw(sdlHandler, nullptr, s, x, y, scale, fg, bg, fontType);
}

int TextOld::draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale, const NCH_Color& fg, const NCH_Color& bg)
{
    return draw(sdlHandler, s, x, y, scale, NCH_Color(255, 255, 255), NCH_Color(0, 0, 0, 0), TextureLoader::GUI_FONT_bte);
}

int TextOld::draw(SDLHandler* sdlHandler, std::string s, int x, int y, int scale)
{
    return draw(sdlHandler, s, x, y, scale, NCH_Color(255, 255, 255), NCH_Color(0, 0, 0, 0));
}

/**/

std::string TextOld::getString() { return text; }

int TextOld::getInsertionPoint() { return insertionPoint; }

void TextOld::getCharTexRect(char c, int& srcX, int& srcY, int& srcW, int& srcH)
{
    srcX = 5*(c%16);
    srcY = 7*(c/16);
    srcW = 5;
    srcH = 7;
}

int TextOld::getWidth() { return width; }

/**/

void TextOld::setString(std::string p_text)
{
    text = p_text;

    SDL_Rect sr;
    SDL_Rect dr; dr.x = 1; dr.y = 0; dr.w = 5; dr.h = 7;

    for(char c : text) {

        //When dr.x goes over the maxWidth, go to next line by setting dr.x to 0 and adding 10 to dr.y
        if( maxWidth>=0 && dr.x>maxWidth/scale ) {
            dr.x = 1;
            dr.y += 10;
        }

        //Add a non-newline character to the pixel array of txtTex.
        if(c != '\n') {
            //Get location of character 'c' in the texture file
            getCharTexRect( c, sr.x, sr.y, sr.w, sr.h );
            if(c=='\t') getCharTexRect( ' ', sr.x, sr.y, sr.w, sr.h );

        //When encountering a newline, do pretty much the same as when dr.x goes over maxWidth.
        } else {
            c = '\n';
            dr.x = -6;
            dr.y += 10;
        }

        //Add to dr.x depending on however long character 'c' is.
        modifyCurrentCharX(c, dr.x, true);
    }

    //Total lengths of each character.
    width = dr.x*scale;
}

void TextOld::setMaxWidth(int p_maxWidth) { maxWidth = p_maxWidth; }

/*
    Get and set insertion point based on x value on screen
*/
void TextOld::setInsertionPointByPx(int dx)
{
    int px = 0;
    unsigned int i = 0;
    for( ; ( i<text.size()&&px+5*scale<dx ); i++ ) {
        modifyCurrentCharX(text[i], px, true);
    }

    insertionPointPx = px;//-5*scale;
    insertionPoint = i;
}

void TextOld::setInsertionPoint(int x)
{
    int px = 0;
    for(int i = 0; i<x; i++ ) {
        modifyCurrentCharX(text[i], px, true);
    }

    insertionPointPx = px;

    if(x<0) x = 0;
    if(x>(int)text.size()) x = text.size();
    insertionPoint = x;
}

void TextOld::setSelected(bool p_selected) { selected = p_selected; }

void TextOld::setPos(int p_sX, int p_sY)
{
    sX = p_sX/scale*scale;
    sY = p_sY/scale*scale;
    txtTex.setDrawPos(sX, sY);
}

void TextOld::setFontType(int ft)
{
    //Set font type
    fontType = ft;
    //Make sure font type is valid. If not use the default 'bte' font type
    switch(ft) {
        case TextureLoader::GUI_FONT_bte:
        case TextureLoader::GUI_FONT_robot: { } break;

        default: {
            std::stringstream ss; ss << "Using default font for unknown font ID '" << ft << "'";
            NCH_Log::warn(__PRETTY_FUNCTION__, ss.str());
            fontType = TextureLoader::GUI_FONT_bte;
        } break;
    }
}

void TextOld::modifyCurrentCharX(char& c, int& x, bool p_scale)
{
    //Add 6 to x to get position ready for the next character
    int initX = x;
    x += 6;

    //Based on the size of the character, decrement dr.x by a few pixels
    switch(c) {
        case 16:  x -= 1; break;
        case 17:  x -= 1; break;
        case 18:  x -= 2; break;
        case ' ': x -= 1; break;
        case '!': x -= 3; break;
        case '"': x -= 2; break;
        case '\'':x -= 4; break;
        case '(': x -= 3; break;
        case ')': x -= 3; break;
        case '*': x -= 2; break;
        case '+': x -= 2; break;
        case ',': x -= 3; break;
        case '-': x -= 2; break;
        case '.': x -= 4; break;
        case '/': x -= 2; break;
        case ':': x -= 4; break;
        case ';': x -= 3; break;
        case 'I': x -= 2; break;
        case '[': x -= 3; break;
        case '\\':x -= 3; break;
        case ']': x -= 3; break;
        case '^': x -= 2; break;
        case '`': x -= 3; break;
        case 'b': x -= 1; break;
        case 'd': x -= 1; break;
        case 'f': x -= 1; break;
        case 'i': x -= 4; break;
        case 'j': x -= 2; break;
        case 'k': x -= 1; break;
        case 'l': x -= 3; break;
        case 'p': x -= 1; break;
        case 'q': x -= 1; break;
        case 'r': x -= 1; break;
        case 't': x -= 1; break;
        case 'u': x -= 1; break;
        case '{': x -= 2; break;
        case '|': x -= 4; break;
        case '}': x -= 2; break;

        case '\t': x+=11; break;
    }

    if( p_scale ) {
        int lastX = x;
        x = initX;
        x += ((lastX-initX)*scale);
    }
}
