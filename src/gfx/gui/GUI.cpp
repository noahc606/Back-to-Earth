#include "GUI.h"
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

void GUI::setUserGUI()
{
    userGUI = true;
    switch( getID() ) {
        case GUIHandler::tb_DEBUG:
            userGUI = false;
        break;
    }
}

GUI::~GUI(){}
void GUI::destroy(){}
void GUI::draw(){}
void GUI::tick(){}

bool GUI::isUserGUI() { return userGUI; }
bool GUI::isWindowComponent() { return windowComponent; };
