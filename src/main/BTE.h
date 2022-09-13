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
    void init(SDLHandler* p_sdlHandler, FileHandler* p_fileHandler, Controls* p_controls);
    virtual ~BTE();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    /**/
    std::string getInfo();
    GUIHandler* getGUIHandler();
    /**/

protected:

private:
    /**/
    /**/
    void setGameState(int p_gamestate);
    void unload(World*& w);
    void unload(Tests*& t);
    void load(World*& w);
    void load(Tests*& t);
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
    int gamestate = TESTING;
    GUIHandler guiHandler;
    Commands commands;
    World* world = nullptr;

    /* Debugging, testing */
    DebugScreen debugScreen;
    Tests* tests = nullptr;
};
