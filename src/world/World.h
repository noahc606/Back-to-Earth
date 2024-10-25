#pragma once
#include "BTEObject.h"
#include "Canvas.h"
#include "GUIHandler.h"
#include "LevelSave.h"
#include "Loggable.h"
#include "Planet.h"
#include "Player.h"
#include "PlayerMenu.h"
#include "RegTexInfo.h"
#include "STexture.h"
#include "StructureMap.h"
#include "Texture.h"
#include "TileMap.h"
#include "TileMapScreen.h"
#include "WorldInteractions.h"
#include "WorldBackground.h"
#include "Minimap.h"

class World : public BTEObject, public Loggable
{
public:
	/**/
	World(std::string dirName);
	virtual ~World();
	void init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls);
	void initCanvases();
	/**/
	void draw(bool debugOn);
	void tick(bool paused, GUIHandler& guiHandler);
	void tickWorldObjs();
	void tickWorldPlayer();
	/**/
    void putInfo(std::stringstream& ss, int& tabs);
	Planet* getPlanet();
	Player* getLocalPlayer();
	PlayerMenu* getLocalPlayerMenu();
	static Texture* getDefaultTileTexture();
protected:

private:
	GUIHandler* guiHandler;
	static Texture defaultTile;
	
	int counter = 0;
	
	WorldInteractions inter;

	StructureMap struMap;
	TileMap tileMap;
	TileDict tileDict;
	NoiseMap noiseMap;

	TileMapScreen tileMapScreen;
	RegTexInfo regTexInfo;
	
	Canvas csTileMap;
	Canvas csATileMap;
	Canvas csInteractions;
	Canvas csEntities;
	Canvas csDebug;
	
	
	double performanceCounter = 0;
	
	Planet planet;
	WorldBackground wbg;
	Minimap minimap;
	
	std::string worldDirName;
	std::string worldDirPath;
	std::string worldDataPath;
	Settings::t_kvMap worldDataKVs;
};
