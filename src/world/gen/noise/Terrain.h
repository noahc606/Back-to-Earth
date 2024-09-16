#pragma once
#include "TileRegion.h"
#include "NoiseMap.h"

class Terrain
{
public:
    Terrain(int64_t seed, NoiseMap::t_baseTerrainMap* btm);
    virtual ~Terrain();

	void genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void genericRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);


protected:

private:
    int64_t seed = 0;
    NoiseMap::t_baseTerrainMap* btm;
    std::vector<TileType> worldTiles;
    float vScale0, vScale1;
    float tZoom, mZoom, lZoom, cZoom;
};
