#include "GUI.h"
#include <nch/cpp-utils/io/Log.h>
#include "GUIHandler.h"


GUI::GUI(){}
void GUI::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);
    setUserGUI();
}
void GUI::init(SDLHandler* sh, Controls* ctrls)
{
    BTEObject::init(sh, nullptr, ctrls);
    setUserGUI();
}
void GUI::init(SDLHandler* sh)
{
    BTEObject::init(sh, nullptr, nullptr);
    setUserGUI();
}

GUI::~GUI(){}
void GUI::destroy(){}

void GUI::draw(){}
void GUI::tick(){}

bool GUI::isUserGUI() { return userGUI; }
bool GUI::isWindowComponent() { return windowComponent; };
int GUI::getExtraID() { return extraID; }

void GUI::setExtraID(int p_exid)
{
    if( exists() ) {
        NCH_Log::warn(__PRETTY_FUNCTION__, "Tried to set the extraID of an object after it was initialized");
    } else {
        extraID = p_exid;
    }
}

void GUI::setUserGUI()
{
    userGUI = true;
    switch( getID() ) {
        case GUIHandler::tbx_DEBUG:
            userGUI = false;
        break;
    }
}
