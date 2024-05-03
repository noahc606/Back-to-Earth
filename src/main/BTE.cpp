#include "BTE.h"
#include <sstream>
#include <type_traits>
#include "ButtonAction.h"
#include "CurlHandler.h"
#include "Log.h"
#include "MainLoop.h"
#include "Tests.h"
#include "TextOld.h"
#include "TextBox.h"

/**/

BTE::BTE(){}
void BTE::preinit(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	/* Init handlers/tools */
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;
	settings = fileHandler->getSettings();
	
	
	/* Configure app to match saved settings */
	if( settings->get(Settings::options, "fullscreen")=="true" ) {	// Fullscreen?
		sdlHandler->toggleFullScreen();
	}
	MainLoop::setMaxFPS( settings->get(Settings::options, "maxFps") );	// Max FPS?
	if( settings->get(Settings::options, "bteCursor")=="true" ) {	// Custom cursor?
		sdlHandler->toggleBTECursor();
		lastBTECursorState = "true";
	}
	if( settings->get(Settings::options, "debugHacks")=="joseph is lame" ) {	// Debugging tools?
		debugScreen.setHaxEnabled(true);
	}
	if( alwaysTest || settings->get(Settings::options, "debugTesting")=="true" ) {	// Testing?
		testing = true;
	}
	if( alwaysTest || settings->get(Settings::options, "debugHardTesting")=="true" ) {	// Hard testing?
		hardTesting = true;
	}
	if( hardTesting ) {
		setGameState(GameState::TESTING);
	}
}
void BTE::init()
{
	Commands::cKV("size", 64);
	
	//GUIHandler
	guiHandler.init(sdlHandler, fileHandler, controls, testing);

	//Init debugScreen
	debugScreen.init(sdlHandler, &guiHandler, controls);

	if( !(testing) ) {
		setGameState(GameState::MAIN_MENU);
	} else {
		setGameState(GameState::TESTING);
	}
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
			if( world==nullptr ) break;
			
			world->draw( debugScreen.getVisible() );
		} break;
	}

	//GUI handler and debug screen exists for all gamestates
	guiHandler.draw();
	
	if(world!=nullptr) {
		Player* localPlayer = world->getLocalPlayer();
		PlayerMenu* localPlayerMenu = world->getLocalPlayerMenu();
		if( localPlayer!=nullptr ) {
			//Draw player within menu if player's menu is open
			if( localPlayerMenu->getState()>0 ) {
				localPlayer->drawCharInMenu();
			}
			
			//Draw specific menu elements
			localPlayerMenu->draw();
		}
	}
	
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

	/** GUIHandler/DebugScreen */
	if( guiHandler.exists() ) {
		/** GUI handler */
		//Tick
		guiHandler.tick();

		//GUI Button Action
		int gaid = guiHandler.getGUIActionID();
		if( gaid>-1 ) {
			ButtonAction ba(sdlHandler, &guiHandler, fileHandler, controls);
			//Game state switching through buttons
			switch( gaid ) {
				/** Main Menu */
				case GUIHandler::btn_MAIN_play: { setGameState(GameState::SELECT_CAMPAIGN); } break;
				case GUIHandler::btn_MAIN_exit: { setGameState(GameState::EXIT); } break;

				/** Campaign select menu */
				case GUIHandler::ssr_SELECT_CAMPAIGN_select: {
					std::string selectedWorld = guiHandler.getGUIActionData();
					Log::log("Opening world \"%s\"", selectedWorld.c_str());
					setGameState(GameState::WORLD, selectedWorld);
				} break;
				

				/** Options menu buttons */
				case GUIHandler::btn_OPTIONS_back: 
				case GUIHandler::btn_SELECT_CAMPAIGN_back: {
					if( gamestate==GameState::WORLD ) {
						guiHandler.setGUIs(GUIHandler::GUIs::PAUSE);
					} else {
						guiHandler.setGUIs(GUIHandler::GUIs::MAIN);
					}
				} break;

				/** Pause Menu */
				case GUIHandler::btn_PAUSED_back: {
					paused = false;
					guiHandler.setGUIs(GUIHandler::GUIs::UNPAUSE);
				} break;
				case GUIHandler::btn_PAUSED_exit: {
					setGameState(GameState::MAIN_MENU);
					paused = false;
				} break;
				
				/* Player Menu */
				case GUIHandler::rbtn_CHARACTER_inventory:
				case GUIHandler::rbtn_CHARACTER_engineering:
				{
					PlayerMenu* lpm = world->getLocalPlayerMenu();
					if( lpm!=nullptr ) {
						lpm->setState( gaid-(GUIHandler::rbtn_CHARACTER_engineering)+2 );
					}
				} break;
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
		ss << "(pFPS, pTPS)=(" << MainLoop::getCurrentPFPS() << ", " << MainLoop::getCurrentPTPS() << "); ";
		DebugScreen::newLine(ss);
	DebugScreen::endGroup(tabs);

	//SDLHandler
	DebugScreen::newGroup(ss, tabs, "SDLHandler");
		if(!true) {
			DebugScreen::indentLine(ss, tabs);
			ss << sdlHandler->getSystemRamDesc() << ";";
			DebugScreen::newLine(ss);
		}

		if(!true) {
			DebugScreen::indentLine(ss, tabs);
			ss << sdlHandler->getVideoDriversDesc() << ";";
			DebugScreen::newLine(ss);
		}

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
		if(true) {
			DebugScreen::newGroup(ss, tabs, "BTE::guiHandler");
				guiHandler.putInfo(ss, tabs);
			DebugScreen::endGroup(tabs);
		}


		if( world!=nullptr ) {
			DebugScreen::newGroup(ss, tabs, "BTE::world");
				world->putInfo(ss, tabs);
			DebugScreen::endGroup(tabs);
		}

		if( tests!=nullptr ) {
			DebugScreen::newGroup(ss, tabs, "BTE::tests");
				tests->putInfo(ss, tabs);
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

void BTE::setGameState(int p_gamestate, std::string extraInfo)
{
	// Upon gamestate change, we should unload world and tests
	if( gamestate!=p_gamestate ) {
		unload(world);
		unload(tests);
	}

	// Set gamestate
	gamestate = p_gamestate;
	Log::log("Switching to gamestate %d...", gamestate);

	// Actions depending on new gamestate value
	switch(p_gamestate) {
		// Exit game
		case EXIT: { MainLoop::quit(); } break;

		// Testing stuff (textures, general testing)
		case TEXTURES: { guiHandler.removeAllUserGUIs(); } break;
		case TESTING: {
			guiHandler.removeAllUserGUIs();
			load(tests);
		} break;

		// Go to main menu
		case MAIN_MENU: {
			guiHandler.setGUIs(GUIHandler::GUIs::MAIN);
			AudioLoader* al = sdlHandler->getAudioLoader();
			al->stopPlayingMusic();
			al->playMusicTitleTheme();
			
			if( !playedImpact ) {
				al->play(AudioLoader::TITLE_impact);
				playedImpact = true;
			}
		} break;

		// Go to campaign selection
		case SELECT_CAMPAIGN: {
			guiHandler.setGUIs(GUIHandler::GUIs::SELECT_CAMPAIGN);
		} break;

		// Go to world
		case WORLD: {
			AudioLoader* al = sdlHandler->getAudioLoader();
			al->stopPlayingMusic();

			guiHandler.setGUIs(GUIHandler::GUIs::WORLD);
			load(world, extraInfo);
		} break;

		// Go to unknown gamestate (testing)
		default: {
			Log::warn(__PRETTY_FUNCTION__, "Tried to switch to invalid gamestate", "going to TESTING (-1)");
			setGameState(TESTING);
		} break;
	}
}
void BTE::setGameState(int p_gamestate) { setGameState(p_gamestate, ""); }

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

void BTE::load(World*& world, std::string dirName)
{
	unload(world);
	world = new World(dirName);
	world->init(sdlHandler, &guiHandler, fileHandler, controls);
}

void BTE::load(Tests*& tests)
{
	unload(tests);
	tests = new Tests();
	tests->init(sdlHandler, fileHandler, controls);
}