#include "ButtonAction.h"
#include <nch/cpp-utils/io/Log.h>
#include "CheckBox.h"
#include "ColorSelector.h"
#include "FileHandler.h"
#include "MainLoop.h"
#include "TextBox.h"

ButtonAction::ButtonAction(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
    AudioLoader* al = sh->getAudioLoader();
    Settings* stgs = fh->getSettings();
    int gaid = gh->getGUIActionID();

    switch(gaid)
    {
        /** Back-to buttons */
        case GUIHandler::btn_back_to_OPTIONS: {
            saveSettingsBasedOnUIs(al, gh, stgs, fh, ctrls);
            gh->setGUIs(GUIHandler::OPTIONS);
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

        /** Update prompt buttons */
        case GUIHandler::btn_UPDATE_PROMPT_not_now: {   gh->setGUIs(GUIHandler::GUIs::DEBUG); } break;
        case GUIHandler::btn_UPDATE_PROMPT_dont_ask: {
            gh->setGUIs(GUIHandler::GUIs::MAIN_MENU);
            stgs->kv(Settings::options, "checkForUpdates", "false");
            fh->saveSettings(Settings::options);
        } break;

        /** Main menu buttons */
        case GUIHandler::btn_MAIN_exit:     { MainLoop::quit(); } break;
        case GUIHandler::btn_MAIN_options:  { gh->setGUIs(GUIHandler::GUIs::OPTIONS); } break;

        /** Options buttons */
        case GUIHandler::btn_OPTIONS_controls:              { gh->setGUIs(gh->CONTROLS); } break;
        case GUIHandler::btn_OPTIONS_graphics_settings:     { gh->setGUIs(gh->GRAPHICS); } break;
        case GUIHandler::btn_OPTIONS_audio_settings:        { gh->setGUIs(gh->AUDIO); } break;
		case GUIHandler::btn_OPTIONS_character_settings:    { gh->setGUIs(gh->CHARACTER); } break;
		case GUIHandler::btn_OPTIONS_debug_settings:        { gh->setGUIs(gh->DEBUG); } break;

        case GUIHandler::btn_AUDIO_SETTINGS_test: { saveSettingsBasedOnUIs(al, gh, stgs, fh, ctrls); } break;

        /** Campaign Selection buttons */
        case GUIHandler::btn_SELECT_CAMPAIGN_createNew:     { gh->setGUIs(gh->SELECT_CAMPAIGN_createNew); } break;
        case GUIHandler::btn_SELECT_CAMPAIGN_openSaveDirectory: { 
            fh->openUserLocalURL("saved/games/");
        } break;
        
        
        case GUIHandler::btn_SELECT_CAMPAIGN_CN_back:       { gh->setGUIs(gh->SELECT_CAMPAIGN); } break;

        /** Pause menu buttons */
        case GUIHandler::btn_PAUSED_howto:   { gh->setGUIs(GUIHandler::GUIs::PAUSE_HOWTO); } break;
        case GUIHandler::btn_PAUSED_options: {  gh->setGUIs(GUIHandler::GUIs::OPTIONS); } break;
		

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
            //std::stringstream ss;
            //ss << "Clicked button with unset action ID: " << gh->getGUIActionID();
            //Log::warnv(__PRETTY_FUNCTION__, ss.str(), "deleting button");
        } break;
    }

    al->play(al->SFX_TITLE_button);
}
ButtonAction::~ButtonAction(){}

void ButtonAction::populateSettingUIInfo(GUIHandler* gh, int& currentWindowID, int& settingFileID, std::vector<std::pair<int, int>>& objIDs, std::vector<std::string>& objKeys)
{
    //List of possible setting window IDs
    std::vector<int> settingWindowIDs = {
        gh->ID::win_GRAPHICS_SETTINGS,
        gh->ID::win_AUDIO_SETTINGS,
        gh->ID::win_CHARACTER_SETTINGS,
        gh->ID::win_CONTROLS,
        gh->ID::win_DEBUG_SETTINGS,
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
            objIDs = {
                std::make_pair(GUIHandler::tbx_GRAPHICS_SETTINGS_maxFps, -1),
                std::make_pair(GUIHandler::cbx_GRAPHICS_SETTINGS_bteCursor, -1),
                std::make_pair(GUIHandler::cbx_GRAPHICS_SETTINGS_fullscreen, -1),
            };
            objKeys = { "maxFps", "bteCursor", "fullscreen", };
            settingFileID = Settings::TextFiles::options;
        } break;
        case gh->ID::win_AUDIO_SETTINGS: {
            objIDs = {
                std::make_pair(GUIHandler::tbx_AUDIO_SETTINGS_masterVolume, -1),
                std::make_pair(GUIHandler::tbx_AUDIO_SETTINGS_musicVolume, -1),
                std::make_pair(GUIHandler::tbx_AUDIO_SETTINGS_sfxVolume, -1),
            };
            objKeys = { "masterVolume", "musicVolume", "sfxVolume", };
            settingFileID = Settings::TextFiles::options;
        } break;

        case gh->ID::win_CHARACTER_SETTINGS: {
            //Set
            objIDs = {
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 0),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 1),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 2),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 3),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 4),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 5),
                std::make_pair(GUIHandler::tbx_CHARACTER_SETTINGS_set_val, 6),
            };
            objKeys = { "hair", "skin", "eyes", "mouth", "shirt", "leggings", "shoes" };
            settingFileID = Settings::TextFiles::character;
        } break;
        case gh->ID::win_CONTROLS: {
            objIDs = {};
            objKeys = {};
            settingFileID = Settings::TextFiles::controls;
        } break;
        case gh->ID::win_DEBUG_SETTINGS: {
            objIDs = {
                std::make_pair(GUIHandler::cbx_DEBUG_SETTINGS_debugEnabled, 0),
                std::make_pair(GUIHandler::cbx_DEBUG_SETTINGS_debugOverlayEnabled, 1),
                std::make_pair(GUIHandler::cbx_DEBUG_SETTINGS_logging, 2),
                std::make_pair(GUIHandler::tbx_DEBUG_SETTINGS_debugHacks, 3),
                std::make_pair(GUIHandler::tbx_DEBUG_SETTINGS_debugTesting, 4),
                std::make_pair(GUIHandler::cbx_DEBUG_SETTINGS_showIntro, 5),
            };
            objKeys = { "debugEnabled", "debugOverlayEnabled", "logging", "debugHacks", "debugTesting", "showIntro" };
            settingFileID = Settings::TextFiles::options;
        } break;
        default: {
            nch::Log::error(__PRETTY_FUNCTION__, "Couldn't find any setting-holding windows");
            return;
        } break;
    }
}

void ButtonAction::saveSettingsBasedOnUIs(AudioLoader* al, GUIHandler* gh, Settings* stgs, FileHandler* fh, Controls* ctrls)
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
                nch::Color thisCol;
                thisCol.setFromB16Str(tbx->getString()+"FF");

                value = thisCol.toStringB10();
            //If inputType is something else
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
    if(currentWindowID==gh->ID::win_AUDIO_SETTINGS) {
        al->setMasterVolumeFactor   ( stgs->getNum(settingFileID, "masterVolume")/100.0 );
        al->setMusicVolumeFactor    ( stgs->getNum(settingFileID, "musicVolume")/100.0 );
        al->setSfxVolumeFactor      ( stgs->getNum(settingFileID, "sfxVolume")/100.0 );
    }
}