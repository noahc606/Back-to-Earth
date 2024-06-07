#include "World.h"
#include <math.h>
#include <time.h>
#include "TextureBuilder.h"
#include "Log.h"
#include "Atmosphere.h"
#include "MainLoop.h"
#include "Timer.h"

World::World(std::string dirName)
{
	if(dirName=="") {
		dirName = "world1";
	}
	worldDirName = dirName;
	worldDirPath = "saved/games/"+worldDirName;
	worldDataPath = worldDirPath+"/data.txt";
}

void World::init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
	if( exists() ) return;
	BTEObject::init(sh, fh, ctrls);
	guiHandler = gh;
	

	/* INIT 0a: Files/directories */
	// Create world files and directories
	fileHandler->createBTEDir(worldDirPath);
	fileHandler->createBTEDir(worldDirPath+"/tilemap/default");
	if (!fh->fileExists(worldDataPath) ) {
		fh->cEditFile(worldDataPath);
		fh->saveCloseFile();
	}
	

	/* INIT 0b: world saved settings */
	//Build default world settings
	Settings::t_kvMap lwd;	//LWD = loaded world data
	Settings::kv(&lwd, "planetRotation", 1000);
	Settings::kv(&lwd, "playerX", "0");
	Settings::kv(&lwd, "playerY", "0");
	Settings::kv(&lwd, "playerZ", "-32");
	Settings::kv(&lwd, "worldSeed", "-0");
	Settings::kv(&lwd, "worldName", "world1");
	//Build loaded world settings into worldDataKVs
	worldDataKVs = fileHandler->readTxtFileKVs(worldDataPath);
	fh->getSettings()->loadNewMapIntoOld(&lwd, worldDataKVs);
	worldDataKVs = lwd;
		
	//Store information in variables
	double px = Settings::getNum(worldDataKVs, "playerX");
	double py = Settings::getNum(worldDataKVs, "playerY");
	double pz = Settings::getNum(worldDataKVs, "playerZ");
	double plntRot = Settings::getNum(worldDataKVs, "planetRotation");
	Log::log("Loaded save data: player(%f, %f, %f); planetRotation=%f\n", px, py, pz, plntRot);
	
	/* INIT 1: Planet */
	//Planet
	planet.init(plntRot);

	/* INIT 2: Player */
	//Player
	localPlayer.init(sh, guiHandler, fileHandler->getSettings(), ctrls);
	localPlayer.setPos(px-0.5, py-0.5, pz);
	//Player menu
	localPlayerMenu.init(sdlHandler, guiHandler, ctrls, &localPlayer);
	
	/* INIT 3: Graphical */
	initCanvases();
	wbg.init(sdlHandler, localPlayer.getCamera());

	/* INIT 4: TileMap and TileMapScreen */
	tileMap.init(sdlHandler, fileHandler, &planet, worldDirName);
	tileMapScreen.init(sdlHandler, fileHandler, &tileMap, &csTileMap);

	/* INIT 5: Miscellaneous */
	//Build textures
	TextureBuilder tb(sdlHandler);
	tb.buildDefaultTile(defaultTile);
}

void World::initCanvases()
{
	//TileMap
	csTileMap.init(sdlHandler, controls, localPlayer.getCamera());
	csTileMap.setMaximumFPS(30);
	csTileMap.setTexUsingDynamicLOD(true);
	csTileMap.setTexAllocCount(5);

	//Entities
	csEntities.init(sdlHandler, controls, localPlayer.getCamera());

	//Interactions canvas
	csInteractions.init(sdlHandler, controls, localPlayer.getCamera());
	csInteractions.setMaximumFPS(30);

	//Debug stuff
	csDebug.init(sdlHandler, controls, localPlayer.getCamera());
	csDebug.setMaximumFPS(1);
}

World::~World()
{
	// Save world objects
	Settings::t_kvMap wdKVs = worldDataKVs;
	Settings::kv(&wdKVs, "planetRotation", planet.getRotationRaw() );
	Settings::kv(&wdKVs, "playerX", std::get<0>(localPlayer.getPos()) );
	Settings::kv(&wdKVs, "playerY", std::get<1>(localPlayer.getPos()) );
	Settings::kv(&wdKVs, "playerZ", std::get<2>(localPlayer.getPos()) );
	fileHandler->saveSettings(wdKVs, worldDataPath);

	// Destroy canvases
	csTileMap.destroy();
	csInteractions.destroy();
	csEntities.destroy();
	csDebug.destroy();
}

void World::draw(bool debugOn)
{
	/*
		Draw order of canvases:
		- 1) Sky (background behind tiles)
		- 2) Tiles (TileMapScreen)
		- 3) Entities (player, other entities, etc.)
		- 4) Interactions (tile selection overlay)
		- 5) Debugging (region texture state overlay)

		THEN we draw:
		- 1) HUDs (player health/defense/etc.)
	*/

	/* Drawing canvases */
	wbg.draw();

	// 2 - Tiles
	tileMapScreen.draw();

	// 3 - Entities
	csEntities.clearCanvas();
	localPlayer.draw(&csEntities, debugOn);
	csEntities.draw();

	// 4 - Interactions
	csInteractions.clearCanvas();
	csInteractions.setSourceTex(TextureLoader::GUI_player_interactions, 0, 0);					// Set tex src to GUI_player_interactions
	int64_t csx = localPlayer.getCamera()->getCsXFromPos(32*mouseXL, 32*mouseYL, 32*mouseZL);	// Get location where tile selector should be
	int64_t csy = localPlayer.getCamera()->getCsYFromPos(32*mouseXL, 32*mouseYL, 32*mouseZL);	// ...
	csInteractions.rcopyNI(csx, csy, 32, 32);													// Draw tile selector
	csInteractions.draw();

	// 5 - Debug Overlays
	if(debugOn) {
		csDebug.clearCanvas();
		if(!csDebug.isFrameFinished())
			tileMapScreen.drawDebugOverlay(&csDebug);
		csDebug.draw();
	}

	/* Drawing misc. */
	// 1 - HUDs
	localPlayer.drawHUD();
}

void World::tick(bool paused, GUIHandler& guiHandler)
{
	performanceCounter = 0;
	Timer t("World tick timer", false);

	/** Tick world objects if not paused */
	if( !paused ) {
		tickWorldObjs();
		tileMapScreen.tick();
	} else {
		//Force-disable character menu when paused
		lpMenuState = 0;
	}

	/** Tick canvases */
	csTileMap.tick();		// 1 - Tiles
	csEntities.tick();		// 2 - Entities
	csInteractions.tick();	// 3 - Interactions
	csDebug.tick();			// 4 - Debug

	/** Interactions with world */
	updateMouseAndCamInfo();
	playerInteractions(guiHandler, paused);

	performanceCounter = t.getElapsedTimeMS();
	playTime++;
	if(playTime==60*60*30) {
		playTime = 0;
	}
}

void World::tickWorldObjs()
{
	/** World objects */
	planet.tick();
	
	int pt = playTime;
	if( pt==200 || pt==60*60*10 || pt==60*60*20 ) {
		AudioLoader* al = sdlHandler->getAudioLoader();
		srand(time(NULL));

		std::vector<int> gameplayTracks = {
			AudioLoader::MUSIC_kc_50_million_year_trip,
			AudioLoader::MUSIC_kc_alien_ruins,
			AudioLoader::MUSIC_kc_digital_sunset,
			AudioLoader::MUSIC_kc_nuclear_winter,
			AudioLoader::MUSIC_kc_space_dust,
			AudioLoader::MUSIC_kc_the_witching_hour,
		};
		al->playOnce(gameplayTracks[(rand()%6)]);
	}
	
	localPlayer.collision(&tileMap);
	localPlayer.tick(&tileMap);

	localPlayerMenu.tick();
}

void World::putInfo(std::stringstream& ss, int& tabs)
{
	if(localPlayer.getCamera()==nullptr) return;

	//Mouse Info
	DebugScreen::indentLine(ss, tabs);
	ss << "Mouse(XYZ)=(" << mouseXL << ", " << mouseYL << ", " << mouseZL << "); ";
	ss << "Mouse(xyz)=(" << mouseX << ", " << mouseY << ", " << mouseZ << "); ";

	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "World tick=" << performanceCounter << "ms; ";
	DebugScreen::newLine(ss);
	
	if(!true) {
		DebugScreen::newGroup(ss, tabs, "World::planet");
			planet.putInfo(ss, tabs);
		DebugScreen::endGroup(tabs);
	}
	
	//Player
	DebugScreen::newGroup(ss, tabs, "World::player");
	localPlayer.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMap
	DebugScreen::newGroup(ss, tabs, "World::tileMap");
	if(localPlayer.getCamera()!=nullptr)
		tileMap.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMapScreen
	DebugScreen::newGroup(ss, tabs, "World::tileMapScreen");
	tileMapScreen.putInfo(ss, tabs, mouseXL, mouseYL, mouseZL);
	DebugScreen::endGroup(tabs);

	//Canvas
	DebugScreen::newGroup(ss, tabs, "CanvasTileMap");
	csTileMap.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

}

Planet* World::getPlanet() { return &planet; }
Player* World::getLocalPlayer() { return &localPlayer; }
PlayerMenu* World::getLocalPlayerMenu() { return &localPlayerMenu; }

void World::updateMouseAndCamInfo()
{
	//Get screen mouseX and screen mouseY both in double and in long long (int64_t) form.
	double mouseSX = std::get<0>(csInteractions.getMouseXY());
	int64_t mouseSXL = (int64_t)floor(mouseSX);
	double mouseSY = std::get<1>(csInteractions.getMouseXY());
	int64_t mouseSYL = (int64_t)floor(mouseSY);

	//Get X, Y, and Z region of camera
	int64_t mouseSRX = TileMap::getRegRXYZ(mouseSXL);
	int64_t mouseSRY = TileMap::getRegRXYZ(mouseSYL);
	int64_t cSL = localPlayer.getCamera()->getLayer();
	int64_t cSRZ = TileMap::getRegRXYZ(cSL);
	
	
	//Get mouseZLL - the position of the top visible tile in view of the camera.
	//At the last line of each case block: Get distance of the closest solid tile "underneath" the mouse.
	TileIterator ti(&tileMap);
	

	switch( localPlayer.getCamera()->getAxis() ) {
		case Camera::X: {
			mouseY = mouseSX;
			mouseYL = mouseSXL;
			mouseZ = mouseSY;
			mouseZL = mouseSYL;
			
			ti.setBoundsByRXYZ( cSRZ, mouseSRX, mouseSRY );
			ti.setTrackerSub( TileMap::getRegSubPos(cSL), TileMap::getRegSubPos(mouseSXL), TileMap::getRegSubPos(mouseSYL) );
			
			mouseXL = cSL+( std::get<0>(RegTexInfo::camTrackedTile(ti, localPlayer.getCamera()->getDirection()) ) );
		} break;
		case Camera::Y: {
			mouseX = mouseSX;
			mouseXL = mouseSXL;
			mouseZ = mouseSY;
			mouseZL = mouseSYL;
			
			ti.setBoundsByRXYZ( cSRZ, mouseSRY, mouseSRX );
			ti.setTrackerSub( TileMap::getRegSubPos(cSL), TileMap::getRegSubPos(mouseSYL), TileMap::getRegSubPos(mouseSXL) );
			
			mouseYL = cSL+( std::get<0>(RegTexInfo::camTrackedTile(ti, localPlayer.getCamera()->getDirection()) ) );
		} break;
		case Camera::Z: {
			mouseX = mouseSX;
			mouseXL = mouseSXL;
			mouseY = mouseSY;
			mouseYL = mouseSYL;
			
			ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseSXL), TileMap::getRegRXYZ(mouseSYL), cSRZ);
			ti.setTrackerSub( TileMap::getRegSubPos(mouseSXL), TileMap::getRegSubPos(mouseSYL), TileMap::getRegSubPos(cSL) );
			
			mouseZL = cSL+( std::get<0>(RegTexInfo::camTrackedTile(ti, localPlayer.getCamera()->getDirection()) ) );
		} break;
	}
	

	//Get mouseCLL - position of camera layer.
	//mouseCL = localPlayer.getCamera()->getLayer();
}

void World::playerInteractions(GUIHandler& guiHandler, bool paused)
{
	//Control character menu state
	if( !paused && controls->isPressed("PLAYER_INVENTORY") ) {
		if(lpMenuState) {
			lpMenuState = false;
		} else {
			lpMenuState = true;
		}
		controls->stopPress("PLAYER_INVENTORY", __PRETTY_FUNCTION__);
	}
	
	if( lpMenuStateLast!=lpMenuState ) {
		lpMenuStateLast = lpMenuState;
		setLocalPlayerMenuState(lpMenuState);
		
		if( lpMenuState ) {
			setLocalPlayerMenuState(1);
		} else {
			setLocalPlayerMenuState(0);
		}
	}

	if( !lpMenuState ) {		
		bool audio = false;
		AudioLoader* al = sdlHandler->getAudioLoader();
		if(al!=nullptr) {
			audio = true;
		}
				
		switch( localPlayer.getAction() )
		{
			case Player::Action::GM_Destroy_Tile: {
				playerTryDestroyTile();
			}; break;
			case Player::Action::GM_Place_Tile: {

				TileType tt;
				tt.init();
				tt.setRGB(localPlayerMenu.getSandboxTexRed(), localPlayerMenu.getSandboxTexGreen(), localPlayerMenu.getSandboxTexBlue());
				tt.setSolid(true);
				tt.setTextureXY(localPlayerMenu.getSandboxTexX(), localPlayerMenu.getSandboxTexY());
				tt.setVisionBlocking(true);
				playerTryPlaceTile(tt, false);
			}; break;
		}
	}
}

void World::playerTryPlaceTile(TileType tt, bool force)
{
	Camera* cam = localPlayer.getCamera();
	/*
	int64_t mouseCsXL = Camera::getCsXFromXYZ(cam, mouseXL, mouseYL, mouseZL);
	int64_t mouseCsXL = Camera::getCsYFromXYZ(cam, mouseXL, mouseYL, mouseZL);
	int64_t mouseCsXL = Camera::getCsZFromXYZ(cam, mouseXL, mouseYL, mouseZL);
	*/
	
	int64_t mouseCsXL = mouseXL;
	int64_t mouseCsYL = mouseYL;
	int64_t mouseCsZL = mouseZL;
	switch(localPlayer.getCamera()->getAxis()) {
		case Camera::X: {
			mouseCsXL = mouseYL;
			mouseCsYL = mouseZL;
			mouseCsZL = mouseXL;
		} break;
		case Camera::Y: {
			mouseCsXL = mouseXL;
			mouseCsYL = mouseZL;
			mouseCsZL = mouseYL;
		} break;
	}

	//Try to place tile
	if (localPlayer.inGodMode() || force ||
		!tileMap.getTileByCsXYZ(cam, mouseCsXL, mouseCsYL, cam->getLayer()).isSolid() )
	{
		tileMap.setTileByCsXYZ(cam, mouseCsXL, mouseCsYL, cam->getLayer(), tt);
	}

	//Add a 3x3 of tile updates regardless of tile placed or not
	switch(cam->getAxis()) {
		case Camera::X: { tileMapScreen.getUpdater()->addTileUpdates(mouseYL, mouseZL); } break;
		case Camera::Y: { tileMapScreen.getUpdater()->addTileUpdates(mouseXL, mouseZL); } break;
		case Camera::Z: { tileMapScreen.getUpdater()->addTileUpdates(mouseXL, mouseYL); } break;
	}
}

void World::playerTryDestroyTile()
{
	TileType tt;
	tt.init();
	tt.setTechnical(true);
	playerTryPlaceTile(tt, true);
}

void World::setLocalPlayerMenuState(int newMenuState)
{
	localPlayerMenu.setState(newMenuState);
}