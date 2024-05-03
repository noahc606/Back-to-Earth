#pragma once
#include "BTEObject.h"
#include "Canvas.h"
#include "GUIHandler.h"
#include "LevelSave.h"
#include "Loggable.h"
#include "Planet.h"
#include "Player.h"
#include "PlayerMenu.h"
#include "RegTexUpdater.h"
#include "Texture.h"
#include "TileMap.h"
#include "TileMapScreen.h"

class World : public BTEObject, public Loggable
{
public:
	/**/
	World(std::string dirName);
	virtual ~World();
	void init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls);
	/**/
	void draw(bool debugOn);
	void tick(bool paused, GUIHandler& guiHandler);
	/**/
    void putInfo(std::stringstream& ss, int& tabs);
	Planet* getPlanet();
	Player* getLocalPlayer();
	PlayerMenu* getLocalPlayerMenu();
protected:

private:
	GUIHandler* guiHandler;
	Texture defaultTile;
	
	int counter = 0;
	
	double prevZoom = -1;
	Player localPlayer;
	PlayerMenu localPlayerMenu;
	int playTime = 0;
	
	bool lpMenuState = false;
	bool lpMenuStateLast = false;
	double mouseX = 0; double mouseY = 0; double mouseZ = 0;
	int64_t mouseXL = 0; int64_t mouseYL = 0; int64_t mouseZL = 0;
	
	TileMap tileMap;
	TileMapScreen tileMapScreen;
	RegTexUpdater regTexUpdater;
	
	Canvas csTileMap;
	Canvas csATileMap;
	Canvas csInteractions;
	Canvas csEntities;
	
	double performanceCounter = 0;
	
	Planet planet;
	
	std::string worldDirName;
	std::string worldDirPath;
	std::string worldDataPath;
	Settings::t_kvMap worldDataKVs;
	
	void updateMouseAndCamInfo();
	void playerInteractions(GUIHandler& guiHandler, bool paused);
	void playerPlaceTile();
	void playerDestroyTile(bool touchingSolid);
	void setLocalPlayerMenuState(int newMenuState);
};
