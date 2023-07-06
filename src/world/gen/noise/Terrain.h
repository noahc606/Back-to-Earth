#pragma once
#include "TileRegion.h"

class Terrain
{
public:
    Terrain();
    virtual ~Terrain();

    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);

protected:

private:
};
