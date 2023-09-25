#pragma once
#include <string>
#include "Controls.h"
#include "DebugScreen.h"
#include "FileHandler.h"
#include "GUIHandler.h"
#include "SDLHandler.h"
#include "Settings.h"
#include "Tests.h"
#include "World.h"

class BTE
{
public:
    /**/
    BTE();
    void preinit(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    void init();
    virtual ~BTE();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    /**/
    bool isHardTesting();
    std::string getInfo();
    GUIHandler* getGUIHandler();
    /**/

protected:

private:
    /**/
    /**/
    void setGameState(int p_gamestate);
	template<typename T> void unload(T*& t);
	void load(World*& world);
	void load(Tests*& tests);

    /**/

    enum GameState {
        EXIT = -10000,
        TEXTURES = -2,
        TESTING = -1,
        MAIN_MENU,
        WORLD,
    };

    /* MainLoop */
    SDLHandler* sdlHandler;
    FileHandler* fileHandler;
    Controls* controls;
    Settings* settings;

    /* Back to Earth */
    int gamestate = 0;
    GUIHandler guiHandler;
    Commands commands;

    World* world = nullptr;
    bool paused = false;
    std::string lastBTECursorState = "false";

    /* Debugging, testing */
    DebugScreen debugScreen;
    Tests* tests = nullptr;     //Object that tests everything inside of it
    bool alwaysTest = false;
    bool testing = false;       //If true, set gamestate to TESTING (blank screen to draw/tick stuff)
    bool hardTesting = false;   //If true, disable window + loading of all texture/audio assets
};
