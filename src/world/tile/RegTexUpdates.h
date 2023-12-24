#pragma once
#include <sstream>
#include "Canvas.h"
#include "TileIterator.h"
#include "TileMap.h"

class RegTexUpdates
{
public:
    RegTexUpdates(SDLHandler* sdlHandler, Camera* camera, TileMap* tileMap, Canvas* csTileMap);
    virtual ~RegTexUpdates();

    static std::tuple<int64_t, TileType> camTrackedTile(TileIterator& ti, int camDirection);
    static std::tuple<int64_t, TileType> topTrackedTile(TileIterator& ti);
    void info(std::stringstream& ss, int& tabs);
    int getScreenSemiWidth();
    int getScreenSemiHeight();
    bool isOnScreen(int rX, int rY);
    int getTileScale();
    int getBlitScale();

    void setScreenInfo();
    void updateScaling(int ts, int bs);
    void updateTimeAvg(int drawsThisSecond);
    void draw(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH);
    void colorFillRegionArea(int64_t rX, int64_t rY, uint8_t r, uint8_t g, uint8_t b);
    void blackOutRegionArea(int64_t rX, int64_t rY);

    void placeEntireScreen();
    void processRegions(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH);
    void processRegions2(int64_t camRX, int64_t camRY, int64_t camRZ, int loadRadiusH);
    void processRegionLayer(TileIterator& ti, int64_t rX, int64_t rY, int64_t camRZ, int64_t camLayer);
    void processTileArea(TileIterator& ti, Texture* tex);

protected:

private:

    SDLHandler* sdlHandler = nullptr;
    Camera* camera = nullptr;
    TileMap* tileMap = nullptr;
    Canvas* csTileMap = nullptr;

    int tileScale = 32;
    int blitScale = 32;
    int regionScale = 32*32;

    int screenWT = 1; int screenHT = 1;
    int screenWR = 1; int screenHR = 1;
    int screenSWR = 1; int screenSHR = 1;

    /* Performance gauging */
    int rtUpdatesToDo = 1024;
    double rtUpdatesTime = -1.0;
    double rtUpdatesTimeTotal = -1.0;
    double rtUpdatesTimeAvg = -1.0;
};
