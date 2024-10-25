#include "Structure.h"
#include <nch/cpp-utils/log.h>
#include "CollectionUtils.h"
#include "TileIterator.h"

bool Structure::suppressWarnings = false;

Structure::Structure(int id, Point3X<int64_t> origin, TileDict* tileDict)
{
    Structure::id = id;
    Structure::origin = origin;
    Structure::tileDict = tileDict;



    
    /* Build structure bounds */
    //Get origin (ox, oy, oz);
    int64_t ox = origin.x;
    int64_t oy = origin.y;
    int64_t oz = origin.z;
    //Based on origin, set the 'hard' bounds of the structure
    switch(id) {
        case CRASHED_SHIP: {
            bnds.c1 = Point3X<int64_t>(ox, oy, oz);
            bnds.c2 = Point3X<int64_t>(ox+15, oy+15, oz+6);
        } break;
        default: {
            bnds.c1 = Point3X<int64_t>(ox, oy, oz);
            bnds.c2 = Point3X<int64_t>(ox, oy+3, oz+8);
        } break;
    }


    /*
    Build structure regions:
        Structure's volume is defined by its rectangular-prism 'bnds' (bounds) within the world.
        The structure's tiles are completely within the volume and are defined by 'tileMap'.
        The regions(s) within this 'tileMap' start at the structure volume's (minX, minY, minZ) and expand outward until the region(s) contain the whole volume.
    */
    //CollectionUtils tool
    CollectionUtils cu;    
    //Based on size of bnds, build the appropriate number of regions
    for(int64_t iRX = 0; iRX<=std::abs(bnds.c1.x-bnds.c2.x)/32; iRX++)
    for(int64_t iRY = 0; iRY<=std::abs(bnds.c1.x-bnds.c2.x)/32; iRY++)
    for(int64_t iRZ = 0; iRZ<=std::abs(bnds.c1.x-bnds.c2.x)/32; iRZ++) {
        TileRegion tr(tileDict);
        cu.insertInMap(regMap, std::make_pair(std::make_tuple(iRX, iRY, iRZ), tr));
    }
    //Based on structure ID, populate the regions with the proper tiles
    switch(id) {
        //Crash-landed ship
        case CRASHED_SHIP: {
            //Metal
            Tile t0 = tileDict->at("hab_titanium_hull");
            Tile t1 = tileDict->at("breathable_air");
            
            TileMap::setTiles(&regMap, 0, 0, 0, 16, 16, 5, t0, true);
            TileMap::setTiles(&regMap, 1, 1, 1, 15, 15, 4, t1, true);
        } break;

        //Unknown structure - causes a Monolith to spawn
        default: {
            if(!suppressWarnings) nch::Log::warnv(__PRETTY_FUNCTION__, "using placeholder", "Tried to build structure with unknown ID '%d'", id);
            
            Tile x = tileDict->at("monolith");
            TileMap::setTiles(&regMap, 0, 0, 0, 0, 3, 8, x, true);
            suppressWarnings = false;
        } break;
    }
}

Structure::~Structure()
{
    while(regMap.size()>0) {
        regMap.erase(regMap.begin());
    }
}

int Structure::getID() { return id; }
Point3X<int64_t> Structure::getOrigin() { return origin; }
Box3X<int64_t> Structure::getBounds() { return bnds; }
std::map<Defs::t_tripleI64, TileRegion>* Structure::getRegionMap() { return &regMap; }
void Structure::suppressNextWarning() { suppressWarnings = true; }