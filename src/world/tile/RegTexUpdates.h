#pragma once
#include <sstream>
#include "Canvas.h"
#include "Defs.h"
#include "TileIterator.h"
#include "TileMap.h"

class RegTexUpdates
{
public:
    RegTexUpdates(SDLHandler* sdlHandler, Camera* camera, TileMap* tileMap, Canvas* csTileMap);
    virtual ~RegTexUpdates();

    static std::tuple<Defs::t_ll, Defs::t_ll, TileType> topTrackedTile(TileIterator& ti);
    void info(std::stringstream& ss, int& tabs);
    int getScreenSemiWidth();
    int getScreenSemiHeight();
    int getTileScale();
    int getBlitScale();

    void setScreenInfo();
    void updateScaling(int ts, int bs);
    void updateTimeAvg(int drawsThisSecond);
    void draw(Defs::t_ll camRX, Defs::t_ll camRY, Defs::t_ll camRZ, int loadRadiusH);
    void colorFillRegionArea(Defs::t_ll rX, Defs::t_ll rY, uint8_t r, uint8_t g, uint8_t b);
    void blackOutRegionArea(Defs::t_ll rX, Defs::t_ll rY);

    void placeEntireScreen();
    void placeRandom(int updates);
    void processRegion(Defs::t_ll camRX, Defs::t_ll camRY, Defs::t_ll camRZ, int loadRadiusH);
    void processRegionLayer(TileIterator& ti, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll camRZ, Defs::t_ll camLayer);
    void processTileArea(TileIterator& ti, Texture* tex);

protected:

private:

    SDLHandler* sdlHandler = nullptr;
    Camera* camera = nullptr;
    TileMap* tileMap = nullptr;
    Canvas* csTileMap = nullptr;

    int tileScale = 32;
    int blitScale = 32;

    int screenWT = 1; int screenHT = 1;
    int screenWR = 1; int screenHR = 1;
    int screenSWR = 1; int screenSHR = 1;

    /* Performance gauging */
    int rtUpdatesToDo = 1024;
    double rtUpdatesTime = -1.0;
    double rtUpdatesTimeTotal = -1.0;
    double rtUpdatesTimeAvg = -1.0;
};
