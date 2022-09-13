#include "BTE.h"
#include <sstream>
#include "ButtonAction.h"
#include "Log.h"
#include "MainLoop.h"
#include "Tests.h"
#include "Text.h"
#include "TextBox.h"

/**/

BTE::BTE(){}
void BTE::init(SDLHandler* p_sdlHandler, FileHandler* p_fileHandler, Controls* p_controls)
{
    //MainLoop
    sdlHandler = p_sdlHandler;
    fileHandler = p_fileHandler;
    controls = p_controls;
    settings = fileHandler->getSettings();

    //GUIHandler
    guiHandler.init(sdlHandler, fileHandler, controls);

    //Init debugScreen
    debugScreen.init(sdlHandler, controls);

    //If we need to toggle fullscreen upon startup
    if( settings->get(Settings::options, "fullscreen")=="true" ) {
        sdlHandler->toggleFullScreen();
    }

    setGameState(GameState::TESTING);
    //setGameState(GameState::MAIN_MENU);

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
            if( world!=nullptr )
                world->draw();
        } break;
    }

    //GUI handler and debug screen exists for all gamestates
    guiHandler.draw();
    debugScreen.draw();
}

void BTE::tick()
{
    //Gamestate specific objects
    switch(gamestate) {

        case TESTING:
        case TEXTURES: {
            if( tests!=nullptr )
                tests->tick();
        } break;

        case GameState::WORLD: {
            if( world!=nullptr ) {
                world->tick();
            }
        } break;
    }

    //GUI handler and debug screen exists for all gamestates
    if( guiHandler.exists() ) {
        /** GUI handler */
        //Tick
        guiHandler.tick();
        //GUI Action
        if( guiHandler.getGUIActionID()>-1 ) {
            ButtonAction ba(sdlHandler, &guiHandler);

            //Game state switching through buttons
            if( guiHandler.getGUIActionID()==GUIHandler::btn_MAIN_play ) {
                setGameState(GameState::WORLD);
            }
            if( guiHandler.getGUIActionID()==GUIHandler::btn_MAIN_exit ) {
                setGameState(GameState::EXIT);
            }

            //Reset gui action
            guiHandler.resetGUIAction(__PRETTY_FUNCTION__);
        }

        /** Debug screen */
        //Tick
        debugScreen.tick(&guiHandler);
        //Update the string as long as it is supposed to be visible
        if(debugScreen.getVisible()) {
            debugScreen.setDebugString(getInfo());
        }
    }

    /** Commands */
    if( Commands::cmdIntResult("gs")!=nullptr ) {
        setGameState(*Commands::cmdIntResult("gs"));
        Commands::resetCMDEntered(__PRETTY_FUNCTION__);
    }
}

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
        ss << "Window(w, h)=(" << sdlHandler->getWidth() << ", " << sdlHandler->getHeight() << "); ";
        DebugScreen::newLine(ss);
    DebugScreen::endGroup(tabs);

    //Controls
    DebugScreen::newGroup(ss, tabs, "Controls");
        DebugScreen::indentLine(ss, tabs);
        ss << "Mouse(x, y)=(" << controls->getMouseX() << ", " << controls->getMouseY() << "); ";
        ss << "Last Mouse Action(press, release)=" << "(" << "null" << ", " << "null" << "); ";
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

void BTE::onWindowUpdate()
{
    guiHandler.onWindowUpdate();
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
            guiHandler.setGUIs(GUIHandler::GUIs::MAIN_MENU);
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


void BTE::unload(World*& w)
{
    if( w!=nullptr ) {
        delete w;
        w = nullptr;
    }
}

void BTE::load(World*& w)
{
    unload(w);
    w = new World();
    (w)->init(sdlHandler, controls);
}

void BTE::unload(Tests*& t)
{
    if( t!=nullptr ) {
        delete t;
        t = nullptr;
    }
}

void BTE::load(Tests*& t)
{
    unload(t);
    t = new Tests();
    t->init(sdlHandler, fileHandler, controls);
}

/**/


