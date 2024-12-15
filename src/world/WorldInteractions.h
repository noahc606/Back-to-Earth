#pragma once
#include <tuple>
#include "Canvas.h"
#include "Controls.h"
#include "GUIHandler.h"
#include "MissionHolder.h"
#include "PlayerMenu.h"
#include "SDLHandler.h"
#include "Tile.h"
#include "TileMapScreen.h"

class WorldInteractions {
public:
    WorldInteractions();
    ~WorldInteractions();

	void initPlayerEtc(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls, TileDict* td, std::tuple<double, double, double> pXYZ, std::string pMode);

	void draw(Canvas& csInteractions);
	void drawLocalPlayerMenu();
	void updateMouseAndCamInfo(Canvas& csInteractions, TileMap* tileMap);
	void playerController(TileMapScreen* tms, TileMap* tm, bool paused);
	void playerActions(TileMapScreen* tms, TileMap* tm);
	void playerInteractTile(TileMapScreen* tms, TileMap* tm);
	void playerTryPlaceTile(TileMapScreen* tms, TileMap* tm, Tile t, bool force);
	void playerTryDestroyTile(TileMapScreen* tms, TileMap* tm);
	void setLocalPlayerMenuState(int newMenuState);
	void playMusic();

	SDLHandler* sdlHandler = nullptr;
	GUIHandler* guiHandler = nullptr;
	Controls* controls = nullptr;

	/* Player */
	Player localPlayer;
	PlayerMenu localPlayerMenu;
	MissionHolder missionHolder;

	/* GUI Info */
	double prevZoom = -1;
	bool lpMenuOpen = false;
	bool lpMenuOpenLast = false;
	int lpMenuLastModule = -1;

	nch::Vec3<double> mousePosD = nch::Vec3<double>(0);		//Decimal
	nch::Vec3<int64_t> mousePosI = nch::Vec3<int64_t>(0);	//Integer
	/* World info */
	std::string worldDirName;
	std::string worldDirPath;
	std::string worldDataPath;
	Settings::t_kvMap worldDataKVs;
	int playTime = 0;

private:

};