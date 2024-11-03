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
	void drawLocalPlayerMenu();
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
	double performanceCounter = 0;

	/* World Interactions */
	WorldInteractions inter;
	/* World Tiles */
	Planet planet;
	StructureMap struMap;
	TileMap tileMap;
	TileDict tileDict;
	NoiseMap noiseMap;
	/* World Graphics */
	TileMapScreen tileMapScreen;
	RegTexInfo regTexInfo;
	WorldBackground wbg;
	Minimap minimap;
	Canvas csTileMap;
	Canvas csATileMap;
	Canvas csInteractions;
	Canvas csEntities;
	Canvas csDebug;
};
