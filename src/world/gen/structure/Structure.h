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
        HAB = 2,
        CAVE_SMALL,
        CAVE_MEDIUM,
        CAVE_LARGE,
    };

    Structure(int id, Vec3X<int64_t> origin, TileDict* tileDict);
    ~Structure();

    int getID();
    Vec3X<int64_t> getOrigin();
    Vec3X<int64_t> getBounds();
    Box3X<int64_t> getBoundingBox();
    std::map<Defs::t_tripleI64, TileRegion>* getRegionMap();
    static void suppressNextWarning();

private:

    void buildRegMap();

    int id = -1;
    Vec3X<int64_t> origin;
    Vec3X<int64_t> bounds;    //Bounds of this structure represented by XYZ dimensions. {a, b, c} represents a a*b*c box.
    size_t bitsize;
    std::map<Defs::t_tripleI64, TileRegion> regMap;
    TileDict* tileDict;

    static bool suppressWarnings;
};
