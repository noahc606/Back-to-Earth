#include "DebugScreen.h"
#include <sstream>
#include "MainLoop.h"
#include "Text.h"
#include "TextBox.h"

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
    if(visible) {
        Text::draw(sdlHandler, debugString, 2, 2, 2, fg, bg, TextureLoader::GUI_FONT_robot);
    }
}

void DebugScreen::tick()
{
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

bool DebugScreen::getVisible() { return visible; }

void DebugScreen::setDebugString(std::string s) { debugString = s; }

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
        guiHandler->addGUI( new TextBox(0, 0, 800, GUIHandler::ID::tb_DEBUG) );
    } else {
        fg.set(255, 255, 255, 255);
        hax0rMode = false;
        guiHandler->removeGUI(GUIHandler::ID::tb_DEBUG);
    }
}


