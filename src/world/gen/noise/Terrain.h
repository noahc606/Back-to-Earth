#pragma once
#include "TileDict.h"
#include "TileRegion.h"
#include "NoiseMap.h"

class Terrain
{
public:
    Terrain(NoiseMap* nMap, TileDict* tDict);
    virtual ~Terrain();

	void genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void genericRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);


protected:

private:
    NoiseMap* nMap;
    TileDict* tDict;
};
