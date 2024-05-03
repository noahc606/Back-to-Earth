#pragma once
#include <vector>
#include "BTEObject.h"
#include "GUI.h"
#include "Loggable.h"
#include "Window.h"

class GUIHandler : public BTEObject, public Loggable
{
public:
    GUIHandler();
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls, bool testing);
    virtual ~GUIHandler();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    void passSpecialInput(ControlBinding& cbsi);
    /**/
    void putInfo(std::stringstream& ss, int& tabs);
    int getGUIActionID();
    std::string getGUIActionData();
    Window* getWindow(int id);
    GUI* getGUI(int type, int id, int extraId);
    GUI* getGUI(int type, int id);
    /**/
    void resetGUIAction(std::string methodName);
    GUI* addGUI(GUI* gui, int extraID);
    GUI* addGUI(GUI* gui);
    void setGUIs(int guis);
    void resetGUIs(int extraID);
    void removeGUI(int id);
    void removeGUIs(int idMin, int idMax);
    void removeChildGUIs(int id);
    void removeAllUserGUIs();
    /* Individual GUIs */
    enum ID {
        /* UIs with specific functions */
        null = -1,
        tbx_DEBUG = 0,
        win_COLORSELECTOR = 1,
            sdr_COLORSELECTOR_set_hue,
            btn_COLORSELECTOR_back,

        /* Titlescreen */
        win_MAIN,
        ttp_MAIN_title,
        btn_MAIN_play,
        btn_MAIN_options,
        btn_MAIN_exit,

        /* Select Campaign */
        win_SELECT_CAMPAIGN,
            ssr_SELECT_CAMPAIGN_select,
            btn_SELECT_CAMPAIGN_back,

        /* Main Options */
        win_OPTIONS,
        btn_OPTIONS_controls,
        btn_OPTIONS_graphics_settings,
		btn_OPTIONS_character_settings,
		btn_OPTIONS_debug_settings,
        btn_OPTIONS_back,
        btn_back_to_OPTIONS,

            // Main Options -> Controls
            win_CONTROLS,
            ttp_CONTROLS_keybind,
            tbx_CONTROLS_set_keybind,
            cbx_CONTROLS_set_defaults,

            // Main Options -> Graphics Settings
            win_GRAPHICS_SETTINGS,
			ttp_GRAPHICS_SETTINGS_maxFps,
			sdr_GRAPHICS_SETTINGS_maxFps,
			tbx_GRAPHICS_SETTINGS_maxFps,
            cbx_GRAPHICS_SETTINGS_bteCursor,
            cbx_GRAPHICS_SETTINGS_fullscreen,
			
            // Main Options -> Character Settings
			win_CHARACTER_SETTINGS,
			ttp_CHARACTER_SETTINGS_tooltip,
			csr_CHARACTER_SETTINGS_set_val,
			tbx_CHARACTER_SETTINGS_set_val,

            // Main Options -> Debug Settings
            win_DEBUG_SETTINGS,

        /* Pause Screen */
        win_PAUSED,
        btn_PAUSED_back,
        btn_PAUSED_options,
        btn_PAUSED_exit,

        /* Character */
        win_CHARACTER,

            // Character -> Inventory
            rbtn_CHARACTER_tabs_1a,
                rbtn_CHARACTER_inventory,
                rbtn_CHARACTER_engineering,
            rbtn_CHARACTER_tabs_1b,

            // Character -> Inventory -> tab tooltip descriptions
            ttp_CHARACTER_tabs_desc,

            // Character -> Inventory -> item tooltip, button, textboxes
            ttp_CHARACTER_item,
            btn_CHARACTER_item,
    		tbx_CHARACTER_item,
    };

    /* Groups of GUIs */
    enum GUIs {
        MAIN,
        OPTIONS,
            CONTROLS,
            GRAPHICS,
			CHARACTER,

        SELECT_CAMPAIGN,

        PAUSE, UNPAUSE,
        WORLD,
            WORLD_characterMenu_open,
            WORLD_characterMenu_close,
    };

protected:
    std::vector<GUI*> guis;

private:
    ControlBinding cbSpecialInput;
    int guiActionID = -1;
    std::string guiActionData = "";

    /**/
    /**/
    void removeAllGUIs();
    void removeGUIByIndex(int index);
};
