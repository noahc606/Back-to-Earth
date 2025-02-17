#include "GUIBuilder.h"
#include <nch/cpp-utils/log.h>
#include <vector>
#include "Button.h"
#include "CheckBox.h"
#include "ColorSelector.h"
#include "GUIAlignable.h"
#include "Player.h"
#include "ProgressBar.h"
#include "RadioButton.h"
#include "SaveSelector.h"
#include "Slider.h"
#include "TextBox.h"
#include "Tooltip.h"
#include "Window.h"


GUIBuilder::GUIBuilder(){}
GUIBuilder::~GUIBuilder(){}

void GUIBuilder::buildUpdatePrompt(GUIHandler& gh, CurlHandler& cuha, int guiActionID)
{
    //Get the newest version and whether a newer version exists
    std::string newestVersion = "";
    std::string newerVersionExists = cuha.newBTEVersionAvailable(&newestVersion);
    //Is this a natural update (not forced)?
    bool natural = !(guiActionID==GUIHandler::btn_DEBUG_SETTINGS_forceUpdate);
    //Get previous open window (should be win_DEBUG_SETTINGS)
    Window* prevWindow = gh.getWindow(GUIHandler::win_DEBUG_SETTINGS);
    if(prevWindow==nullptr) {
        nch::Log::warnv(__PRETTY_FUNCTION__, "skipping update prompt GUI", "Didn't find debug settings window (are you there?)");
        return;
    }

/* Build the GUIs */
    gh.setWindowActiveState(prevWindow, false);
    Window* win;

    //If there was a curl handler error...
    if(newerVersionExists!="true" && newerVersionExists!="false") {
        win = (Window*)gh.addGUI(new Window(ch, cv, 384, 320, "Network Error", "", gh.win_UPDATE_PROMPT));
        gh.addGUI(new Tooltip(win, ch, 92+32*0, newerVersionExists, gh.ttp_GENERIC ));
        gh.addGUI(new Tooltip(win, ch, 92+32*2, "Check your Internet connection.", gh.ttp_GENERIC ));
        gh.addGUI(new Button(win, ch, 320-38, 320, "Back", gh.btn_UPDATE_PROMPT_not_now));
    //If the curl operation was successful...
    } else {
        //If checking for update and no newer version exists...
        if(natural && newerVersionExists=="false") {
            win = (Window*)gh.addGUI(new Window(ch, cv, 384, 320, "No Update Found", "", gh.win_UPDATE_PROMPT));
            gh.addGUI(new Tooltip(win, ch, 92+32*0, "No new release version was found.", gh.ttp_GENERIC ));
            gh.addGUI(new Button(win, ch, 320-38, 320, "Back", gh.btn_UPDATE_PROMPT_not_now));
        //Use another update method (either newer version detected, or force updating)
        } else {
            //Not force update
            if(natural) { 
                win = (Window*)gh.addGUI(new Window(ch, cv, 704, 448, "New Update Available", "", gh.win_UPDATE_PROMPT));
                gh.addGUI(new Tooltip(win, ch, 92+32*0, "Download it automatically?", gh.ttp_GENERIC ));
            //force update
            } else {
                win = (Window*)gh.addGUI(new Window(ch, cv, 704, 448, "Force Update", "", gh.win_UPDATE_PROMPT));
                gh.addGUI(new Tooltip(win, ch, 92+32*0, "Download all the latest assets?", gh.ttp_GENERIC ));
            }
            
            gh.addGUI(new Tooltip(win, ch, 92+32*1, "You will have to restart after everything has downloaded.", gh.ttp_GENERIC ));
            gh.addGUI(new Button(win, ch, 92+32*3, 320, "Yes", gh.btn_UPDATE_PROMPT_accept));
            gh.addGUI(new Button(win, ch, 92+32*5, 320, "No", gh.btn_UPDATE_PROMPT_not_now));
            gh.addGUI(new Tooltip(win, ch, 92+32*8, "(currently on "+Main::VERSION_LABEL+", latest is "+newestVersion+")", gh.ttp_GENERIC ));
        }
    }



    //Update window
    gh.onWindowUpdate();
}

void GUIBuilder::buildUpdatingScreen(GUIHandler& gh)
{
    Window* updateWindow = gh.getWindow(gh.win_UPDATE_PROMPT);
    if(updateWindow!=nullptr) {
        gh.removeGUI(gh.win_UPDATE_PROMPT);

        Window* win = (Window*)gh.addGUI(new Window(ch, cv, 704, 448, "Updating", "", gh.win_UPDATING_SCREEN));
        gh.addGUI(new Tooltip(win, ch, 92+32*0, "Downloading assets...", gh.ttp_GENERIC ));
        gh.addGUI(new ProgressBar(win, ch, 92+32*3, 640, gh.pbr_UPDATING_SCREEN) );
    } else {

    }

    
    gh.onWindowUpdate();
    //gh.setWindowActiveState(titleWindow, false)
}

void GUIBuilder::buildTitleScreen(GUIHandler& gh)
{
    gh.removeAllUserGUIs();
    gh.addGUI(new Window(gh.win_MAIN));
    gh.addGUI(new Tooltip(gh.getWindow(gh.win_MAIN), ch, cv, "Back to Earth", true, gh.ttp_MAIN_title ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), ch, cv, 450, "Play Campaign", gh.btn_MAIN_play ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), ch, cv, 450, "Options", gh.btn_MAIN_options ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), ch, cv, 450, "Exit", gh.btn_MAIN_exit ));
}

void GUIBuilder::buildMainOptions(GUIHandler& gh)
{
    //Remove all title screen elements
    gh.removeGUI(gh.ttp_MAIN_title);
    gh.removeGUI(gh.btn_MAIN_play);
    gh.removeGUI(gh.btn_MAIN_options);
    gh.removeGUI(gh.btn_MAIN_exit);

    //Remove all other windows
    gh.removeGUI(gh.win_PAUSED);
    gh.removeGUI(gh.win_CONTROLS);
    gh.removeGUI(gh.win_GRAPHICS_SETTINGS);
    gh.removeGUI(gh.win_AUDIO_SETTINGS);
	gh.removeGUI(gh.win_CHARACTER_SETTINGS);
	gh.removeGUI(gh.win_DEBUG_SETTINGS);

    Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Options", "", gh.win_OPTIONS ));
    
    int width = 375;
    int uiY = 1;
    gh.addGUI(new Button( w, ch, 82+uiY*32, width, "Keybinds...", gh.btn_OPTIONS_controls ));

    uiY += 2;
    gh.addGUI(new Button( w, ch, 82+uiY*32, width, "Graphics...", gh.btn_OPTIONS_graphics_settings ));

    uiY += 2;
    gh.addGUI(new Button( w, ch, 82+uiY*32, width, "Audio...", gh.btn_OPTIONS_audio_settings ));
    
    uiY += 2;
    gh.addGUI(new Button( w, ch, 82+uiY*32, width, "Character...", gh.btn_OPTIONS_character_settings ));
    
    uiY += 2;
    gh.addGUI(new Button( w, ch, 82+uiY*32, width, "Debug & Miscellaneous...", gh.btn_OPTIONS_debug_settings ));
    
    
    gh.addGUI(new Button( w, ch, 730, 300, "Back", gh.btn_OPTIONS_back ));
}

void GUIBuilder::buildMainControls(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

    int width = 300;
    gh.addGUI(new Window( ch, cv, 800, 800, "Input Settings", "", gh.win_CONTROLS ));

    Window* w = gh.getWindow(gh.win_CONTROLS);
    Settings* settings = fh.getSettings();


	//Vector of pairs: 1st object = string name, 2nd object = string value
	std::vector<std::pair<std::string, std::string>> ctrls;
	ctrls.push_back( std::make_pair("Navigate to Next UI Element", "NAV_NEXT_UI") );
	ctrls.push_back( std::make_pair("Interact With UI Element (click, toggle, etc)", "INTERACT_UI") );
	ctrls.push_back( std::make_pair("Pause Game", "INGAME_PAUSE") );
	ctrls.push_back( std::make_pair("Modify Map Zoom", "MAP_USE_ZOOM") );
	ctrls.push_back( std::make_pair("Inventory", "PLAYER_OPEN_INVENTORY") );
	ctrls.push_back( std::make_pair("Move Forward", "PLAYER_MOVE_NORTH") );
	ctrls.push_back( std::make_pair("Move Right", "PLAYER_MOVE_EAST") );
	ctrls.push_back( std::make_pair("Move Backward", "PLAYER_MOVE_SOUTH") );
	ctrls.push_back( std::make_pair("Move Left", "PLAYER_MOVE_WEST") );
	ctrls.push_back( std::make_pair("Noclip/Fly", "PLAYER_NOCLIP") );
	ctrls.push_back( std::make_pair("Crouch", "PLAYER_CROUCH") );
	ctrls.push_back( std::make_pair("Sprint", "PLAYER_SPRINT") );
	ctrls.push_back( std::make_pair("Jump", "PLAYER_JUMP") );
	ctrls.push_back( std::make_pair("Switch Hotbar", "HOTBAR_SWITCH") );

	//Add objects from the list above to the window
	for(unsigned int i = 0; i<ctrls.size(); i++) {
        std::pair<std::string, std::string> elem = ctrls.at(i);
        std::string name = elem.first;
        std::string stng = elem.second;
        int ctrlIndex = settings->find( settings->getKvMap(Settings::controls), stng);

        //Add tooltip at the beginning of the row
        gh.addGUI(new Tooltip(w, 30, 92+i*32, name+": ", gh.ttp_CONTROLS_keybind), ctrlIndex);

        //Add input box for setting a new control binding in the middle of the row
        GUI* tb = gh.addGUI(new TextBox(w, 536, 82+i*32, 200, gh.tbx_CONTROLS_set_keybind), ctrlIndex);
        ControlBinding newCB(getCtrl(fh.getSettings(), stng));
        ((TextBox*)tb)->setControlBinding(newCB);

        //Add the 'set default' button at the end of the row
        gh.addGUI(new CheckBox(w, 738, 82+i*32, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), ctrlIndex);
    }

    gh.addGUI(new Button(w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainGraphics(GUIHandler& gh, FileHandler& fh)
{
	gh.removeGUI(gh.win_OPTIONS);

	int width = 300;
	Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Graphics Settings", "", gh.win_GRAPHICS_SETTINGS ));
	Settings* settings = fh.getSettings();
	

    //General
    gh.addGUI(new Tooltip(w, ch, 92, "General", gh.ttp_GENERIC));
        //Max FPS
	    gh.addGUI(new Tooltip( w, 30, 92+32*1, "Maximum FPS:", gh.ttp_GRAPHICS_SETTINGS_maxFps));
	    gh.addGUI(new Slider( w, 406, 82+32*1, 0, 320, settings->get(Settings::TextFiles::options, "maxFps") , gh.sdr_GRAPHICS_SETTINGS_maxFps ) );
	    gh.addGUI(new TextBox(w, 664, 82+32*1, 72, gh.tbx_GRAPHICS_SETTINGS_maxFps ) );
	    gh.addGUI(new CheckBox(w, 738, 82+32*1, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);
        //Different Cursor
	    gh.addGUI(new CheckBox( w, 26, 92+32*2, "Use Different Cursor", settings->get(Settings::TextFiles::options, "bteCursor"), gh.cbx_GRAPHICS_SETTINGS_bteCursor ));
	    //Fullscreen on Startup
        gh.addGUI(new CheckBox( w, 26, 92+32*3, "Force Fullscreen on Startup", settings->get(Settings::TextFiles::options, "fullscreen"), gh.cbx_GRAPHICS_SETTINGS_fullscreen ));
	//World
    //gh.addGUI(new Tooltip(w, ch, 92+32*5, "World", gh.ttp_GENERIC));
        //Sky Quality
        /*
	    gh.addGUI(new Tooltip( w, 30, 92+32*6, "Sky Quality:", gh.ttp_GRAPHICS_SETTINGS_maxRLT));
	    gh.addGUI(new Slider( w, 406, 82+32*6, 0, 20, settings->get(Settings::TextFiles::options, "maxFps") , gh.sdr_GRAPHICS_SETTINGS_maxRLT ) );
	    gh.addGUI(new TextBox(w, 664, 82+32*6, 72, gh.tbx_GRAPHICS_SETTINGS_maxRLT ) );
	    gh.addGUI(new CheckBox(w, 738, 82+32*6, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);
        //Max Region Load Time
	    gh.addGUI(new Tooltip( w, 30, 92+32*7, "Maximum Time to Load Regions (ms):", gh.ttp_GRAPHICS_SETTINGS_maxRLT));
	    gh.addGUI(new Slider( w, 406, 82+32*7, 0, 20, settings->get(Settings::TextFiles::options, "maxFps") , gh.sdr_GRAPHICS_SETTINGS_maxRLT ) );
	    gh.addGUI(new TextBox(w, 664, 82+32*7, 72, gh.tbx_GRAPHICS_SETTINGS_maxRLT ) );
	    gh.addGUI(new CheckBox(w, 738, 82+32*7, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);*/


    //Back button
    gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainAudio(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

	Window* win = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Audio Settings", "", gh.win_AUDIO_SETTINGS ));
	Settings* settings = fh.getSettings();

    //Master volume
    int uiY = 1;
    gh.addGUI(new Tooltip( win, 30, 92+32*uiY, "Master Volume (%):", gh.ttp_GENERIC));
    gh.addGUI(new Slider( win, 406, 82+32*uiY, 0, 100, settings->get(Settings::TextFiles::options, "masterVolume") , gh.sdr_AUDIO_SETTINGS_masterVolume ) );
    gh.addGUI(new TextBox(win, 664, 82+32*uiY, 72, gh.tbx_AUDIO_SETTINGS_masterVolume ) );
    gh.addGUI(new CheckBox(win, 738, 82+32*uiY, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);

    //Music volume
    uiY++; 
    gh.addGUI(new Tooltip( win, 30, 92+32*uiY, "Music Volume (%):", gh.ttp_GENERIC));
    gh.addGUI(new Slider( win, 406, 82+32*uiY, 0, 100, settings->get(Settings::TextFiles::options, "musicVolume") , gh.sdr_AUDIO_SETTINGS_musicVolume ) );
    gh.addGUI(new TextBox(win, 664, 82+32*uiY, 72, gh.tbx_AUDIO_SETTINGS_musicVolume ) );
    gh.addGUI(new CheckBox(win, 738, 82+32*uiY, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);

    //SFX volume
    uiY++;
    gh.addGUI(new Tooltip( win, 30, 92+32*uiY, "SFX Volume (%):", gh.ttp_GENERIC));
    gh.addGUI(new Slider( win, 406, 82+32*uiY, 0, 100, settings->get(Settings::TextFiles::options, "sfxVolume") , gh.sdr_AUDIO_SETTINGS_sfxVolume ) );
    gh.addGUI(new TextBox(win, 664, 82+32*uiY, 72, gh.tbx_AUDIO_SETTINGS_sfxVolume ) );
    gh.addGUI(new CheckBox(win, 738, 82+32*uiY, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);

    //Test
    uiY += 2;
    gh.addGUI(new Button(win, ch, 82+32*uiY, 128, "Test", gh.btn_AUDIO_SETTINGS_test));

    //Back button
	int width = 300;
    gh.addGUI(new Button( win, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainCharacter(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);
	
	std::vector<std::pair<std::string, std::string>> uis;
	uis.push_back( std::make_pair("Hair", "hair") );
	uis.push_back( std::make_pair("Skin", "skin") );
	uis.push_back( std::make_pair("Eyes", "eyes") );
	uis.push_back( std::make_pair("Mouth", "mouth") );
	uis.push_back( std::make_pair("Shirt", "shirt") );
	uis.push_back( std::make_pair("Leggings", "leggings") );
	uis.push_back( std::make_pair("Shoes", "shoes") );
	
	int width = 300;
	Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Character Settings", "", gh.win_CHARACTER_SETTINGS ));
	Settings* settings = fh.getSettings();
	
    gh.addGUI(new Tooltip(w, ch, 82+10, "Set Colors", gh.ttp_GENERIC));
	
    for(unsigned int i = 0; i<uis.size(); i++) {
		std::pair<std::string, std::string> elem = uis.at(i);
		std::string name = elem.first;
		std::string stng = elem.second;
        int ctrlIndex = settings->find( settings->getKvMap(Settings::character), stng);
        
        //Get x and y pos from indexes
        int xPos = 462;
        int yPos = 130+i*32;
        if(i<=3) {
            xPos = 30;
        } else {
            yPos = yPos - 4*32;
        }

        //Get color from setting
        nch::Color color; color.setFromB10Str(settings->get(Settings::character, stng));

        //Add GUIs
		gh.addGUI(new Tooltip(w, xPos, yPos+10, name+":", gh.ttp_CHARACTER_SETTINGS_tooltip), ctrlIndex);
        gh.addGUI(new ColorSelector(w, xPos+96, yPos, color, gh.csr_CHARACTER_SETTINGS_set_val), ctrlIndex);
		TextBox* colorTBX = (TextBox*)gh.addGUI(new TextBox(w, xPos+96+34, yPos, 144, TextBox::FREE_HEX_BASIC, gh.tbx_CHARACTER_SETTINGS_set_val), ctrlIndex);
        colorTBX->setString(color.toStringB16(false));
        colorTBX->setColorInput(true);

		gh.addGUI(new CheckBox(w, xPos+96+180, yPos, "", CheckBox::CBX_RESET, true, gh.cbx_CHARACTER_SETTINGS_set_defaults), ctrlIndex);
	}
	
    //Add back button
	gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainDebug(GUIHandler& gh, FileHandler& fh)
{
    //Remove old windows
    gh.removeGUI(gh.win_DEBUG_SETTINGS);
    gh.removeGUI(gh.win_OPTIONS);
    gh.removeGUI(gh.win_UPDATE_PROMPT);


    //Build list of UIs
	std::vector<std::pair<std::string, std::string>> uis;
	uis.push_back( std::make_pair("Debug Screen Visible [F12]", "debugEnabled") );
	uis.push_back( std::make_pair("Debug Overlays Visible [F12]", "debugOverlayEnabled") );
	uis.push_back( std::make_pair("Logging", "logging") );
	uis.push_back( std::make_pair("Console Commands", "debugHacks") );
	uis.push_back( std::make_pair("Testing on Startup", "debugTesting") );
	uis.push_back( std::make_pair("Show Logo on Startup", "showIntro") );

    //Add window, get settings
	int width = 300;
	Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Debug Settings & Miscellaneous", "", gh.win_DEBUG_SETTINGS ));
	Settings* stngs = fh.getSettings();

    /* Add setting UIs */
    //General
    gh.addGUI(new Tooltip(w, ch, 92, "General", gh.ttp_GENERIC));
        //Debug Screen visible
        gh.addGUI(new CheckBox( w, 26, 92+32*(1), uis[0].first, stngs->get(Settings::options, uis[0].second), gh.cbx_DEBUG_SETTINGS_debugEnabled ), 0);
        //Debug Screen visible
        gh.addGUI(new CheckBox( w, 26, 92+32*(2), uis[1].first, stngs->get(Settings::options, uis[1].second), gh.cbx_DEBUG_SETTINGS_debugOverlayEnabled ), 1);
        //Logging
        gh.addGUI(new CheckBox( w, 26, 92+32*(3), uis[2].first, stngs->get(Settings::options, uis[2].second), gh.cbx_DEBUG_SETTINGS_logging ), 2);
    //Advanced
    gh.addGUI(new Tooltip(w, ch, 92+32*5, "Advanced", gh.ttp_GENERIC));
        //Console Commands
        gh.addGUI(new Tooltip(w, 30, 92+32*(6), uis[3].first+": ", gh.ttp_DEBUG_SETTINGS_debugHacks), 3);
        gh.addGUI(new TextBox(w, 460, 82+32*(6), 300, TextBox::FREE_TEXT, stngs->get(Settings::options, uis[3].second), gh.tbx_DEBUG_SETTINGS_debugHacks), 3);
        //Testing on Startup
        gh.addGUI(new Tooltip(w, 30, 92+32*(7), uis[4].first+": ", gh.ttp_DEBUG_SETTINGS_debugTesting), 4);
        gh.addGUI(new TextBox(w, 460, 82+32*(7), 300, TextBox::FREE_TEXT, stngs->get(Settings::options, uis[4].second), gh.tbx_DEBUG_SETTINGS_debugTesting), 4);
    //About
    gh.addGUI(new Tooltip(w, ch, 92+32*(14), "About", gh.ttp_GENERIC));
        gh.addGUI(new Tooltip(w, 30, 92+32*(15), "\"Back to Earth\" by Out of this World Software", gh.ttp_GENERIC));
        //Startup Intro visible
        gh.addGUI(new CheckBox( w, 26, 82+32*(16), uis[5].first, stngs->get(Settings::options, uis[5].second), gh.cbx_DEBUG_SETTINGS_showIntro ), 5);
        //Check For Updates
        gh.addGUI(new Tooltip(w, 30, 92+32*(17), "Version: "+Main::VERSION_LABEL, gh.ttp_GENERIC));
    	gh.addGUI(new Button( w, 460, 82+32*(17), width, "Check For Updates...", gh.btn_DEBUG_SETTINGS_checkForUpdates ));

        gh.addGUI(new Tooltip(w, 30, 92+32*(18), "Released: "+Main::VERSION_DATE, gh.ttp_GENERIC));
    	gh.addGUI(new Button( w, 460, 82+32*(18), width, "Force Update...", gh.btn_DEBUG_SETTINGS_forceUpdate ));



    //Back button
	gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildWorldPause(GUIHandler& gh)
{
	gh.removeGUI(gh.win_OPTIONS);
    gh.removeGUI(gh.win_PAUSED_howto);

	gh.addGUI(new Window( ch, cv, 600, 500, "Paused", "", gh.win_PAUSED ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Back to Game", gh.btn_PAUSED_back ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Help", gh.btn_PAUSED_howto ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Options", gh.btn_PAUSED_options ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Save & Exit", gh.btn_PAUSED_exit ));
}

void GUIBuilder::buildWorldPauseHowto(GUIHandler& gh)
{
    gh.removeGUI(gh.win_PAUSED);

    Window* w = new Window( ch, cv, 800, 800, "Help", "", gh.win_PAUSED_howto );
    gh.addGUI(w);

    int uiY = 0;
    gh.addGUI(new Tooltip(w, ch, 92+32*uiY, "Gameplay", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use WASD to move forward/left/backward/right from the camera's perspective.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use SPACE to jump or otherwise move up.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use X to sprint or C to sneak.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use LCTRL + scroll to change the camera's zoom.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Move while holding LCTRL to fly/noclip if in Sandbox Mode.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Selected tiles can be placed down with RIGHT MOUSE.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Tiles can be destroyed with LEFT MOUSE.", gh.ttp_GENERIC));
    uiY++;
    uiY += 2;

    gh.addGUI(new Tooltip(w, ch, 92+32*uiY, "Inventory", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use SHIFT to open/close the inventory menu.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use LEFT MOUSE to move an item stack.", gh.ttp_GENERIC));
    uiY++;
    gh.addGUI(new Tooltip(w, 30, 92+32*uiY, "-Use RIGHT MOUSE to select an item slot/stack.", gh.ttp_GENERIC));
    uiY++;
 

    gh.addGUI(new Button( w, ch, 730, 300, "Back", gh.btn_OPTIONS_back ));
}

void GUIBuilder::buildCharacterMenu(GUIHandler& gh, int gamemode)
{
    if( gh.getGUI(BTEObject::GUI_window, gh.win_CHARACTER)==nullptr ) {
        int w = 13;
        int h = 10;

        WindowData* wd = new WindowData(w, h);
        wd->setPanelData(0, "ddddhhhhhhhhw");
        wd->setPanelData(1, "cccciiiiiiiiw");
        wd->setPanelData(2, "cccciiiiiiiiw");
        wd->setPanelData(3, "cccciiiiiiiiw");
        wd->setPanelData(4, "cccciiiiiiiiw");
        wd->setPanelData(5, "cccciiiiiiiiw");
        wd->setPanelData(6, "ddddiiiiiiiiw");
        wd->setPanelData(7, "ddddiiiiiiiiw");
        wd->setPanelData(8, "ddddaaaaaaaaa");
        wd->setPanelData(9, "ddddaaaaaaaaa");

        wd->setPanelColor('d', nch::Color(150, 105, 55, 240) );  //[D]etails about character
        wd->setPanelColor('i', nch::Color(0, 255, 0, 240) );     //[I]nventory
        wd->setPanelColor('h', nch::Color(255, 120, 40, 240) );  //[H]otbar
        wd->setPanelColor('c', nch::Color(0, 0, 0, 240) );       //[C]haracter
        wd->setPanelColor('a', nch::Color(0, 255, 255, 240) );   //[A]ctions from items
        wd->setPanelColor('w', nch::Color(130, 210, 180, 240) ); //[W]orkstation tabs (crafting, assembling, etc.)

        gh.addGUI(new Window( ch, cv, wd, gh.win_CHARACTER ));

        if(gamemode==Player::SURVIVAL) {
            gh.addGUI(new Tooltip( gh.getWindow(gh.win_CHARACTER), 30, 30, "Survival Mode", gh.ttp_CHARACTER_tabs_desc ) );
        } else if(gamemode==Player::HARDCORE) {
            gh.addGUI(new Tooltip( gh.getWindow(gh.win_CHARACTER), 30, 30, "Hardcore Mode", gh.ttp_CHARACTER_tabs_desc ) );
        } else {
            gh.addGUI(new Tooltip( gh.getWindow(gh.win_CHARACTER), 30, 30, "Sandbox Mode", gh.ttp_CHARACTER_tabs_desc ) );
        }
    }
}

void GUIBuilder::buildColorSelector(GUIHandler& gh, Window* parentWindow, int extraID)
{
    gh.setWindowActiveState(parentWindow, false);

    WindowData* wd = new WindowData(11, 8, "Color Selection", "");
    wd->setPanelData(0, "xxxxxxxxxxx");
    wd->setPanelData(1, "ooooooooooo");
    wd->setPanelData(2, "ooooooooooo");
    wd->setPanelData(3, "ooooooooooo");
    wd->setPanelData(4, "ooooooooooo");
    wd->setPanelData(5, "ooooooooooo");
    wd->setPanelData(6, "ooooooooooo");
    wd->setPanelData(7, "ooooooooooo");
    wd->setPanelData(8, "xxxxxxxxxxx");
    wd->setPanelColor('x', nch::Color(0, 0, 200, 240) );
    wd->setPanelColor('o', nch::Color(150, 105, 55, 240) );

    int width = 240;
    Window* win = new Window(ch, cv, wd, GUIHandler::win_COLORSELECTOR);
    Button* btn = new Button( win, ch, wd->getHeight()*64+32-70, width, "Set", gh.btn_COLORSELECTOR_back);
    gh.addGUI(new Button( win, ch, wd->getHeight()*64+32-70, width, "Cancel", gh.btn_COLORSELECTOR_back));
    ColorSelector* csr = (ColorSelector*)gh.getGUI(BTEObject::GUI_colorselect, GUIHandler::csr_CHARACTER_SETTINGS_set_val, extraID);


    TextBox* tbx = (TextBox*)gh.getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_CHARACTER_SETTINGS_set_val, extraID);
    nch::Color c;
    c.setFromB16Str(tbx->getString()+"00");
    auto hsv = c.getHSV();
    csr->setHue(hsv[0]);
    csr->setSat(hsv[1]);
    csr->setVal(hsv[2]);


    wd->setSpecialType(WindowData::COLOR_SELECTOR, csr);
    gh.addGUI(win, extraID);
    gh.addGUI(btn, extraID);

    //Hue
    int dx = 332;
    int dyf = 54;
    std::vector<std::string> groups = {"H:", "S:", "V:", "R:", "G:", "B:"};
    for(int i = 1; i<=6; i++) {
        
        switch(i) {
            case 1: {
                gh.addGUI(new Tooltip(win, 384, 64+32*i, "Hue-Saturation-Value", gh.ttp_GENERIC));
            } break;
            case 2: {

            } break;
            case 3: {

            } break;
            case 4: {
                gh.addGUI(new Tooltip(win, 384, 64+32*i, "Red-Green-Blue", gh.ttp_GENERIC));
            } break;
            case 5: {
            } break;
            case 6: {
            } break;
        }
        if(i==4) dyf += 64;
        
        gh.addGUI(new Tooltip(win, dx-24, 92+32*i+dyf, groups[i-1], gh.ttp_GENERIC));
        Slider* sdr0 = (Slider*)gh.addGUI(new Slider(win, dx, 82+32*i+dyf, 0, 360, "0", gh.sdr_COLORSELECTOR_set_hue), extraID);
        sdr0->setNumSpaces(128);
        gh.addGUI(new TextBox(win, dx+258, 82+32*i+dyf, 72, gh.tbx_GRAPHICS_SETTINGS_maxFps ) );
        gh.addGUI(new CheckBox(win, dx+332, 82+32*i+dyf, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);

        switch(i) {
            case 1: {
                std::stringstream ss; ss << csr->getHue();
                sdr0->setSelectorVal(ss.str());
            } break;
            case 2: {
                std::stringstream ss; ss << csr->getSat();
                sdr0->setSelectorVal(ss.str());
            } break;
            case 3: {
                std::stringstream ss; ss << csr->getVal();
                sdr0->setSelectorVal(ss.str());
            } break;
        }
    }

    gh.onWindowUpdate();
}

void GUIBuilder::buildSelectCampaign(GUIHandler& gh, FileHandler& fh)
{
    //Remove all unnecessary elements
    gh.removeGUI(gh.ttp_MAIN_title);
    gh.removeGUI(gh.btn_MAIN_play);
    gh.removeGUI(gh.btn_MAIN_options);
    gh.removeGUI(gh.btn_MAIN_exit);
    gh.removeGUI(gh.win_SELECT_CAMPAIGN_CN);


    WindowData* wd = new WindowData(12, 12, "Select Campaign", "");
    wd->setPanelData(0, "tttttttttttt");
    wd->setPanelData(1, "ssssssssssss");
    wd->setPanelData(2, "ssssssssssss");
    wd->setPanelData(3, "ssssssssssss");
    wd->setPanelData(4, "ssssssssssss");
    wd->setPanelData(5, "ssssssssssss");
    wd->setPanelData(6, "ssssssssssss");
    wd->setPanelData(7, "ssssssssssss");
    wd->setPanelData(8, "ssssssssssss");
    wd->setPanelData(9, "ssssssssssss");
    wd->setPanelData(10,"aaaaaaaaaaaa");
    wd->setPanelData(11,"bbbbbbbbbbbb");
    wd->setPanelColor('t', nch::Color(64, 64, 64, 240) );
    wd->setPanelColor('s', nch::Color(0, 0, 255, 160) );
    wd->setPanelColor('a', nch::Color(96, 128, 240, 240) );
    wd->setPanelColor('b', nch::Color(64, 64, 64, 240) );

    Window* w = new Window( ch, cv, wd, gh.win_SELECT_CAMPAIGN );
    gh.addGUI(w);

    auto saveList = fh.listSubDirs("saved/games");
    int index = 0;
    for(std::string s : saveList) {
        std::string saveDir = s;
        if(saveDir.substr(0, 12).compare("saved/games/")==0 || saveDir.substr(0, 12).compare("saved\\games\\")==0) {
            saveDir = saveDir.substr(12);
        }

        nch::Log::log("Loaded save dir \"%s\"", saveDir.c_str());

        //Get world size description
        std::string sizeDesc = fh.getReadableMemorySize(fh.dirDiskSpaceUsed(s));
        
        //Get world name
        auto dataFile = fh.readTxtFileKVs("saved/games/"+saveDir+"/data.txt");
        std::string worldName = Settings::get(dataFile, "worldName");

        gh.addGUI(  new SaveSelector(w, index, saveDir, worldName, sizeDesc, gh.ssr_SELECT_CAMPAIGN_select )  );

        index++;
    }

    gh.addGUI(new Button( w, 26, wd->getH64()-38-66, 200, "Create New...", gh.btn_SELECT_CAMPAIGN_createNew ));
    gh.addGUI(new Button( w, ch, wd->getH64()-38-66, 300, "Open Save Directory", gh.btn_SELECT_CAMPAIGN_openSaveDirectory ));
    gh.addGUI(new Button( w, ch, wd->getH64()-38, 300, "Back", gh.btn_SELECT_CAMPAIGN_back ));
}

void GUIBuilder::buildSelectCampaignCN(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_SELECT_CAMPAIGN);

    WindowData* wd = new WindowData(12, 12, "Create New Campaign", "");
    wd->setPanelData(0, "tttttttttttt");
    wd->setPanelData(1, "ssssssssssss");
    wd->setPanelData(2, "ssssssssssss");
    wd->setPanelData(3, "ssssssssssss");
    wd->setPanelData(4, "ssssssssssss");
    wd->setPanelData(5, "ssssssssssss");
    wd->setPanelData(6, "ssssssssssss");
    wd->setPanelData(7, "ssssssssssss");
    wd->setPanelData(8, "ssssssssssss");
    wd->setPanelData(9, "ssssssssssss");
    wd->setPanelData(10,"aaaaaaaaaaaa");
    wd->setPanelData(11,"bbbbbbbbbbbb");
    wd->setPanelColor('t', nch::Color(64, 64, 64, 240) );
    wd->setPanelColor('s', nch::Color(0, 0, 255, 160) );
    wd->setPanelColor('a', nch::Color(96, 128, 240, 240) );
    wd->setPanelColor('b', nch::Color(64, 64, 64, 240) );

    Window* w = new Window( ch, cv, wd, gh.win_SELECT_CAMPAIGN_CN );
    gh.addGUI(w);

    //Save file name input
    int height = 0;
    gh.addGUI(new Tooltip( w, ch, 92+32*height, "General", gh.ttp_GENERIC), 0);
    
    height += 1;
    gh.addGUI(new Tooltip(w, 30, 92+32*height, "Save File Name: ", gh.ttp_GENERIC), 0);
    gh.addGUI(new TextBox(w, 26+32*6, 82+32*height, 32*16, TextBox::LEVELNAME_TEXT, gh.tbx_SELECT_CAMPAIGN_CN_levelName ), 0);
    
    //Seed input
    height += 1;
    gh.addGUI(new Tooltip(w, 30, 92+32*height, "Seed: ", gh.ttp_GENERIC), 0);
    
    int64_t randomSeed = ( ((int64_t)rand())<<32)+(int64_t)rand();
    TextBox* seedTbx = new TextBox(w, 26+32*6, 82+32*height, 32*16, TextBox::FREE_TEXT, gh.tbx_SELECT_CAMPAIGN_CN_worldSeed );
    gh.addGUI(seedTbx, 0);
    std::stringstream seedSS; seedSS << randomSeed;
    seedTbx->setString(seedSS.str());

    //Game mode selection
    height += 1;
    int ghGM0 = gh.rbtn_SELECT_CAMPAIGN_CN_gameMode0;
    int ghGM2 = gh.rbtn_SELECT_CAMPAIGN_CN_gameMode2;
    gh.addGUI(new Tooltip(      w, 30,          92+32*height, "Game Mode: ", gh.ttp_GENERIC));
    gh.addGUI(new RadioButton(  w, 26+32*6,     82+32*height, "Sandbox", gh.rbtn_SELECT_CAMPAIGN_CN_gameMode0, ghGM0, ghGM2));
    gh.addGUI(new RadioButton(  w, 26+32*11,    82+32*height, "Survival", true, gh.rbtn_SELECT_CAMPAIGN_CN_gameMode1, ghGM0, ghGM2));
    gh.addGUI(new RadioButton(  w, 26+32*16,    82+32*height, "Hardcore", gh.rbtn_SELECT_CAMPAIGN_CN_gameMode2, ghGM0, ghGM2));

    height += 2;
    //gh.addGUI(new Button( w, ch, 82+32*height, 300, "Show Additional Options...", gh.btn_SELECT_CAMPAIGN_CN_showAdditional));

    gh.addGUI(new Button( w, 26, wd->getH64()-38-66, 200, "Create", gh.btn_SELECT_CAMPAIGN_CN_mkdir ));
    //gh.addGUI(new Button( w, ch, wd->getH64()-38-66, 300, "Restore Defaults", gh.btn_SELECT_CAMPAIGN_CN_defaults ));

    gh.addGUI(new Button( w, ch, wd->getH64()-38, 300, "Back", gh.btn_SELECT_CAMPAIGN_CN_back ));
}

void GUIBuilder::campaignNewShowMore(GUIHandler& gh, FileHandler& fh)
{
    Window* w = gh.getWindow(gh.win_SELECT_CAMPAIGN_CN);
    if(w==nullptr) {
        nch::Log::warn(__PRETTY_FUNCTION__, "Couldn't fild campaignNew window.");
        return;
    }

    int height = 7;
    height += 2;
    gh.addGUI(new Tooltip( w, ch, 92+32*height, "Starting Planet", gh.ttp_GENERIC), 0);
    height += 1;
    gh.addGUI(new CheckBox(w, 26+32*0, 82+32*height, "In Habitable Zone", CheckBox::CBX_TRUE, false, gh.cbx_SELECT_CAMPAIGN_CN_sp), 0);
    height += 1;
    gh.addGUI(new CheckBox(w, 26+32*0, 82+32*height, "Default Terrain Parameters", CheckBox::CBX_TRUE, false, gh.cbx_SELECT_CAMPAIGN_CN_sp), 1);
    height += 1;
    gh.addGUI(new CheckBox(w, 26+32*0, 82+32*height, "Default Mineral Distribution", CheckBox::CBX_TRUE, false, gh.cbx_SELECT_CAMPAIGN_CN_sp), 2);
    height += 1;
    gh.addGUI(new CheckBox(w, 26+32*0, 82+32*height, "Atmosphere Breathable", CheckBox::CBX_FALSE, false, gh.cbx_SELECT_CAMPAIGN_CN_sp), 3);
    height += 1;
    gh.addGUI(new CheckBox(w, 26+32*0, 82+32*height, "Default Colors", CheckBox::CBX_TRUE, false, gh.cbx_SELECT_CAMPAIGN_CN_sp), 4);
}

void GUIBuilder::campaignNewShowLess(GUIHandler& gh, FileHandler& fh)
{

}

std::string GUIBuilder::getCtrl(Settings* stngs, std::string s)
{
    return stngs->get(Settings::controls, s);
}
