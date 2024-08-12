#pragma once
#include "Box3X.h"
#include "Defs.h"
#include "TileRegion.h"

class Structure
{
public:
    Structure(int id, Point3X<int64_t> origin);
    ~Structure();

    Box3X<int64_t> getBounds();
    std::map<Defs::t_tripleI64, TileRegion>* getRegionMap();
private:

    int id = -1;
    Box3X<int64_t> bounds;
    std::map<Defs::t_tripleI64, TileRegion> regMap;

    static bool suppressWarnings;
};
