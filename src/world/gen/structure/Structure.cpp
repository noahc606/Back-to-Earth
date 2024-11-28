#include "Structure.h"
#include <nch/cpp-utils/log.h>
#include "CollectionUtils.h"
#include "ShapeBuilder.h"
#include "TileIterator.h"


bool Structure::suppressWarnings = false;

Structure::Structure(int id, Vec3X<int64_t> origin, TileDict* tileDict)
{
    /* Preparations */
    //Constructor
    Structure::id = id;
    Structure::origin = origin;
    Structure::tileDict = tileDict;
   //Set the bounds of the structure based on its maximum size AND THEN build regMap.
    switch(id) {
        case HAB:           { bounds = Vec3X<int64_t>(16, 16, 16); } break;
        case CAVE_LARGE:    { bounds = Vec3X<int64_t>(128, 128, 128); } break;
        default:            { bounds = Vec3X<int64_t>(1, 4, 9); } break;
    }
    buildRegMap();

    /* Based on structure ID, populate the regions in 'regMap' with the proper tiles */
    ShapeBuilder sb(&regMap);
    sb.seedRandFromPos(origin);
    switch(id) {
        case HAB: {
            Tile t0 = tileDict->at("hab_titanium_hull");
            Tile t1 = tileDict->at("artificial_air");
            
            TileMap::setTiles(&regMap, 0, 0, 0, 15, 15, 5, t0, true);
            TileMap::setTiles(&regMap, 1, 1, 1, 14, 14, 4, t1, true);
        } break;
        case CAVE_LARGE: {
            Tile t0 = tileDict->at("accrio_air");
            sb.carveOrganicTunnel(Vec3X<int64_t>(64, 64, 64), 63, 8, t0);
        } break;
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
    while(regMap.size()>0) { regMap.erase(regMap.begin()); }
}

int Structure::getID() { return id; }
Vec3X<int64_t> Structure::getOrigin() { return origin; }
Vec3X<int64_t> Structure::getBounds() { return bounds; }
Box3X<int64_t> Structure::getBoundingBox() { return Box3X<int64_t>(origin.x, origin.y, origin.z, origin.x+bounds.x-1, origin.y+bounds.y-1, origin.z+bounds.z-1); }
std::map<Defs::t_tripleI64, TileRegion>* Structure::getRegionMap() { return &regMap; }
void Structure::suppressNextWarning() { suppressWarnings = true; }

void Structure::buildRegMap()
{
    /*
    Build structure regions:
        Structure's volume is defined by its rectangular-prism 'bnds' (bounds) within the world.
        The structure's tiles are completely within the volume inside of this 'regMap'.
        The regions(s) within this 'regMap' start at the structure volume's (minX, minY, minZ) and expand outward until the region(s) contain the whole volume.
    */
    //CollectionUtils tool
    CollectionUtils cu;    
    //Based on size of bnds, build the appropriate number of regions
    for(int64_t iRX = 0; iRX<=bounds.x/32; iRX++)
    for(int64_t iRY = 0; iRY<=bounds.y/32; iRY++)
    for(int64_t iRZ = 0; iRZ<=bounds.z/32; iRZ++) {
        TileRegion tr(tileDict);
        cu.insertInMap(regMap, std::make_pair(std::make_tuple(iRX, iRY, iRZ), tr));
    }
}