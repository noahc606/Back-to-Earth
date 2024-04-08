#include "DebugScreen.h"
#include <sstream>
#include "MainLoop.h"
#include "TextOld.h"
#include "TextBox.h"
#include "TextureLoader.h"

DebugScreen::DebugScreen(){}
void DebugScreen::init(SDLHandler* sh, GUIHandler* guis, Controls* ctrls)
{
    sdlHandler = sh;
    guiHandler = guis;
    controls = ctrls;

    bg.set(255, 255, 255, 0);
}
DebugScreen::~DebugScreen(){}

/**/

void DebugScreen::draw()
{
    if( debugDrawTimeLeft>0 ) {
        TextureLoader* tl = sdlHandler->getTextureLoader();

        SDL_Texture* stex = tl->getTexture(debugDrawTexID);
        int texW = 0; int texH = 0;
        int sdlW = sdlHandler->getWidth(); int sdlH = sdlHandler->getHeight();
        SDL_QueryTexture(stex, nullptr, nullptr, &texW, &texH);


        SDL_Rect dr;
        dr.w = texW*512; dr.h = texH*512;

        while( dr.w>sdlW*3/4 || dr.h>sdlH*3/4 ) {
            dr.w /= 2;
            dr.h /= 2;
        }

        dr.x = (sdlW-dr.w);
        dr.y = 0;

        SDL_RenderCopy(
            sdlHandler->getRenderer(),
            stex,
            NULL,
            &dr
        );

        std::stringstream txt;
        txt << "Dimensions:\n" << texW << "x" << texH;
        TextOld::draw(sdlHandler, txt.str(), dr.x, dr.h, 2, fg, bg, TextureLoader::GUI_FONT_robot);
    }

    if(visible) {
        TextOld::draw(sdlHandler, debugString, 2, 2, 2, fg, bg, TextureLoader::GUI_FONT_robot);
    }
}

void DebugScreen::tick()
{
    if( debugDrawTimeLeft>0 ) {
        debugDrawTimeLeft--;
    }

    //If RSHIFT is pressed
    bool rshift = false;
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if( (state[SDL_SCANCODE_RSHIFT] ) )
    {
        rshift = true;
    }

    //If FUNC_DEBUG and CTRL and ALT are pressed
    if( controls->isPressed("FUNC_DEBUG") && rshift && haxEnabled ) {
        if( hax0rMode ) {
            setHax0rMode(false);
        } else {
            setHax0rMode(true);
        }
        //Stop press
        controls->stopPress("FUNC_DEBUG", __PRETTY_FUNCTION__);
    } else
    //If only FUNC_DEBUG is pressed
    if( controls->isPressed("FUNC_DEBUG") ) {
        //If debug screen is visible, set to invisible.
        if(visible) {
            visible = false;
        //If debug screen is invisible, set to visible.
        } else {
            visible = true;
        }
        //Stop press
        controls->stopPress("FUNC_DEBUG", __PRETTY_FUNCTION__);
    }

    //If hax are disabled for this user
    if(!haxEnabled) {
        setHax0rMode(false);
    }

    //If 1337 hax0r
    if(hax0rMode) {
        int _5p33d = 5;
        //(y(13 f0r39r0und 73x7 R68
        if(fg.r == 255) rgbCycleState[0] = true;
        if(fg.g == 255) rgbCycleState[1] = true;
        if(fg.b == 255) rgbCycleState[2] = true;
        if(fg.r == 0) rgbCycleState[0] = false;
        if(fg.g == 0) rgbCycleState[1] = false;
        if(fg.b == 0) rgbCycleState[2] = false;
        if(rgbCycleState[0]) { fg.r-=_5p33d; } else { fg.r+=_5p33d; }
        if(rgbCycleState[1]) { fg.g-=_5p33d; } else { fg.g+=_5p33d; }
        if(rgbCycleState[2]) { fg.b-=_5p33d; } else { fg.b+=_5p33d; }
    }
}

bool DebugScreen::getVisible() { return visible; }


void DebugScreen::setVisible(bool p_visible)
{
    visible = p_visible;
}

void DebugScreen::setHaxEnabled(bool p_haxEnabled)
{
    haxEnabled = p_haxEnabled;

    if( haxEnabled==false ) {
        setHax0rMode(false);
    }

}

void DebugScreen::debugSetString(std::string s) { debugString = s; }
void DebugScreen::debugDrawTexTemporarily(int id)
{
    debugDrawTimeLeft = 5*60;
    debugDrawTexID = id;
}

void DebugScreen::newGroup(std::stringstream& ss, int& indents, std::string s)
{
    indentLine(ss, indents);
    ss << "[";
    ss << s;
    ss << "]\n";

    indents++;
}
void DebugScreen::endGroup(int& indents)
{
    indents--;
}
void DebugScreen::indentLine(std::stringstream& ss, int& indents)
{
    for(int i = 0; i<indents; i++) {
        ss << "\t";
    }
}
void DebugScreen::newLine(std::stringstream& ss)
{
    ss << "\n";
}


void DebugScreen::setHax0rMode(bool val)
{
    if(val) {
        fg.set(0, 85, 170, 255);
        hax0rMode = true;
        guiHandler->addGUI( new TextBox(0, 0, 800, GUIHandler::ID::tbx_DEBUG) );
    } else {
        fg.set(255, 255, 255, 255);
        hax0rMode = false;
        guiHandler->removeGUI(GUIHandler::ID::tbx_DEBUG);
    }
}


