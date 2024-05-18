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
            saveSettingsBasedOnUIs(gh, stgs, fh, ctrls);
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

void ButtonAction::populateSettingUIInfo(GUIHandler* gh, int& currentWindowID, int& settingFileID, std::vector<std::pair<int, int>>& objIDs, std::vector<std::string>& objKeys)
{
    //List of possible setting window IDs
    std::vector<int> settingWindowIDs = {
        gh->ID::win_GRAPHICS_SETTINGS,
        gh->ID::win_CHARACTER_SETTINGS,
        gh->ID::win_CONTROLS,
    };

    //Set currentWindowID to whichever setting window we are on, if any (if none, currentWindowID remains what it was).
    for(int i = 0; i<settingWindowIDs.size(); i++) {
        //If we found a window with ID settingWindowIDs[i]
        if( gh->getWindow(settingWindowIDs[i])!=nullptr ) {
            //Set currentWindowID and break out of for
            currentWindowID = settingWindowIDs[i];
            break;
        }
    }

    switch(currentWindowID) {
        case gh->ID::win_GRAPHICS_SETTINGS: {
            //Set
            objIDs = {
                std::make_pair(GUIHandler::ID::tbx_GRAPHICS_SETTINGS_maxFps, -1),
                std::make_pair(GUIHandler::ID::cbx_GRAPHICS_SETTINGS_bteCursor, -1),
                std::make_pair(GUIHandler::ID::cbx_GRAPHICS_SETTINGS_fullscreen, -1),
            };
            objKeys = { "maxFps", "bteCursor", "fullscreen", };
            settingFileID = Settings::TextFiles::options;
        } break;
        case gh->ID::win_CHARACTER_SETTINGS: {
            //Set
            objIDs = {
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 0),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 1),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 2),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 3),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 4),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 5),
                std::make_pair(GUIHandler::ID::tbx_CHARACTER_SETTINGS_set_val, 6),
            };
            objKeys = { "hair", "skin", "eyes", "mouth", "shirt", "leggings", "shoes" };
            settingFileID = Settings::TextFiles::character;
        } break;
        case gh->ID::win_CONTROLS: {
            objIDs = {};
            objKeys = {};
            settingFileID = Settings::TextFiles::controls;
        } break;
        default:{
            Log::error(__PRETTY_FUNCTION__, "Couldn't find any setting-holding windows");
            return;
        } break;
    }
}

void ButtonAction::saveSettingsBasedOnUIs(GUIHandler* gh, Settings* stgs, FileHandler* fh, Controls* ctrls)
{
    //Set currentWindowID, settingFileID, objIDs and objKeys.
    int currentWindowID = -1000;
    int settingFileID = -1000;
    std::vector<std::pair<int, int>> objIDs;  //Pair: <ID, extraID>.
    std::vector<std::string> objKeys;
    populateSettingUIInfo(gh, currentWindowID, settingFileID, objIDs, objKeys);

    //Iterate through all specified objs that have the specified objKeys
    for(unsigned int i = 0; i<objKeys.size(); i++) {
        //Get possible UIs corresponding to these IDs
        GUI* possibleTBX = gh->getGUI(BTEObject::GUI_textbox, objIDs[i].first, objIDs[i].second);
        GUI* possibleCBX = gh->getGUI(BTEObject::GUI_checkbox, objIDs[i].first, objIDs[i].second);
        
        //If we found a checkbox
        if( possibleCBX!=nullptr ) {
            int cbxState = ((CheckBox*)possibleCBX)->getState();
            std::string value = "null";
            if( cbxState==CheckBox::States::CBX_FALSE ) { value = "false"; }
            if( cbxState==CheckBox::States::CBX_TRUE ) { value = "true"; }

            stgs->kv(settingFileID, objKeys[i], value);
        }
        
        //If we found a textbox
        if( possibleTBX!=nullptr ) {
            TextBox* tbx = (TextBox*)possibleTBX;
            std::string value = "null";
            //If inputType is hex digits
            if(tbx->getInputType()==tbx->FREE_HEX_BASIC) {
                try {
                    int number = (int)strtol(tbx->getString().c_str(), NULL, 16);
                    std::stringstream ss; ss << number; value = ss.str();
                } catch(...) {
                    Log::warnv(__PRETTY_FUNCTION__, "using \"null\"", "Failed to convert hex string \"%s\" to a base 10 integer string.", tbx->getString().c_str());
                }
                
            //If inputType is anything else
            } else {
                value = tbx->getString();
            }

            stgs->kv(settingFileID, objKeys[i], value);
        }
    }

    fh->saveSettings(settingFileID);
    ctrls->reloadBindings(fh->getSettings());
    
    //Special settings which should be set manually
    if(currentWindowID==gh->ID::win_GRAPHICS_SETTINGS) {
        std::string maxFps = stgs->get(settingFileID, "maxFps");
        MainLoop::setMaxFPS(maxFps);
    }


    gh->setGUIs(GUIHandler::GUIs::OPTIONS);
}