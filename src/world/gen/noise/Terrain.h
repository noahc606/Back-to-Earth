#pragma once
#include "TileRegion.h"

class Terrain
{
public:
    Terrain();
    virtual ~Terrain();

	void genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void genericRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);

protected:

private:
    std::vector<TileType> worldTiles;
    float vScale0, vScale1;
    float tZoom, mZoom, lZoom, cZoom;
};
