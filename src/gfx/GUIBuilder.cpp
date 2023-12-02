#include "GUIBuilder.h"
#include <vector>
#include "Button.h"
#include "CheckBox.h"
#include "GUIAlignable.h"
#include "RadioButton.h"
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
    gh.addGUI(new Tooltip(gh.getWindow(gh.win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, "Back to Earth", gh.ttp_MAIN_title ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Play", gh.btn_MAIN_play ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Options", gh.btn_MAIN_options ));
    gh.addGUI(new Button( gh.getWindow(gh.win_MAIN), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 450, "Exit", gh.btn_MAIN_exit ));
}

void GUIBuilder::buildMainOptions(GUIHandler& gh)
{
    gh.removeGUI(gh.ttp_MAIN_title);
    gh.removeGUI(gh.btn_MAIN_play);
    gh.removeGUI(gh.btn_MAIN_options);
    gh.removeGUI(gh.btn_MAIN_exit);

    gh.removeGUI(gh.win_PAUSED);

    gh.removeGUI(gh.win_CONTROLS);
    gh.removeGUI(gh.win_GRAPHICS_SETTINGS);

    int width = 300;
    Window* w = (Window*)gh.addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Options", "", gh.win_OPTIONS ));
    gh.addGUI(new Button( w, GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Controls", gh.btn_OPTIONS_controls ));
    gh.addGUI(new Button( w, GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, width, "Graphics Settings", gh.btn_OPTIONS_graphics_settings ));
    gh.addGUI(new Button( w, GUIAlignable::CENTER_H, 730, width, "Back", gh.btn_OPTIONS_back ));
}

void GUIBuilder::buildMainControls(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

    int width = 300;
    gh.addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Controls", "", gh.win_CONTROLS ));

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
        GUI* tb = gh.addGUI(new TextBox (w, 528, 82+i*32, 200, gh.tbx_CONTROLS_set_keybind), ctrlIndex);
        ControlBinding newCB(getCtrl(stng));
        ((TextBox*)tb)->setControlBinding(newCB);

        //Add the 'set default' button at the end of the row
        gh.addGUI(new CheckBox(w, 738, 82+i*32, "", CheckBox::RESET, true, gh.cbx_CONTROLS_set_defaults), ctrlIndex);
    }

    gh.addGUI(new Button(w, GUIAlignable::CENTER_H, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildMainGraphics(GUIHandler& gh, FileHandler& fh)
{
    gh.removeGUI(gh.win_OPTIONS);

    int width = 300;
    Window* w = (Window*)gh.addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 800, 800, "Graphics Settings", "", gh.win_GRAPHICS_SETTINGS ));
    Settings* stngs = fh.getSettings();

    gh.addGUI(new CheckBox( w, 40, 120, "Use Different Cursor", stngs->get(Settings::TextFiles::options, "bteCursor"), gh.cbx_GRAPHICS_SETTINGS_bteCursor ));
    gh.addGUI(new CheckBox( w, 40, 160, "Force Fullscreen on Startup", stngs->get(Settings::TextFiles::options, "fullscreen"), gh.cbx_GRAPHICS_SETTINGS_fullscreen ));
    gh.addGUI(new Button( w, GUIAlignable::CENTER_H, 730, width, "Back", gh.btn_back_to_OPTIONS ));
}

void GUIBuilder::buildWorldPause(GUIHandler& gh)
{
	gh.removeGUI(gh.win_OPTIONS);

	gh.addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 600, 500, "Paused", "", gh.win_PAUSED ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Back to Game", gh.btn_PAUSED_back ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Options", gh.btn_PAUSED_options ));
	gh.addGUI(new Button( gh.getWindow(gh.win_PAUSED), GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, 300, "Save & Exit", gh.btn_PAUSED_exit ));
}

void GUIBuilder::buildCharacterMenu(GUIHandler& gh)
{
    if( gh.getGUI(BTEObject::GUI_window, gh.win_CHARACTER)==nullptr ) {
        int w = 18;
        int h = 10;

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

        gh.addGUI(new Window( GUIAlignable::CENTER_H, GUIAlignable::CENTER_V, wd, gh.win_CHARACTER ));
        gh.addGUI(new Tooltip( gh.getWindow(gh.win_CHARACTER), 30, 30, "Character Tabs", gh.ttp_CHARACTER_tabs_desc ) );
        gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 60, "Backpack", true, gh.rbtn_CHARACTER_inventory, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
        gh.addGUI(new RadioButton( gh.getWindow(gh.win_CHARACTER), 30, 100, "Engineering", gh.rbtn_CHARACTER_engineering, gh.rbtn_CHARACTER_tabs_1a, gh.rbtn_CHARACTER_tabs_1b ) );
    }
}

std::string GUIBuilder::getCtrl(std::string s)
{
    return settings->get(Settings::controls, s);
}
