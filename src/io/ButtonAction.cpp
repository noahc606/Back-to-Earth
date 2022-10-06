#include "ButtonAction.h"
#include "MainLoop.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh)
{
    switch( gh->getGUIActionID() )
    {
        /** Main menu buttons */
        case GUIHandler::btn_MAIN_exit:
            MainLoop::quit();
        break;
        case GUIHandler::btn_MAIN_options:
            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        break;

        /** Options menu buttons */
        case GUIHandler::btn_OPTIONS_back:
            gh->setGUIs(GUIHandler::GUIs::MAIN_MENU);
        break;

        /** Pause menu buttons */
        case GUIHandler::btn_PAUSED_options:
            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        break;
    }

}
ButtonAction::~ButtonAction(){}
