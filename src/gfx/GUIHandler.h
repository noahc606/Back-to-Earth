#pragma once
#include <vector>
#include "BTEObject.h"
#include "GUI.h"
#include "Window.h"

class GUIHandler : public BTEObject
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
    void info(std::stringstream& ss, int& tabs);
    int getGUIActionID();
    Window* getWindow(int id);
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
    /**/
    enum ID {
        null = -1,

        tbx_DEBUG = 0,

        win_MAIN = 100,
        ttp_MAIN_title,
        btn_MAIN_play,
        btn_MAIN_options,
        btn_MAIN_exit,
        btn_back_to_MAIN,

        win_OPTIONS = 200,
        btn_OPTIONS_controls,
        btn_OPTIONS_graphics_settings,
        btn_OPTIONS_back,
        btn_back_to_OPTIONS,

            win_CONTROLS = 300,
            ttp_CONTROLS_keybind,
            tbx_CONTROLS_set_keybind,
            cbx_CONTROLS_set_defaults,

            win_GRAPHICS_SETTINGS = 400,
            cbx_GRAPHICS_SETTINGS_bteCursor,
            cbx_GRAPHICS_SETTINGS_fullscreen,

        win_PAUSED = 1000,
        btn_PAUSED_back,
        btn_PAUSED_options,
        btn_PAUSED_exit,

        win_CHARACTER = 2000,
            rbtn_CHARACTER_tabs_1a,
                rbtn_CHARACTER_inventory,
                rbtn_CHARACTER_engineering,
            rbtn_CHARACTER_tabs_1b,

            ttp_CHARACTER_tabs_desc,
    };

    enum GUIs {
        MAIN,
        OPTIONS,
            CONTROLS,
            GRAPHICS,

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

    /**/
    /**/
    void removeAllGUIs();
    void removeGUIByIndex(int index);
};
