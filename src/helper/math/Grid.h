#pragma once
#include <tuple>
#include "TileMap.h"

class Grid {
public:
    ~Grid();

    void updateOrderFinder(SDLHandler* sh, TileMap* tm, Camera* cam, int loadDist, int stateToUse);

    void logInfo();

    static int taxiDist(int x1, int y1, int x2, int y2);
    std::pair<int, int> getLowestElementXY();
private:

    void alloc(int w, int h);

    int** grid;
    int w = 0; int h = 0;
};