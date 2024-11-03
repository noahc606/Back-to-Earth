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
	void playerInteractions(SDLHandler* sdlHandler, GUIHandler& guiHandler, Controls* controls, TileMapScreen* tms, TileMap* tm, bool paused);
	void playerTryPlaceTile(TileMapScreen* tms, TileMap* tm, Tile t, bool force);
	void playerTryDestroyTile(TileMapScreen* tms, TileMap* tm);
	void setLocalPlayerMenuState(int newMenuState);

	SDLHandler* sdlHandler;

	/* Player */
	Player localPlayer;
	PlayerMenu localPlayerMenu;
	MissionHolder missionHolder;

	/* GUI Info */
	double prevZoom = -1;
	bool lpMenuOpen = false;
	bool lpMenuOpenLast = false;
	int lpMenuLastModule = -1;
	double mouseX = 0; double mouseY = 0; double mouseZ = 0;
	int64_t mouseXL = 0; int64_t mouseYL = 0; int64_t mouseZL = 0;

	/* World info */
	std::string worldDirName;
	std::string worldDirPath;
	std::string worldDataPath;
	Settings::t_kvMap worldDataKVs;
	int playTime = 0;

private:

};