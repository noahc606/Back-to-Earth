#pragma once
#include <vector>
#include "BTEObject.h"
#include "GUI.h"
#include "Window.h"

class GUIHandler : public BTEObject
{
public:
    GUIHandler();
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual ~GUIHandler();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    void passKeyboardInput(std::string text, bool special);
    /**/
    void info(std::stringstream& ss, int& tabs);
    int getGUIActionID();
    Window* getWindow(int id);
    GUI* getGUI(int type, int id);
    /**/
    void resetGUIAction(std::string methodName);
    void addGUI(GUI* gui);
    void setGUIs(int guis);
    void removeGUI(int id);
    void removeGUIs(int idMin, int idMax);
    void removeChildGUIs(int id);
    void removeAllUserGUIs();
    /**/
    enum ID {
        null = -1,

        tb_DEBUG = 0,

        win_MAIN = 100,
        ttp_MAIN_title,
        btn_MAIN_play,
        btn_MAIN_options,
        btn_MAIN_exit,

        win_OPTIONS = 200,
        btn_OPTIONS_controls,
        btn_OPTIONS_gs,
        btn_OPTIONS_back,

            win_CONTROLS = 300,
            btn_CONTROLS_keybind,
            btn_CONTROLS_back,

            win_GRAPHICS_SETTINGS = 300,

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
        MAIN_MENU,
        OPTIONS,
            CONTROLS,

        PAUSE, UNPAUSE,
        WORLD,
            WORLD_characterMenu_open,
            WORLD_characterMenu_close,
    };

protected:
    std::vector<GUI*> guis;

private:
    //Controls::KeyboardInput kbInput;
    //Controls::KeyboardInput lastkbInput;

    std::string kbInput = "";
    bool kbInputSpecial = false;
    std::string lastKBInput = "";
    bool lastKBInputSpecial = false;

    int guiActionID = -1;

    /**/
    /**/
    void removeAllGUIs();
    void removeGUIByIndex(int index);
};
