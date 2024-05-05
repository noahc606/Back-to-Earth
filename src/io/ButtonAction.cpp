#include "ButtonAction.h"
#include "CheckBox.h"
#include "ColorSelector.h"
#include "FileHandler.h"
#include "Log.h"
#include "MainLoop.h"
#include "TextBox.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
    Settings* stgs = fh->getSettings();

    switch( gh->getGUIActionID() )
    {
        /** Back-to buttons */
        case GUIHandler::btn_back_to_OPTIONS: {
            int objIDs[] = {
				GUIHandler::ID::tbx_GRAPHICS_SETTINGS_maxFps,
                GUIHandler::ID::cbx_GRAPHICS_SETTINGS_bteCursor,
                GUIHandler::ID::cbx_GRAPHICS_SETTINGS_fullscreen,
            };
            std::vector<std::string> objKeys = {
				"maxFps",
                "bteCursor",
                "fullscreen",
            };

            for(unsigned int i = 0; i<objKeys.size(); i++) {
				
				GUI* possibleTBX = gh->getGUI(BTEObject::GUI_textbox, objIDs[i]);
                GUI* possibleCBX = gh->getGUI(BTEObject::GUI_checkbox, objIDs[i]);
				
				
                if( possibleCBX!=nullptr ) {
                    int cbxState = ((CheckBox*)possibleCBX)->getState();
                    std::string value = "null";
                    if( cbxState==CheckBox::States::CBX_FALSE ) { value = "false"; }
                    if( cbxState==CheckBox::States::CBX_TRUE ) { value = "true"; }

                    stgs->kv(Settings::TextFiles::options, objKeys[i], value);
                }
				
				if( possibleTBX!=nullptr ) {
					std::string tbxVal = ((TextBox*)possibleTBX)->getString();
					
					stgs->kv(Settings::TextFiles::options, objKeys[i], tbxVal);
				}
            }

            fh->saveSettings(Settings::TextFiles::options);
            ctrls->reloadBindings(fh->getSettings());
			
			std::string maxFps = stgs->get(Settings::TextFiles::options, "maxFps");
			MainLoop::setMaxFPS(maxFps);

            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        } break;
        case GUIHandler::btn_COLORSELECTOR_back: {
            //Get extra id of the colorselector window
            GUI* gui1 = gh->getGUI(BTEObject::GUI_window, gh->win_COLORSELECTOR);
            if(gui1!=nullptr) {
                int eid = gui1->getExtraID();

                //Deselect color selector button
                GUI* gui2 = gh->getGUI(BTEObject::GUI_colorselect, gh->csr_CHARACTER_SETTINGS_set_val, eid);
                if(gui2!=nullptr) {
                    ((ColorSelector*)gui2)->deselect();
                }
            }

            //Remove color selector window
            gh->removeGUI(gh->win_COLORSELECTOR);
            gh->setAllWindowsActiveState(true);
        } break;

        //case GUIHandler::btn_back_to_MAIN:{ gh->setGUIs(GUIHandler::GUIs::MAIN); } break;

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
		case GUIHandler::btn_OPTIONS_character_settings: {
			gh->setGUIs(GUIHandler::GUIs::CHARACTER);
		} break;

        /** Pause menu buttons */
        case GUIHandler::btn_PAUSED_options: {
            gh->setGUIs(GUIHandler::GUIs::OPTIONS);
        } break;

        /** Graphics settings buttons */
		
		/** World buttons */
		case GUIHandler::btn_CHARACTER_item: {
			
			std::vector<std::string> strs;
			for( int i = 0; i<4; i++ ) {
				GUI* gui = gh->getGUI( BTEObject::GUI_textbox, GUIHandler::tbx_CHARACTER_item, 1000+i );
				if( gui!=nullptr ) {
					TextBox* tbx = (TextBox*)gui;
					strs.push_back( tbx->getString() );
				}
			}
			
			int x = 0;
			int y = 0;
			int z = 0;
			try {
				x = std::stoi(strs[0]);
				y = std::stoi(strs[1]);
				z = std::stoi(strs[2]);
			} catch(...) {}
			
			std::stringstream ss;
			ss << "tele " << x << " " << y << " " << z;
			Commands::executeCMD(ss.str());
		} break;
		
        /** Default */
        default: {
            std::stringstream ss;
            ss << "Clicked button with unset action ID: " << gh->getGUIActionID();
            Log::trbshoot(__PRETTY_FUNCTION__, ss.str(), "deleting button");
        } break;
    }
}
ButtonAction::~ButtonAction(){}
