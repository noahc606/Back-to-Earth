#pragma once
#include "TileIterator.h"
#include "ScreenUpdater.h"

class RegTexProcessor
{
public:
    void init(SDLHandler* sh, Canvas* cs, ScreenUpdater* tmu);

    void processRegions(Camera* cam, int loadDist, int rtUpdatesToDo);
    void processRegionLayer(TileIterator& ti, int64_t sRX, int64_t sRY, int64_t sRZ, Camera* cam, int rtUpdatesToDo);
    void processTileArea(TileIterator& ti, Texture* tex, int blitScale);
	void buildRegionArea(int64_t sRX, int64_t sRY);

    static void colorFillRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    static void colorFillRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY, uint8_t r, uint8_t g, uint8_t b);
    static void clearRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY);
private:
    SDLHandler* sdlHandler = nullptr;
    Canvas* csTileMap = nullptr;
    
    Camera* cam = nullptr;
    TileMap* tileMap = nullptr;
    ScreenUpdater* screenUpdater = nullptr;
};