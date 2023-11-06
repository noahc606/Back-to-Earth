#pragma once
#include "TileRegion.h"

class Terrain
{
public:
    Terrain();
    virtual ~Terrain();

	void testRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural);
	void testRegion(TileRegion& tr, int rX, int rY, int rZ);
    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);

protected:

private:
};
