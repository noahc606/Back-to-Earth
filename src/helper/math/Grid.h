#pragma once
#include <tuple>
#include "TileMap.h"

class Grid {
public:
    Grid(SDLHandler* sh, TileMap* tm, Camera* cam, int64_t loadDistH, int64_t loadDistV, int stateToUse);
    ~Grid();

    void logInfo();

    static int taxiDist(int x1, int y1, int x2, int y2);
    std::pair<int, int> getLowestElementXY();

    void floodfill(int** array, int w, int h, int x, int y, int c);
private:

    void alloc(int w, int h);

    int** grid;
    int w = 0; int h = 0;
};