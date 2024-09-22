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
    Point3X<int64_t> getOrigin();
    Box3X<int64_t> getBounds();
    std::map<Defs::t_tripleI64, TileRegion>* getRegionMap();
private:

    int id = -1;
    Point3X<int64_t> origin;
    Box3X<int64_t> bnds;    //Bounds
    std::map<Defs::t_tripleI64, TileRegion> regMap;

    static bool suppressWarnings;
};
