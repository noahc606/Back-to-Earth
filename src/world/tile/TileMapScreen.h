#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "BTEObject.h"
#include "Canvas.h"
#include "Player.h"
#include "RegTexUpdates.h"
#include "TileIterator.h"
#include "TileMap.h"
#include "TileRegion.h"
#include "TileType.h"

class TileMapScreen : public BTEObject
{
public:
    /**/
    void init( SDLHandler* sdlHandler, FileHandler* fileHandler, Controls* ctrls, Player* player, TileMap* tileMap, Canvas* csTileMap );
    virtual ~TileMapScreen();
    void destroy();

    /**/
    void tick();
    void draw();

    /**/
    void info(std::stringstream& ss, int& tabs, TileMap::t_ll mouseX, TileMap::t_ll mouseY, TileMap::t_ll mouseZ);

    /* Variables */
    static const int regionSize = 32;                   //Size of region in tiles
    static const int tileSize = 32;                     //Size of a tile in pixels
    static const int regSize = regionSize*tileSize;     //Size of region in pixels. Doesn't matter what camera zoom is.

private:
    void updateMapVisible();
    void updateRegTicked(int rX, int rY, int rZ);
    void updateMapTicked();
    void updateMapMoved();
    void updateMapIdle();

    Camera* cam = nullptr;                              //Area of the world shown on screen. Also tracks size of each tile (zoom factor).
    TileMap* tileMap = nullptr;
    Canvas* csTileMap = nullptr;
    RegTexUpdates* regTexUpdates = nullptr;

    int camRX = 0; int camRY = 0; int camRZ = 0;        //Coordinates of region which the center of the screen occupies. e.g. As player moves regionSize tiles to the right, cameraRX increases by 1.
    int camL = 0;

    double camZoom = 2;                                 //Zoom factor. Determines how many onscreen pixels take up 1px of a single onscreen 32x32px tile
                                                        //e.g. mapZoom of 2 = each tile will be 64x64pixels on the screen.
    double regScale = tileSize*regionSize*camZoom;      //Size of the region in screen pixels based on camera's zoom.

    /* Map update distance + counts */
    int umiTicks = 0;
    int umiTicksMax = 300;
    int loadCountMax = 1;
    int loadRadiusH = 4;
    int loadRadiusV = 1;
    std::string currentDimPath = "default";

    /* Performance gauging */
    // Next second and #draws this second
    uint64_t nextSecond = 0;
    int drawsThisSecond = 0;
    // Info: tick time, draw time
    double infoTickTime = 0.0;
    double infoDrawTime = 0.0;
    // Info: region load counting
    int infoRegLoadDivisor = 20;
    int infoRegLoadCount = 0;
    // Info: region load time
    double infoRegLoadTime = 0.0;
    double infoRegLoadTimeLatest = 0.0;
    double infoRegLoadTimeAvg = 0.0;
};
