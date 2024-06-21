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
    void drawCursor();
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

    bool updateBTEApp();
    void setGameState(int p_gamestate, std::string extraInfo);
    void setGameState(int p_gamestate);
	void unload(World*& world);
	void unload(Tests*& tests);
	void load(World*& world, std::string dirName);
	void load(Tests*& tests);
    void performGUIAction(int guiActionID);


    enum GameState {
        EXIT = -10000,
        TEXTURES = -2,
        TESTING = -1,
        MAIN_MENU,
        SELECT_CAMPAIGN,
        WORLD,
        UPDATING,
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
	bool playedImpact = false;

    /* Debugging, testing */
    DebugScreen debugScreen;
    Tests* tests = nullptr;     //Object that tests everything inside of it
    int updateProgress = 0;
    int updateNumFiles = 1;
    bool alwaysTest = 0;
    bool forceDisableUpdateDLs = false;
    bool testing = 0;          //If true, set gamestate to TESTING (blank screen to draw/tick stuff)
    bool hardTesting = 0;      //If true, disable window + loading of all texture/audio assets
};
