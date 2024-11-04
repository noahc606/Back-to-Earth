#include "World.h"
#include <math.h>
#include <nch/cpp-utils/log.h>
#include <time.h>
#include "TextureBuilder.h"
#include "Atmosphere.h"
#include "MainLoop.h"
#include "Timer.h"

Texture World::defaultTile;

World::World(std::string dirName)
{
	if(dirName=="") {
		dirName = "world1";
	}
	inter.worldDirName = dirName;
	inter.worldDirPath = "saved/games/"+inter.worldDirName;
	inter.worldDataPath = inter.worldDirPath+"/data.txt";
}

void World::init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
	if( exists() ) return;
	BTEObject::init(sh, fh, ctrls);
	guiHandler = gh;
	

	/* INIT 0a: Files/directories */
	// Create world files and directories
	fileHandler->createBTEDir(inter.worldDirPath);
	fileHandler->createBTEDir(inter.worldDirPath+"/missions");
	fileHandler->createBTEDir(inter.worldDirPath+"/tilemap/default");
	fileHandler->createBTEDir(inter.worldDirPath+"/tiledict");
	if (!fh->fileExists(inter.worldDataPath) ) {
		fh->cEditFile(inter.worldDataPath);
		fh->saveCloseFile();
	}

	/* INIT 0b: world saved settings */
	//Build default world settings
	Settings::t_kvMap lwd;	//LWD = loaded world data
	Settings::kv(&lwd, "planetRotation", 19200);
	Settings::kv(&lwd, "playerGameMode", "sandbox");
	Settings::kv(&lwd, "playerX", "0");
	Settings::kv(&lwd, "playerY", "0");
	Settings::kv(&lwd, "playerZ", "-64");
	Settings::kv(&lwd, "playTime", 0);
	Settings::kv(&lwd, "worldSeed", "0");
	Settings::kv(&lwd, "worldName", "world1");
	//Build loaded world settings into worldDataKVs
	inter.worldDataKVs = fileHandler->readTxtFileKVs(inter.worldDataPath);
	fh->getSettings()->loadNewMapIntoOld(&lwd, inter.worldDataKVs);
	inter.worldDataKVs = lwd;
		
	//Store information in variables
	std::string pMode = Settings::get(inter.worldDataKVs, "playerGameMode");
	double px = Settings::getNum(inter.worldDataKVs, "playerX");
	double py = Settings::getNum(inter.worldDataKVs, "playerY");
	double pz = Settings::getNum(inter.worldDataKVs, "playerZ");
	inter.playTime = Settings::getNum(inter.worldDataKVs, "playTime");
	int64_t worldSeed = Settings::getI64(inter.worldDataKVs, "worldSeed");
	double plntRot = Settings::getNum(inter.worldDataKVs, "planetRotation");
	nch::Log::log("Loaded save data: player(%f, %f, %f); planetRotation=%f\n", px, py, pz, plntRot);
	std::cout << "Seed: " << worldSeed << "\n";

	/* INIT 1: Planet */
	//Planet
	tileDict.init(sh, inter.worldDirName, "default");
	planet.init(plntRot);

	/* INIT 2: Player */
	inter.initPlayerEtc(sdlHandler, guiHandler, fileHandler, controls, &tileDict, std::make_tuple(px, py, pz), pMode);

	/* INIT 3: Graphical */
	initCanvases();
	wbg.init(sdlHandler, inter.localPlayer.getCamera(), &planet);

	/* INIT 4: StructureMap, TileMap, TileMapScreen, Minimap */
	noiseMap.init(worldSeed);
	struMap.init(&noiseMap, &tileDict, inter.localPlayer.getCamera(), 6);
	tileMap.init(sdlHandler, &planet, &struMap, &noiseMap, &tileDict, inter.worldDirName);
	tileMapScreen.init(sdlHandler, fileHandler, &tileMap, &csTileMap);
	minimap.init(sdlHandler, inter.localPlayer.getCamera(), &tileMap);

	/* INIT 5: Miscellaneous */
	//Build textures
	TextureBuilder tb(sdlHandler);
	tb.buildDefaultTile(defaultTile);
}

void World::initCanvases()
{
	//TileMap
	csTileMap.init(sdlHandler, controls, inter.localPlayer.getCamera());
	csTileMap.setMaximumFPS(30);
	csTileMap.setTexUsingDynamicLOD(true);
	csTileMap.setTexAllocCount(5);

	//Entities
	csEntities.init(sdlHandler, controls, inter.localPlayer.getCamera());

	//Interactions canvas
	csInteractions.init(sdlHandler, controls, inter.localPlayer.getCamera());
	csInteractions.setMaximumFPS(30);

	//Debug stuff
	csDebug.init(sdlHandler, controls, inter.localPlayer.getCamera());
	csDebug.setMaximumFPS(1);
}

World::~World()
{
	// Save world objects
	Settings::t_kvMap wdKVs = inter.worldDataKVs;
	Settings::kv(&wdKVs, "planetRotation", planet.getRotationRaw() );
	Settings::kv(&wdKVs, "playerX", inter.localPlayer.getPos()[0]+0.5 );
	Settings::kv(&wdKVs, "playerY", inter.localPlayer.getPos()[1]+0.5 );
	Settings::kv(&wdKVs, "playerZ", inter.localPlayer.getPos()[2]-1 );
	Settings::kv(&wdKVs, "playTime", inter.playTime );
	fileHandler->saveSettings(wdKVs, inter.worldDataPath);

	// Save player inventory
	inter.localPlayerMenu.getInventory()->save(inter.worldDirPath);

	// Save TileMap (deals w/ TileDict saving, region saving is done elsewhere)
	tileMap.destroy();

	// Destroy canvases
	csTileMap.destroy();
	csInteractions.destroy();
	csEntities.destroy();
	csDebug.destroy();
}

void World::drawLocalPlayerMenu() { inter.drawLocalPlayerMenu(); }

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
		- 1) HUDs (player bars, minimap, etc.)
	*/

	/* Drawing canvases */
	// 1 - World Background
	wbg.draw();

	// 2 - Tiles
	tileMapScreen.draw();

	// 3 - Entities
	csEntities.clearCanvas();
	inter.localPlayer.draw(&csEntities, debugOn);
	
	//csEntities.setSourceTex(TextureLoader::missing, 0, 0);
	//csEntities.rcopy(mouseXL*32, mouseYL*32, 32, 32);
	csEntities.draw();

	// 4 - Interactions
	inter.draw(csInteractions);

	// 5 - Debug Overlays
	if(debugOn) {
		csDebug.clearCanvas();
		if(!csDebug.isFrameFinished())
			tileMapScreen.drawDebugOverlay(&csDebug);
		csDebug.draw();
	}

	/* Drawing misc. */
	// 1 - HUDs
	inter.localPlayer.drawHUD();
	minimap.draw();
}

void World::tick(bool paused, GUIHandler& guiHandler)
{
	performanceCounter = 0;
	nch::Timer t("World tick timer", false);

	/** Tick world objects if not paused */
	if( !paused ) {
		tickWorldObjs();
	} else {
		//Force-disable character menu when paused
		inter.lpMenuOpen = false;
	}

	/** Tick canvases */
	csTileMap.tick();		// 1 - Tiles
	csEntities.tick();		// 2 - Entities
	csInteractions.tick();	// 3 - Interactions
	csDebug.tick();			// 4 - Debug

	/** Interactions with world */
	inter.updateMouseAndCamInfo(csInteractions, &tileMap);
	inter.playerInteractions(sdlHandler, guiHandler, controls, &tileMapScreen, &tileMap, paused);

	performanceCounter = t.getElapsedTimeMS();
	inter.playTime++;
	if(inter.playTime==60*60*30) {
		inter.playTime = 0;
	}
}

void World::tickWorldObjs()
{
	/** World objects */
	planet.tick();
	wbg.tick();
	struMap.tick();
	tileMapScreen.tick();

	//Music, player collision+animation+logic, player menu
	tickWorldPlayer();
}

void World::tickWorldPlayer()
{
	int pt = inter.playTime;
	if( pt==200 || (pt>0 && pt%36000==0) ) {
		inter.playMusic();
	}
	
	inter.localPlayer.collision(&tileMap);
	inter.localPlayer.tick(&tileMap);

	//Tick player menu
	inter.localPlayerMenu.tick();

	//Control player state based on menu items
	if( inter.localPlayerMenu.getInventory()->getSlotItemType(0, -4, 2)==Items::QUANTUM_EXOSUIT_HELMET &&
		inter.localPlayerMenu.getInventory()->getSlotItemType(0, -4, 3)==Items::QUANTUM_EXOSUIT_BODY &&
		inter.localPlayerMenu.getInventory()->getSlotItemType(0, -4, 4)==Items::QUANTUM_EXOSUIT_LEGGINGS
	) {
		inter.localPlayer.setSpaceSuitState(Player::SpaceSuitStates::STABLE);
	} else {
		inter.localPlayer.setSpaceSuitState(Player::SpaceSuitStates::NO_SUIT);
	}
}

void World::putInfo(std::stringstream& ss, int& tabs)
{
	if(inter.localPlayer.getCamera()==nullptr) return;

	//Mouse Info
	DebugScreen::indentLine(ss, tabs);
	ss << "Mouse(xyz)=(" << inter.mouseX << ", " << inter.mouseY << ", " << inter.mouseZ << "); ";
	DebugScreen::newLine(ss);
	
	DebugScreen::indentLine(ss, tabs);
	ss << "World tick=" << performanceCounter << "ms; ";
	DebugScreen::newLine(ss);
	
	if(!true) {
		DebugScreen::newGroup(ss, tabs, "planet");
			planet.putInfo(ss, tabs);
		DebugScreen::endGroup(tabs);
	}
	
	//Player
	DebugScreen::newGroup(ss, tabs, "player");
	inter.localPlayer.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMap
	DebugScreen::newGroup(ss, tabs, "tileMap");
	if(inter.localPlayer.getCamera()!=nullptr)
		tileMap.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);
	
	//StructureMap
	DebugScreen::newGroup(ss, tabs, "struMap");
		struMap.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMapScreen
	DebugScreen::newGroup(ss, tabs, "tileMapScreen");
	tileMapScreen.putInfo(ss, tabs, inter.mouseXL, inter.mouseYL, inter.mouseZL);
	DebugScreen::endGroup(tabs);

	//Canvas
	if(!true) {
		DebugScreen::newGroup(ss, tabs, "csTileMap");
		csTileMap.putInfo(ss, tabs);
		DebugScreen::endGroup(tabs);
	}
}

Planet* World::getPlanet() { return &planet; }
Player* World::getLocalPlayer() { return &inter.localPlayer; }
PlayerMenu* World::getLocalPlayerMenu() { return &inter.localPlayerMenu; }
Texture* World::getDefaultTileTexture() { return &defaultTile; }