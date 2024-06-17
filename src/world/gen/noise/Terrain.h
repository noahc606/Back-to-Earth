#pragma once
#include "TileRegion.h"

class Terrain
{
public:
    Terrain(int64_t seed);
    virtual ~Terrain();

	void genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void genericRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);


protected:

private:
    int64_t seed = 0;
    std::vector<TileType> worldTiles;
    float vScale0, vScale1;
    float tZoom, mZoom, lZoom, cZoom;
};
