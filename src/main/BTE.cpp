#include "BTE.h"
#include <sstream>
#include <type_traits>
#include "ButtonAction.h"
#include "Log.h"
#include "MainLoop.h"
#include "Tests.h"
#include "Text.h"
#include "TextBox.h"

/**/

BTE::BTE(){}
void BTE::preinit(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    //MainLoop
    sdlHandler = sh;
    fileHandler = fh;
    controls = ctrls;
    settings = fileHandler->getSettings();
	
	
    //Settings
    if( settings->get(Settings::options, "fullscreen")=="true" ) {
        sdlHandler->toggleFullScreen();
    }

    if( settings->get(Settings::options, "bteCursor")=="true" ) {
        sdlHandler->toggleBTECursor();
        lastBTECursorState = "true";
    }

    if( settings->get(Settings::options, "debugHacks")=="joseph is lame" ) {
        debugScreen.setHaxEnabled(true);
    }

    //Testing
    if( alwaysTest || settings->get(Settings::options, "debugTesting")=="true" ) {
        testing = true;
    }
    if( alwaysTest || settings->get(Settings::options, "debugHardTesting")=="true" ) {
        hardTesting = true;
    }
    if( testing||hardTesting ) {
        setGameState(GameState::TESTING);
    }
}
void BTE::init()
{
    //GUIHandler
    guiHandler.init(sdlHandler, fileHandler, controls, testing);

    //Init debugScreen
    debugScreen.init(sdlHandler, &guiHandler, controls);

}

BTE::~BTE()
{
    unload(world);
    unload(tests);
}

/**/

void BTE::draw()
{
    //Gamestate specific objects
    switch(gamestate) {

        case TESTING:
        case TEXTURES: {
            if( tests!=nullptr )
                tests->draw();
        } break;

        case GameState::WORLD: {
            if( world!=nullptr ) {
                world->draw();
			}
        } break;
    }

    //GUI handler and debug screen exists for all gamestates
    guiHandler.draw();
    debugScreen.draw();

    if( sdlHandler->usingBTECursor() ) {

        SDL_Rect dst;
        dst.x = controls->getMouseX()/2*2-2; dst.y = controls->getMouseY()/2*2-2; dst.w = 24; dst.h = 24;
        SDL_Rect src;
        src.x = 0; src.y = 0; src.w = 12; src.h = 12;

        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
            src.y = 36;
        }

        if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
            src.y = 24;
        }

        sdlHandler->renderCopy( TextureLoader::GUI_cursor, &src, &dst );
    }
}

void BTE::tick()
{
    std::string bteCursorState = fileHandler->getSettings()->get(Settings::TextFiles::options, "bteCursor");
    if( lastBTECursorState!=bteCursorState ) {
        lastBTECursorState = bteCursorState;
        sdlHandler->toggleBTECursor();
    }

    /** Gamestate specific objects */
    switch(gamestate) {

        case TESTING:
        case TEXTURES: {
            if( tests!=nullptr ) {
                tests->tick();
            }
        } break;

        case GameState::WORLD: {
            if( world!=nullptr ) {
                //Toggle between pause and unpause
                if( controls->isPressed("INGAME_PAUSE") ) {
                    if( paused ) {
                        paused = false;
                        guiHandler.setGUIs( GUIHandler::GUIs::UNPAUSE );
                    } else {
                        paused = true;
                        guiHandler.setGUIs( GUIHandler::GUIs::PAUSE );
                    }
                    controls->stopPress("INGAME_PAUSE", __PRETTY_FUNCTION__);
                }

                //Tick world
                world->tick(paused, guiHandler);
            }
        } break;
    }

    /** GUIHandler/DebugScreen */
    if( guiHandler.exists() ) {
        /** GUI handler */
        //Tick
        guiHandler.tick();

        //GUI Button Action
        if( guiHandler.getGUIActionID()>-1 ) {
            ButtonAction ba(sdlHandler, &guiHandler, fileHandler, controls);
            //Game state switching through buttons

            switch( guiHandler.getGUIActionID() ) {
                /** Main Menu */
                case GUIHandler::btn_MAIN_play:
                    setGameState(GameState::WORLD);
                break;
                case GUIHandler::btn_MAIN_exit:
                    setGameState(GameState::EXIT);
                break;

                /** Options menu buttons */
                case GUIHandler::btn_OPTIONS_back:
                    if( gamestate==GameState::WORLD ) {
                        guiHandler.setGUIs(GUIHandler::GUIs::PAUSE);
                    } else {
                        guiHandler.setGUIs(GUIHandler::GUIs::MAIN);
                    }
                break;

                /** Pause Menu */
                case GUIHandler::btn_PAUSED_back:
                    paused = false;
                    guiHandler.setGUIs(GUIHandler::GUIs::UNPAUSE);
                break;
                case GUIHandler::btn_PAUSED_exit:
                    setGameState(GameState::MAIN_MENU);
                    paused = false;
                break;
            }

            //Reset gui action
            guiHandler.resetGUIAction(__PRETTY_FUNCTION__);
        }

        /** Debug screen */
        // Set debug screen to be invisible if it is disabled
        if( settings->get(Settings::options, "debugEnabled")!="true" ) {
            debugScreen.setVisible(false);
        } else {
            //Tick
            debugScreen.tick();
            // Update the string as long as it is supposed to be visible
            if(debugScreen.getVisible()) {
                debugScreen.debugSetString(getInfo());
            }
        }
    }

    /** Commands */
    if( Commands::cmdIntResult("gs")!=nullptr ) {
        setGameState(*Commands::cmdIntResult("gs"));
        Commands::resetCMDEntered(__PRETTY_FUNCTION__);
    }

    /** Commands */
    if( Commands::cmdIntResult("debug")!=nullptr ) {
        std::string s = *Commands::cmdStringResult("debug.type");
        if( s=="memTexes" ) {
            if( Commands::cmdIntResult("debug.memTexesID")!=nullptr ) {
                int id = *Commands::cmdIntResult("debug.memTexesID");
                debugScreen.debugDrawTexTemporarily(id);
            }
        }
        Commands::resetCMDEntered(__PRETTY_FUNCTION__);
    }
}

void BTE::onWindowUpdate() { guiHandler.onWindowUpdate(); }

bool BTE::isHardTesting() { return hardTesting; }

std::string BTE::getInfo()
{
    std::stringstream ss;
    int tabs = 0;

    //System time
    ss << MainLoop::getSystemTime();

    //MainLoop
    DebugScreen::newGroup(ss, tabs, "MainLoop");
        DebugScreen::indentLine(ss, tabs);
        ss << "FPS=" << MainLoop::getCurrentFPS() << ", TPS=" << MainLoop::getCurrentTPS() << "; ";
        ss << "(mspf, mspt)=(" << MainLoop::getCurrentMSPF() << ", " << MainLoop::getCurrentMSPT() << "); ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //SDLHandler
    DebugScreen::newGroup(ss, tabs, "SDLHandler");
        DebugScreen::indentLine(ss, tabs);
        ss << sdlHandler->getVideoDriversDesc();
        DebugScreen::newLine(ss);

        DebugScreen::indentLine(ss, tabs);
        ss << "Window(w, h)=(" << sdlHandler->getWidth() << ", " << sdlHandler->getHeight() << "); ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //Controls
    DebugScreen::newGroup(ss, tabs, "Controls");
        DebugScreen::indentLine(ss, tabs);
        ss << "Mouse(x, y)=(" << controls->getMouseX() << ", " << controls->getMouseY() << "); ";
        ss << "Mouse Wheel(y)=" << controls->getMouseWheel() << "; ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //[BTE]
    DebugScreen::newGroup(ss, tabs, "BTE");
        DebugScreen::newGroup(ss, tabs, "BTE::guiHandler");
            guiHandler.info(ss, tabs);
        DebugScreen::endGroup(tabs);

        if( world!=nullptr ) {
            DebugScreen::newGroup(ss, tabs, "BTE::world");
                world->info(ss, tabs);
            DebugScreen::endGroup(tabs);
        }

        if( tests!=nullptr ) {
            DebugScreen::newGroup(ss, tabs, "BTE::tests");
                tests->info(ss, tabs);
            DebugScreen::endGroup(tabs);
        }

    DebugScreen::endGroup(tabs);


    //GUIHandler

    //World
    return ss.str();
}

GUIHandler* BTE::getGUIHandler()
{
    return &guiHandler;
}

void BTE::setGameState(int p_gamestate)
{
    if( gamestate!=p_gamestate ) {
        unload(world);
        unload(tests);
    }

    //Gamestate switch
    gamestate = p_gamestate;

    //Gamestate switch message
    std::stringstream ss;
    ss << "Switching to gamestate " << p_gamestate << "...";
    Log::log(ss.str());

    //Validate gamestate switch
    switch(p_gamestate) {
        case EXIT: {
            MainLoop::quit();
        } break;

        case TEXTURES: {
            guiHandler.removeAllUserGUIs();
        } break;
        case TESTING: {
            guiHandler.removeAllUserGUIs();
            load(tests);
        } break;
        case MAIN_MENU: {
            guiHandler.setGUIs(GUIHandler::GUIs::MAIN);
        } break;
        case WORLD: {
            guiHandler.setGUIs(GUIHandler::GUIs::WORLD);
            load(world);
        } break;

        default: {
            Log::warn(__PRETTY_FUNCTION__, "Tried to switch to invalid gamestate", "going to TESTING (-1)");
            setGameState(TESTING);
        } break;
    }
}

void BTE::unload(World*& world)
{
	if(world!=nullptr) {
		delete world;
		world = nullptr;
	}
}

void BTE::unload(Tests*& tests)
{
	if(tests!=nullptr) {
		delete tests;
		tests = nullptr;
	}
}

void BTE::load(World*& world)
{
	unload(world);
	world = new World();
	world->init(sdlHandler, &guiHandler, fileHandler, controls);
}

void BTE::load(Tests*& tests)
{
	unload(tests);
	tests = new Tests();
	tests->init(sdlHandler, fileHandler, controls);
}