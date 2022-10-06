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
        btn_OPTIONS_gs,
        btn_OPTIONS_back,

        win_PAUSED = 300,
        btn_PAUSED_back,
        btn_PAUSED_options,
        btn_PAUSED_exit,
    };

    enum GUIs {
        MAIN_MENU,
        OPTIONS,
        PAUSE, UNPAUSE,
        WORLD,
    };

protected:

private:
    //Controls::KeyboardInput kbInput;
    //Controls::KeyboardInput lastkbInput;

    std::string kbInput = "";
    bool kbInputSpecial = false;
    std::string lastKBInput = "";
    bool lastKBInputSpecial = false;

    std::vector<GUI*> guis;

    int guiActionID = -1;

    /**/
    /**/
    void removeAllGUIs();
    void removeGUIByIndex(int index);
    void alignWindows();
    void alignWindowComponents(int alignment);
};
