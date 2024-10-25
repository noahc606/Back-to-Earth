#pragma once
#include "Box3X.h"
#include "Defs.h"
#include "TileDict.h"
#include "TileRegion.h"

class Structure
{
public:
    enum IDs {
        MONOLITH = -999,
        CRASHED_SHIP = 2,  
    };

    Structure(int id, Point3X<int64_t> origin, TileDict* tileDict);
    ~Structure();

    int getID();
    Point3X<int64_t> getOrigin();
    Box3X<int64_t> getBounds();
    std::map<Defs::t_tripleI64, TileRegion>* getRegionMap();
    static void suppressNextWarning();

private:

    int id = -1;
    Point3X<int64_t> origin;
    Box3X<int64_t> bnds;    //Bounds
    std::map<Defs::t_tripleI64, TileRegion> regMap;
    TileDict* tileDict;

    static bool suppressWarnings;
};
