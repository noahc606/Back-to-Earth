#pragma once
#include <vector>
#include <nch/cpp-utils/color.h>

/*
    Successor to the old TileType, with some new features:
    - Per-direction textures (6)
    - Multiple texture overlaying and coloring
*/

class Tile {
public:
    Tile();
    ~Tile();

    bool solid = true;
    bool visionBlocking = true;
    bool invisible = false;
    nch::Color modColor;
    std::vector<std::pair<int, int>> atlasTexLocations; //[0]==west side, [1]==east, [2]==north, [3]==south, [4]==up, [5]==down
private:
    void init();
};