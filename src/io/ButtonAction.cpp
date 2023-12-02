#include "ButtonAction.h"
#include "CheckBox.h"
#include "FileHandler.h"
#include "Log.h"
#include "MainLoop.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
    Settings* stgs = fh->getSettings();

    switch( gh->getGUIActionID() )
    {
        /** Back-to buttons */
        case GUIHandler::btn_back_to_OPTIONS: {
            int objIDs[] = {
                GUIHandler::ID::cbx_GRAPHICS_SETTINGS_bteCursor,
                GUIHandler::ID::cbx_GRAPHICS_SETTINGS_fullscreen,
            };
            std::vector<std::string> objKeys = {
                "bteCursor",
                "fullscreen",
            };

            for(unsigned int i = 0; i<objKeys.size(); i++) {

                GUI* possibleCBX = gh->getGUI(BTEObject::GUI_checkbox, objIDs[i]);
                if( possibleCBX!=nullptr && possibleCBX->getType()==BTEObject::Type::GUI_checkbox ) {
                    int cbxState = ((CheckBox*)possibleCBX)->getState();
                    std::string value = "null";
                    switch( cbxState ) {
                        case CheckBox::States::FALSE:{ value = "false"; }break;
                        case CheckBox::States::TRUE:{ value = "true"; }break;
                    }
                    stgs->kv(Settings::TextFiles::options, objKeys[i], value);
                }
            }

            fh->saveSettings(Settings::TextFiles::options);
            ctrls->reloadBindings(fh->getSettings());

            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        } break;

        case GUIHandler::btn_back_to_MAIN:{ gh->setGUIs(GUIHandler::GUIs::MAIN); } break;

        case GUIHandler::btn_OPTIONS_back:{

        } break;

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
            gh->setGUIs(GUIHandler::GUIs::GRAPHICS);
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
            Log::trbshoot(__PRETTY_FUNCTION__, ss.str(), "deleting button");
        } break;
    }
}
ButtonAction::~ButtonAction(){}
