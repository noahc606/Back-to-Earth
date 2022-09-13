#include "ButtonAction.h"
#include "MainLoop.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh)
{
    switch( gh->getGUIActionID() )
    {
    case GUIHandler::btn_MAIN_exit:
        MainLoop::quit();
    break;
    case GUIHandler::btn_MAIN_options:
        gh->setGUIs(GUIHandler::GUIs::OPTIONS);
    break;

    case GUIHandler::btn_OPTIONS_back:
        gh->setGUIs(GUIHandler::GUIs::MAIN_MENU);
    break;
    }

}
ButtonAction::~ButtonAction(){}
