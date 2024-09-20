#include "BTE.h"
#include <nch/cpp-utils/io/Log.h>
#include <sstream>
#include <type_traits>
#include "ButtonAction.h"
#include "CampaignCreation.h"
#include "CurlHandler.h"
#include "GUIBuilder.h"
#include "ProgressBar.h"
#include "MainLoop.h"
#include "Tests.h"
#include "TextOld.h"
#include "TextBox.h"
#include "Tooltip.h"

/**/

BTE::BTE(){}
void BTE::preinit(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	/* Init handlers/tools */
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;
	settings = fileHandler->getSettings();
	AudioLoader* al = sdlHandler->getAudioLoader();
	
	
	/* Configure app to match saved settings */
	if( settings->get(Settings::options, "fullscreen")=="true" ) {	// Fullscreen?
		sdlHandler->toggleFullScreen();
	}
	MainLoop::setMaxFPS( settings->get(Settings::options, "maxFps") );	// Max FPS?
	al->setMasterVolumeFactor   ( settings->getNum(Settings::options, "masterVolume")/100.0 );	//Master Volume?
	al->setMusicVolumeFactor    ( settings->getNum(Settings::options, "musicVolume")/100.0 );	//Music Volume?
	al->setSfxVolumeFactor      ( settings->getNum(Settings::options, "sfxVolume")/100.0 );		//SFX Volume?

	if( settings->get(Settings::options, "bteCursor")=="true" ) {	// Custom cursor?
		sdlHandler->toggleBTECursor();
		lastBTECursorState = "true";
	}
	if( settings->get(Settings::options, "debugHacks")==Main::PASSWORD ) {	// Debugging tools?
		debugScreen.setHaxEnabled(true);
	}
	if( alwaysTest || settings->get(Settings::options, "debugTesting")==Main::PASSWORD ) {	// Testing?
		debugScreen.setHaxEnabled(true);
		testing = true;
	}
	if( alwaysTest || settings->get(Settings::options, "debugHardTesting")==Main::PASSWORD ) {	// Hard testing?
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

	if( !testing ) {
		if( settings->get(Settings::options, "showIntro")=="true" ) {
			setGameState(GameState::INTRO);
		} else {
			setGameState(GameState::MAIN_MENU);
		}
		
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

void BTE::drawCursor()
{
	if(!sdlHandler->usingBTECursor()) return;

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

void BTE::draw()
{
	//Draw gamestate specific objects
	switch(gamestate) {
		case GameState::TESTING:
		case GameState::TEXTURES: {
			if( tests!=nullptr )
				tests->draw();
		} break;

		case GameState::INTRO: {
			SDL_Rect r;
			r.w = 1024; r.h = 512;
			r.x = sdlHandler->getWidth()/2 - r.w/2;
			r.y = sdlHandler->getHeight()/2 - r.h/2;

			nch::Color cmod(255, 255, 255, 255);
			if(introTimer<80) {
				cmod.a = introTimer*3;
			}

			SDL_Texture* stex = sdlHandler->getTextureLoader()->getTexture(TextureLoader::ootws);
			SDL_SetTextureBlendMode(stex, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(stex, cmod.a);
			sdlHandler->renderCopy(TextureLoader::ootws, NULL, &r);
			TextOld::draw(sdlHandler, "presents...", sdlHandler->getWidth()/2-400/2, r.y+r.h, 8, cmod, nch::Color());
		} break;

		case GameState::WORLD: {
			if( world==nullptr ) break;
			
			world->draw( debugScreen.getVisible() && settings->get(Settings::options, "debugOverlayEnabled")=="true" );
		} break;
	}

	//Draw GUIHandler
	guiHandler.draw();
	
	//Draw player within world
	if(world!=nullptr) {
		Player* localPlayer = world->getLocalPlayer();
		PlayerMenu* localPlayerMenu = world->getLocalPlayerMenu();
		if( localPlayer!=nullptr ) {
			//Draw player within menu if player's menu is open
			if( localPlayerMenu->getModule()>=0 ) {
				localPlayer->drawCharInMenu();
			}
			
			//Draw specific menu elements
			localPlayerMenu->draw();
		}
	}
	
	debugScreen.drawMain();		//Draw debugscreen if applicable
	debugScreen.drawProfiler();	//Draw debugscreen profiler if applicable
	drawCursor();				//Draw cursor if applicable
}

void BTE::tick()
{
	std::string bteCursorState = fileHandler->getSettings()->get(Settings::TextFiles::options, "bteCursor");
	if( lastBTECursorState!=bteCursorState ) {
		lastBTECursorState = bteCursorState;
		sdlHandler->toggleBTECursor();
	}
	AudioLoader* al = sdlHandler->getAudioLoader();
	al->tick();

	/** GUIHandler/DebugScreen */
	if( guiHandler.exists() ) {
		/** GUI handler */
		//Tick
		guiHandler.tick();

		//GUI Button Action 
		int gaid = guiHandler.getGUIActionID();
		if( gaid>-1 ) {
			performGUIAction(gaid);
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

		case INTRO: {
			introTimer++;
			if(introTimer>=40*5) {
				setGameState(GameState::MAIN_MENU);
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

		case GameState::UPDATING: {
			GUI* pgui = guiHandler.getGUI(BTEObject::GUI_progressbar, GUIHandler::pbr_UPDATING_SCREEN);
			Window* pwin = guiHandler.getWindow(GUIHandler::win_UPDATING_SCREEN);
			if(pgui!=nullptr && pwin!=nullptr) {
				ProgressBar* usPbr = (ProgressBar*)pgui;
				if(usPbr->isWorkFinished()) {
					guiHandler.addGUI(new Tooltip(pwin, GUIAlignable::CENTER_H, 92+32*5, "Assets finished downloading. You should now exit and restart.", GUIHandler::ttp_GENERIC));
					guiHandler.addGUI(new Button(pwin, GUIAlignable::CENTER_H, 448-38, 300, "Exit", GUIHandler::btn_MAIN_exit));
					onWindowUpdate();
					usPbr->resetWorkFinished();
				}
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

		DebugScreen::indentLine(ss, tabs);
		uint8_t r; uint8_t g; uint8_t b; uint8_t a;
		SDL_GetRenderDrawColor(sdlHandler->getRenderer(), &r, &g, &b, &a);
		ss << "SDL_GetRenderDrawColor()=(" << (int)r << ", " << (int)g << ", " << (int)b << ", " << (int)a << ");"; 
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

bool BTE::updateBTEApp()
{
	//Prevent update sequence if not in the correct gamestate
	if(gamestate!=GameState::UPDATING) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Within invalid gamestate");
		return false;
	}
	ProgressBar* pbr = nullptr;
	GUI* pgui = guiHandler.getGUI(BTEObject::GUI_progressbar, GUIHandler::pbr_UPDATING_SCREEN);
	if(pgui!=nullptr) {
		pbr = (ProgressBar*)pgui;
	} else {
		nch::Log::warn(__PRETTY_FUNCTION__, "Could not find progress bar");
		return false;
	}

	/* Update app */
	CurlHandler* ch = new CurlHandler();
	ch->init(sdlHandler);

	std::string newVersion = "";
	std::string nbvaRes = ch->newBTEVersionAvailable(&newVersion);
	if( nbvaRes=="true" || nbvaRes=="false" ) {
		nch::Log::log("================================");
		nch::Log::log("Preparing to download assets for version \"%s\".", newVersion.c_str());

		std::vector<std::string>* assets = new std::vector<std::string>(ch->getBTEAssetPathList());
		updateNumFiles = assets->size();
		auto dirs = ch->getBTEDirList(*assets);

		//Make the necessary dirs
		nch::Log::log("CREATING DIRECTORIES:");
        FileHandler fh;
        fh.init(sdlHandler->getResourcePath(), sdlHandler->getFilesystemType());
		for(std::string s : dirs) {
			if(s.substr(0,12).compare("backtoearth/")!=0) {
				nch::Log::error(__PRETTY_FUNCTION__, "Invalid directory \"%s\" found", s.c_str());
			} else {
				if(!forceDisableUpdateDLs) {
					fh.createBTEDir(s.substr(12));
				}
				nch::Log::log(s.substr(12));
			}
		}

		pbr->initWorkTypeA(ch, assets, forceDisableUpdateDLs);

		nch::Log::log("STARTING DOWNLOADS:");
		nch::Log::log("================================");
		return true;
	}

	return false;
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
	nch::Log::log("Switching to gamestate %d...", gamestate);

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

		case INTRO: {
			if( !playedImpact ) {
				AudioLoader* al = sdlHandler->getAudioLoader();
				al->play(AudioLoader::SFX_TITLE_impact);
				playedImpact = true;
			}
		} break;

		// Go to main menu
		case MAIN_MENU: {
			guiHandler.setGUIs(GUIHandler::GUIs::MAIN_MENU);
			AudioLoader* al = sdlHandler->getAudioLoader();
			al->stopPlayingMusic();
			al->playMusicTitleTheme();
		} break;

		case UPDATING: {
			nch::Log::log("Started updating...");
			GUIBuilder gb;
			gb.buildUpdatingScreen(guiHandler);

			updateBTEApp();

			nch::Log::log("Finished updating.");

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
			nch::Log::warn(__PRETTY_FUNCTION__, "Tried to switch to invalid gamestate", "going to TESTING (-1)");
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

void BTE::performGUIAction(int guiActionID)
{
	int gaid = guiActionID;

	//Handle most button action IDs
	ButtonAction ba(sdlHandler, &guiHandler, fileHandler, controls);
	
	//Handle special button action IDs (gamestate switching, start/create new campaign, etc.)
	switch( gaid ) {
		/** Update prompt */
		case GUIHandler::btn_UPDATE_PROMPT_accept: { setGameState(GameState::UPDATING); } break;

		/** Main Menu */
		case GUIHandler::btn_MAIN_play: { setGameState(GameState::SELECT_CAMPAIGN); } break;
		case GUIHandler::btn_MAIN_exit: { setGameState(GameState::EXIT); } break;

		/** Campaign select menu */
		case GUIHandler::ssr_SELECT_CAMPAIGN_select: {
			std::string selectedWorld = guiHandler.getGUIActionData();
			nch::Log::log("Opening world \"%s\"", selectedWorld.c_str());
			setGameState(GameState::WORLD, selectedWorld);
		} break;
		case GUIHandler::btn_SELECT_CAMPAIGN_CN_mkdir: {
			if(CampaignCreation::createWorldDir(&guiHandler, fileHandler)==true) {
				setGameState(GameState::SELECT_CAMPAIGN);
			} else {
				
			}
		} break;
		

		/** Options menu buttons */
		case GUIHandler::btn_OPTIONS_back:
		case GUIHandler::btn_SELECT_CAMPAIGN_back: {
			if( gamestate==GameState::WORLD ) {
				guiHandler.setGUIs(GUIHandler::GUIs::PAUSE);
			} else {
				guiHandler.setGUIs(GUIHandler::GUIs::MAIN_MENU);
			}
		} break;
		case GUIHandler::btn_DEBUG_SETTINGS_checkForUpdates:
		case GUIHandler::btn_DEBUG_SETTINGS_forceUpdate: {
			//Try to build update prompt
			CurlHandler ch;		ch.init(sdlHandler);
			GUIBuilder gb;		gb.buildUpdatePrompt(guiHandler, ch, gaid);
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
	}
}