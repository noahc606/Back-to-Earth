#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "BTEObject.h"
#include "Canvas.h"
#include "Defs.h"
#include "Planet.h"
#include "Player.h"
#include "RegTexInfo.h"
#include "TileIterator.h"
#include "TileMap.h"
#include "TileMapUpdater.h"
#include "TileRegion.h"
#include "TileType.h"

class TileMapScreen : public BTEObject
{
public:
    /**/
    void init( SDLHandler* sh, FileHandler* fh, TileMap* tileMap, Canvas* cs);
    virtual ~TileMapScreen();
    void destroy();
    /**/
    void tick();
    void draw();
    void drawDebugOverlay(Canvas* csInteractions);
    void putInfo(std::stringstream& ss, int& tabs, int64_t mouseX, int64_t mouseY, int64_t mouseZ, bool mouseExists);
    void putInfo(std::stringstream& ss, int& tabs, int64_t mouseX, int64_t mouseY, int64_t mouseZ);
    void putInfo(std::stringstream& ss, int& tabs);
    TileMapUpdater* getUpdater();
    /**/
    static const int regionSize = 32;                   //Size of region in tiles
    static const int tileSize = 32;                     //Size of a tile in pixels
    static const int regSize = regionSize*tileSize;     //Size of region in pixels. Doesn't matter what camera zoom is.

	
private:
	void updateMinimap();
    
	Camera* cam = nullptr;							//Area of the world shown on screen. Also tracks size of each tile (zoom factor).
	TileMap* tileMap = nullptr;                     //TileMap we are drawing
	Canvas* csTileMap = nullptr;                    //Canvas object depicting the TileMap
	
    RegTexInfo regTexInfo;
    TileMapUpdater tileMapUpdater;
    Texture minimap;

	int camRX = 0; int camRY = 0; int camRZ = 0;	//Coordinates of region which the center of the screen occupies. e.g. As player moves regionSize tiles to the right, cameraRX increases by 1.
	int camL = 0;                                   //Layer which the camera occupies
	int camD = -1;                                  //Direction of the camera (see Camera::Directions)

	/*
	 * Zoom factor. Determines how many onscreen pixels take up 1px of a single onscreen 32x32px tile
	 * e.g. mapZoom of 2 = each tile will be 64x64pixels on the screen.
	 */
	double camZoom = 2;
	int daySegment = -1;	//Whenever this changes, we need to update all tiles on screen to reflect different sky colors

    /* Map update distance + counts */
    int umiTicks = 0;
    int umiTicksMax = 10;	//TODO: Base this on performance level of drawing
    int loadDist = 6;
        
    std::string currentDimPath = "default";

    /* Performance gauging */
    // Next second and #draws this second
    uint64_t nextSecond = 0;
    int drawsThisSecond = 0;
    // Info: tick time, draw time
    double infoTickTime = 0.0;
    double infoDrawTime = 0.0;
};
