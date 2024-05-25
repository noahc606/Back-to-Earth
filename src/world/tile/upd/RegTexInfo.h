#pragma once
#include <sstream>
#include "Canvas.h"
#include "TileIterator.h"
#include "RegTexProcessor.h"

class RegTexInfo
{
public:
    virtual ~RegTexInfo();
	void init(SDLHandler* sh, TileMapUpdater* tm, Canvas* cs);
	
    static std::pair<int64_t, TileType> camTrackedTile(TileIterator& ti, int camDirection);
    static std::pair<int64_t, TileType> topTrackedTile(TileIterator& ti);
    static bool isRegOnScreen(Camera* cam, int64_t rX, int64_t rY, int64_t rZ);
    static bool isCsRegOnScreen(Camera* cam, int64_t csRX, int64_t csRY);

    void info(std::stringstream& ss, int& tabs);
	Camera* getCamera();
	Canvas* getTileMapCanvas();

    void setScreenInfo();
    void updateScaling(int ts, int bs);
    void updateTimeAvg(int drawsThisSecond);
    void draw(int64_t camRX, int64_t camRY, int64_t camRZ, int loadDistH);
    void drawDebugOverlay(Canvas* csDebug, int64_t camRX, int64_t camRY, int64_t camRZ, int loadDist);

protected:

private:

	TileMap* tileMap = nullptr;
	SDLHandler* sdlHandler = nullptr;
	Camera* cam = nullptr;
	Canvas* csTileMap = nullptr;
    RegTexProcessor regTexProcessor;

    /* Performance gauging */
    int rtUpdatesToDo = 1024;
    double rtUpdatesTime = -1.0;
    double rtUpdatesTimeTotal = -1.0;
    double rtUpdatesTimeAvg = -1.0;
};
