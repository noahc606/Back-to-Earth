#pragma once
#include "Box3X.h"
#include "Defs.h"
#include "TileRegion.h"

class Structure
{
public:
    enum IDs {
        MONOLITH = -999,
        CRASHED_SHIP = 2,  
    };

    Structure(int id, Point3X<int64_t> origin);
    ~Structure();

    int getID();
    Box3X<int64_t> getBounds();
    std::map<Defs::t_tripleI64, TileRegion>* getRegionMap();
private:

    int id = -1;
    Box3X<int64_t> bounds;
    std::map<Defs::t_tripleI64, TileRegion> regMap;

    static bool suppressWarnings;
};
