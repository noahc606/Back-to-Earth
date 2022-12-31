#include "ButtonAction.h"
#include "Log.h"
#include "MainLoop.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh)
{
    switch( gh->getGUIActionID() )
    {
        /** Back-to buttons */
        case GUIHandler::btn_back_to_OPTIONS:{ gh->setGUIs(GUIHandler::GUIs::OPTIONS); }break;
        case GUIHandler::btn_back_to_MAIN:{ gh->setGUIs(GUIHandler::GUIs::MAIN); }break;


        /** Main menu buttons */
        case GUIHandler::btn_MAIN_exit: {
            MainLoop::quit();
        } break;
        case GUIHandler::btn_MAIN_options: {
            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        } break;

        /** Options buttons */
        case GUIHandler::btn_OPTIONS_controls: {
            gh->setGUIs(GUIHandler::GUIs::CONTROLS);
        } break;
        case GUIHandler::btn_OPTIONS_graphics_settings: {
            gh->setGUIs(GUIHandler::GUIs::GRAPHICS_SETTINGS);
        } break;

        /** Pause menu buttons */
        case GUIHandler::btn_PAUSED_options: {
            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        } break;

        /** Graphics settings buttons */

        /** Default */
        default: {
            std::stringstream ss;
            ss << "Clicked button with unset action ID: " << gh->getGUIActionID();
            Log::warn(__PRETTY_FUNCTION__, ss.str(), "deleting button");
        } break;
    }
}
ButtonAction::~ButtonAction(){}
