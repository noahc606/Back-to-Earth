#pragma once
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "BTEObject.h"
#include "Canvas.h"
#include "Player.h"
#include "TileIterator.h"
#include "TileMap.h"
#include "TileRegion.h"
#include "TileType.h"

class TileMapScreen : public BTEObject
{
public:
    /**/
    void init( SDLHandler* sh, Controls* ctrls, TileMap* t_tm, Player* p_pl );
    virtual ~TileMapScreen();
    void destroy();

    /**/
    void tick();
    void draw(Canvas* csTileMap);

    /**/
    void info(std::stringstream& ss, int& tabs, TileMap::t_ll mouseX, TileMap::t_ll mouseY, TileMap::t_ll mouseZ);
    std::tuple<TileMap::t_ll, TileMap::t_ll, TileType> topTrackedTile(TileIterator& ti);


    /* Variables */
    static const int regionSize = 32;                   //Size of region in tiles
    static const int tileSize = 32;                     //Size of a tile in pixels

private:
    uint64_t idleLoadTimer = 0;

    Camera* cam = nullptr;                              //Area of the world shown on screen. Also tracks size of each tile (zoom factor).
    TileMap* tileMap = nullptr;
    int screenTX = 0;                                   //Translate everything on screen horizontally based on screen size
    int screenTY = 0;                                   //Translate everything on screen vertically based on screen size
    int camRX = 0; int camRY = 0; int camRZ = 0;        //Coordinates of region which the center of the screen occupies. e.g. As player moves regionSize tiles to the right, cameraRX increases by 1.
    int camL = 0;

    double mapZoom = 2;                                 //Zoom factor. Determines how many onscreen pixels take up 1px of a single onscreen 32x32px tile
                                                        //e.g. mapZoom of 2 = each tile will be 64x64pixels on the screen.
    int tileScale = tileSize*mapZoom;                   //Tile scale
    double regScale = tileSize*regionSize*mapZoom;      //Size of the region in screen pixels based on camera's zoom.
    const int regSize = regionSize*tileSize;            //Size of region in pixels. Doesn't matter what camera zoom is.

    /* RegTexUpdate performance gauging */
    double regTexUpdateTime = -1.0;
    double regTexUpdateTimeTotal = -1.0;
    double regTexUpdateTimeAvg = -1.0;
    uint64_t nextSecond = 0;
    int drawsThisSecond = 0;

    bool regNeedsUpdate(long rX, long rY, long rZ);
    void mapUpdates();
    void regTexUpdates(Canvas* csTileMap, int maxUpdates);

    void regTexUpdate(TileIterator& ti, Texture* tex);
};
