#pragma once
#include "BTEObject.h"
#include "Canvas.h"
#include "GUIHandler.h"
#include "LevelSave.h"
#include "Loggable.h"
#include "Planet.h"
#include "Player.h"
#include "PlayerMenu.h"
#include "Texture.h"
#include "TileMap.h"
#include "TileMapScreen.h"

class World : public BTEObject, public Loggable
{
public:
	/**/
	World();
	virtual ~World();
	void init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls);
	/**/
	void draw();
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
	
	bool lpMenuState = false;
	bool lpMenuStateLast = false;
	double mouseX = 0; double mouseY = 0;
	int64_t mouseXLL = 0; int64_t mouseYLL = 0; int64_t mouseZLL = 0; int64_t mouseCLL = 0;
	
	TileMap tileMap;
	TileMapScreen tileMapScreen;
	
	Canvas csTileMap;
	Canvas csATileMap;
	Canvas csInteractions;
	Canvas csEntities;
	
	double performanceCounter = 0;
	
	Planet planet;
	
	std::string worldDataPath;
	Settings::t_kvMap worldData;
	
	void updateMouseAndCamInfo();
	void playerInteractions(GUIHandler& guiHandler, bool paused);
	void setLocalPlayerMenuState(int newMenuState);
};
