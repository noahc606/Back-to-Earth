#pragma once
#include <stdint.h>
#include "Camera.h"
#include "STexture.h"
#include "TileMap.h"

class Minimap {
public:
    void init(SDLHandler* sh, Camera* cam, TileMap* tm);
    void draw();
private:
	void updateTerrainPartial(int64_t startX, int64_t endX);

    SDLHandler* sdlHandler;
    TileMap* tileMap;
    Camera* camera;

    STexture terrain;
    int64_t updateTimeMS = 500; int64_t lastUpdateMS = 0;

    int64_t lastTerrainPartDrawMS = 0;
    int64_t numTerrainPartDraws = 10;
    int64_t currTerrainPartDraw = 0;

    int64_t camLX = 0, camLY = 0, camLZ = 0;
};