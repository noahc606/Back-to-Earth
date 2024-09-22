#pragma once
#include "TileRegion.h"
#include "NoiseMap.h"

class Terrain
{
public:
    Terrain(NoiseMap* nMap);
    virtual ~Terrain();

	void genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void genericRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);


protected:

private:
    NoiseMap* nMap;
    std::vector<TileType> worldTiles;
};
