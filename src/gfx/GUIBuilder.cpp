#include "GUIBuilder.h"
#include <vector>
#include "Button.h"
#include "CheckBox.h"
#include "ColorSelector.h"
#include "GUIAlignable.h"
#include "Log.h"
#include "RadioButton.h"
#include "SaveSelector.h"
#include "Slider.h"
#include "TextBox.h"
#include "Tooltip.h"
#include "Window.h"


GUIBuilder::GUIBuilder(){}
GUIBuilder::~GUIBuilder(){}

void getSetting();

void GUIBuilder::buildTitleScreen(GUIHandler& gh)
{
    gh.removeAllUserGUIs();
    gh.addGUI(new Window(gh.win_MAIN));
    gh.addGUI(new Tooltip(gh.getWindow(gh.win_MAIN), ch, cv, "Back to Earth", gh.ttp_MAIN_title ));
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
	gh.removeGUI(gh.win_CHARACTER_SETTINGS);

    int width = 300;
    Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Options", "", gh.win_OPTIONS ));
    gh.addGUI(new Button( w, ch, cv, width, "Controls...", gh.btn_OPTIONS_controls ));
    gh.addGUI(new Button( w, ch, cv, width, "Graphics...", gh.btn_OPTIONS_graphics_settings ));
    gh.addGUI(new Button( w, ch, cv, width, "Character...", gh.btn_OPTIONS_character_settings ));
    gh.addGUI(new Button( w, ch, cv, width, "Debug...", gh.btn_OPTIONS_debug_settings ));
    gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_OPTIONS_back ));
}

void GUIBuilder::buildMainControls(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

    int width = 300;
    gh.addGUI(new Window( ch, cv, 800, 800, "Controls Settings", "", gh.win_CONTROLS ));

    Window* w = gh.getWindow(gh.win_CONTROLS);
    settings = fh.getSettings();


	//Vector of pairs: 1st object = string name, 2nd object = string value
	std::vector<std::pair<std::string, std::string>> ctrls;
	ctrls.push_back( std::make_pair("Navigate to Next UI Element", "NAV_NEXT_UI") );
	ctrls.push_back( std::make_pair("Interact With UI Element (click, toggle, etc)", "INTERACT_UI") );
	ctrls.push_back( std::make_pair("Pause Game", "INGAME_PAUSE") );
	ctrls.push_back( std::make_pair("Modify Map Zoom", "MAP_USE_ZOOM") );
	ctrls.push_back( std::make_pair("Character Menu", "PLAYER_INVENTORY") );
	ctrls.push_back( std::make_pair("Move North", "PLAYER_MOVE_NORTH") );
	ctrls.push_back( std::make_pair("Move East", "PLAYER_MOVE_EAST") );
	ctrls.push_back( std::make_pair("Move South", "PLAYER_MOVE_SOUTH") );
	ctrls.push_back( std::make_pair("Move West", "PLAYER_MOVE_WEST") );
	ctrls.push_back( std::make_pair("Move Up", "PLAYER_MOVE_UP") );
	ctrls.push_back( std::make_pair("Move Down", "PLAYER_MOVE_DOWN") );
	ctrls.push_back( std::make_pair("Crouch", "PLAYER_CROUCH") );
	ctrls.push_back( std::make_pair("Sprint", "PLAYER_SPRINT") );
	ctrls.push_back( std::make_pair("Jump", "PLAYER_JUMP") );
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
        ControlBinding newCB(getCtrl(stng));
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
		
	//Max FPS
	gh.addGUI(new Tooltip( w, 30, 92, "Maximum FPS:", gh.ttp_GRAPHICS_SETTINGS_maxFps));
	gh.addGUI(new Slider( w, 406, 82, 0, 320, settings->get(Settings::TextFiles::options, "maxFps") , gh.sdr_GRAPHICS_SETTINGS_maxFps ) );
	gh.addGUI(new TextBox(w, 664, 82, 72, gh.tbx_GRAPHICS_SETTINGS_maxFps ) );
	gh.addGUI(new CheckBox(w, 738, 82, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);

	
	gh.addGUI(new CheckBox( w, 26, 92+32*1, "Use Different Cursor", settings->get(Settings::TextFiles::options, "bteCursor"), gh.cbx_GRAPHICS_SETTINGS_bteCursor ));
	gh.addGUI(new CheckBox( w, 26, 92+32*2, "Force Fullscreen on Startup", settings->get(Settings::TextFiles::options, "fullscreen"), gh.cbx_GRAPHICS_SETTINGS_fullscreen ));
	gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainCharacter(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);
	
	std::vector<std::pair<std::string, std::string>> uis;
	uis.push_back( std::make_pair("Hair Color", "hair") );
	uis.push_back( std::make_pair("Skin Color", "skin") );
	uis.push_back( std::make_pair("Eye Color", "eyes") );
	uis.push_back( std::make_pair("Mouth Color", "mouth") );
	uis.push_back( std::make_pair("Shirt Color", "shirt") );
	uis.push_back( std::make_pair("Leggings Color", "leggings") );
	uis.push_back( std::make_pair("Shoes Color", "shoes") );
	
	int width = 300;
	Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Character Settings", "", gh.win_CHARACTER_SETTINGS ));
	Settings* settings = fh.getSettings();
	
	for(unsigned int i = 0; i<uis.size(); i++) {
		std::pair<std::string, std::string> elem = uis.at(i);
		std::string name = elem.first;
		std::string stng = elem.second;
        int stngIndex = settings->find( settings->getKvMap(Settings::character), stng);
		
        //Add tooltip at the beginning of the row
		gh.addGUI(new Tooltip(w, 30, 92+i*32, name+":", gh.ttp_CHARACTER_SETTINGS_tooltip), stngIndex);
		
        Color color; color.setFromDecimalStr(settings->get(Settings::character, stng));
        gh.addGUI(new ColorSelector(w, 334, 82+i*32, color, gh.csr_CHARACTER_SETTINGS_set_val));

		gh.addGUI(new TextBox(w, 592, 82+i*32, 144, TextBox::FREE_NUMBERS_BASIC, gh.tbx_CHARACTER_SETTINGS_set_val ) );
		gh.addGUI(new CheckBox(w, 738, 82+i*32, "", CheckBox::CBX_RESET, true, gh.cbx_CONTROLS_set_defaults), 123456);
	}
	
	gh.addGUI(new Button( w, ch, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainDebug(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

	std::vector<std::pair<std::string, std::string>> uis;
	uis.push_back( std::make_pair("Debug Screen", "debugEnabled") );
	uis.push_back( std::make_pair("Advanced Debug Screen", "debugHacks") );
	uis.push_back( std::make_pair("Logging", "logging") );

	int width = 300;
	Window* w = (Window*)gh.addGUI(new Window( ch, cv, 800, 800, "Debug Settings", "", gh.win_DEBUG_SETTINGS ));
	Settings* settings = fh.getSettings();
}

void GUIBuilder::buildWorldPause(GUIHandler& gh)
{
    

	gh.removeGUI(gh.win_OPTIONS);

	gh.addGUI(new Window( ch, cv, 600, 500, "Paused", "", gh.win_PAUSED ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Back to Game", gh.btn_PAUSED_back ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Options", gh.btn_PAUSED_options ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), ch, cv, 300, "Save & Exit", gh.btn_PAUSED_exit ));
}

void GUIBuilder::buildCharacterMenu(GUIHandler& gh)
{
    if( gh.getGUI(BTEObject::GUI_window, gh.win_CHARACTER)==nullptr ) {
        int w = 18;
        int h = 11;

        /*
            's': Scroll through buttons representing other menu options
            'd': Brief Character description. Can be clicked on to view even more info
            'c': Character menu which is dynamic
            'x': Character graphic. Translucent background
            'e': Essential info. 3 graphics which represent Health, nutrition, oxygen + buttons
                 to view specifics of these 3 graphics. More on the right half.
            'b': Bottom tab for extra pages (next/prev)

        */
        WindowData* wd = new WindowData(w, h);
        wd->setPanelData(0, "ssssssddddcccccccc");
        wd->setPanelData(1, "ssssssxxxxcccccccc");
        wd->setPanelData(2, "ssssssxxxxcccccccc");
        wd->setPanelData(3, "ssssssxxxxcccccccc");
        wd->setPanelData(4, "ssssssxxxxcccccccc");
        wd->setPanelData(5, "ssssssxxxxcccccccc");
        wd->setPanelData(6, "ssssssddddcccccccc");
        wd->setPanelData(7, "ssssssddddcccccccc");
        wd->setPanelData(8, "eeeeeeeeeeeeeeeeee");
        wd->setPanelData(9, "eeeeeeeeeeeeeeeeee");
        wd->setPanelData(10,"bbbbbbbbbbbbbbbbbb");

        wd->setPanelColor('s', Color(0, 0, 200, 240) );
        wd->setPanelColor('d', Color(150, 105, 55, 240) );
        wd->setPanelColor('c', Color(0, 255, 0, 240) );
        wd->setPanelColor('x', Color(0, 0, 0, 240) );
        wd->setPanelColor('e', Color(100, 0, 0, 240) );
        wd->setPanelColor('b', Color(130, 210, 180, 240) );

        gh.addGUI(new Window( ch, cv, wd, gh.win_CHARACTER ));
        gh.addGUI(new Tooltip( gh.getWindow(gh.win_CHARACTER), 30, 30, "Sandbox Mode", gh.ttp_CHARACTER_tabs_desc ) );
        gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 60, "Backpack", true, gh.rbtn_CHARACTER_inventory, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
        gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 100, "Engineering", gh.rbtn_CHARACTER_engineering, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
    }
}

void GUIBuilder::buildColorSelector(GUIHandler& gh)
{
    int w = 8; int h = 6;
    WindowData* wd = new WindowData(w, h, "Color Selection", "");
    wd->setPanelData(0, "xxxxxxxxxx");
    wd->setPanelData(1, "oooooooooo");
    wd->setPanelData(2, "oooooooooo");
    wd->setPanelData(3, "oooooooooo");
    wd->setPanelData(4, "oooooooooo");
    wd->setPanelData(5, "oooooooooo");
    wd->setPanelData(6, "xxxxxxxxxx");
    wd->setPanelColor('x', Color(0, 0, 200, 240) );
    wd->setPanelColor('o', Color(150, 105, 55, 240) );

    int width = 300;
    Window* win = new Window(ch, cv, wd, GUIHandler::win_COLORSELECTOR);
    Button* btn = new Button( win, ch, wd->getHeight()*64+32-70, width, "Back", gh.btn_COLORSELECTOR_back );
    wd->setSpecialType( WindowData::COLOR_SELECTOR, gh.getGUI(BTEObject::GUI_colorselect, GUIHandler::csr_CHARACTER_SETTINGS_set_val) );

    gh.addGUI(win);
    gh.addGUI(btn);
    gh.addGUI(new Slider(win, 30, 92, 0, 360, "150", GUIHandler::sdr_COLORSELECTOR_set_hue));

    gh.onWindowUpdate();
}

void GUIBuilder::buildSelectCampaign(GUIHandler& gh, FileHandler& fh)
{
    //Remove all title screen elements
    gh.removeGUI(gh.ttp_MAIN_title);
    gh.removeGUI(gh.btn_MAIN_play);
    gh.removeGUI(gh.btn_MAIN_options);
    gh.removeGUI(gh.btn_MAIN_exit);

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
    wd->setPanelColor('t', Color(64, 64, 64, 240) );
    wd->setPanelColor('s', Color(0, 0, 255, 160) );
    wd->setPanelColor('a', Color(96, 128, 240, 240) );
    wd->setPanelColor('b', Color(64, 64, 64, 240) );

    int width = 300;
    Window* w = new Window( ch, cv, wd, gh.win_SELECT_CAMPAIGN );
    gh.addGUI(w);

    auto saveList = fh.listSubDirs("saved/games");
    int index = 0;
    for(std::string s : saveList) {
        std::string saveDir = s;
        if(saveDir.substr(0, 12).compare("saved/games/")==0) {
            saveDir = saveDir.substr(12);
        }

        Log::log("Loaded save dir \"%s\"", saveDir.c_str());

        std::string sizeDesc = fh.getReadableMemorySize(fh.dirDiskSpaceUsed(s));
        gh.addGUI(  new SaveSelector(w, index, saveDir, sizeDesc, gh.ssr_SELECT_CAMPAIGN_select )  );

        index++;
    }

    gh.addGUI(new Button( w, ch, wd->getH64()-38, width, "Back", gh.btn_SELECT_CAMPAIGN_back ));

    

    //gh.addGUI(new Tooltip( gh.getWindow(gh.win_SELECT_CAMPAIGN), 30, 30, "Sandbox Mode", gh.ttp_CHARACTER_tabs_desc ) );
    //gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 60, "Backpack", true, gh.rbtn_CHARACTER_inventory, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
    //gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 100, "Engineering", gh.rbtn_CHARACTER_engineering, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
}

std::string GUIBuilder::getCtrl(std::string s)
{
    return settings->get(Settings::controls, s);
}
