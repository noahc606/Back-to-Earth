#include "Structure.h"
#include <nch/cpp-utils/io/Log.h>
#include "CollectionUtils.h"
#include "TileIterator.h"

bool Structure::suppressWarnings = false;

Structure::Structure(int id, Point3X<int64_t> origin)
{
    Structure::id = id;



    
    /* Build structure bounds */
    //Get origin (ox, oy, oz);
    int64_t ox = origin.x;
    int64_t oy = origin.y;
    int64_t oz = origin.z;
    //Based on origin, set the 'hard' bounds of the structure
    switch(id) {
        case CRASHED_SHIP: {
            bounds.c1 = Point3X<int64_t>(ox, oy, oz);
            bounds.c2 = Point3X<int64_t>(ox+63, oy+47, oz+23);
        } break;
        default: {
            nch::Log::warnv(__PRETTY_FUNCTION__, "using placeholder", "Tried to build bounds of structure with unknown ID '%d'", id);
            bounds.c1 = Point3X<int64_t>(ox, oy, oz);
            bounds.c2 = Point3X<int64_t>(ox, oy+3, oz+8);
        } break;
    }


    /*
    Build structure regions:
        Structure's volume is defined by its rectangular-prism 'bounds' within the world.
        The structure's tiles are completely within the volume and are defined by 'tileMap'.
        The regions(s) within this 'tileMap' start at the structure volume's (minX, minY, minZ) and expand outward until the region(s) contain the whole volume.
    */
    //CollectionUtils tool
    CollectionUtils cu;    
    //Based on size of bounds, build the appropriate number of regions
    for(int64_t iRX = 0; iRX<std::abs(bounds.c1.x-bounds.c2.x)/32+1; iRX++)
    for(int64_t iRY = 0; iRY<std::abs(bounds.c1.x-bounds.c2.x)/32+1; iRY++)
    for(int64_t iRZ = 0; iRZ<std::abs(bounds.c1.x-bounds.c2.x)/32+1; iRZ++) {
        TileRegion tr;
        cu.insertInMap(regMap, std::make_pair(std::make_tuple(iRX, iRY, iRZ), tr));
    }
    //Based on structure ID, populate the regions with the proper tiles
    switch(id) {
        //Crash-landed ship
        case CRASHED_SHIP: {
            TileType tt0; tt0.init(); tt0.setRGB(255, 255, 255); tt0.setSolid(true); tt0.setTextureXY(1, 3); tt0.setVisionBlocking(true);
            TileType tt1; tt1.init(); tt1.setRGB(0, 0, 255); tt1.setSolid(false); tt0.setVisionBlocking(false);
            //TileMap::setTile(regMap, 0, 0, 0, tt)
            //fill(0, 0, 23, 63, 47, 23)
            TileRegion* tr0 = cu.findInMap(regMap, std::make_tuple((int64_t)0, (int64_t)0, (int64_t)0));
            int16_t id0 = tr0->addToPalette(tt0, true); //Metal tiling
            int16_t id1 = tr0->addToPalette(tt1, true); //Air
            tr0->setTiles(15, 15, 11, 31, 31, 15, id0);
            tr0->setTiles(16, 16, 12, 30, 30, 14, id1);
        } break;

        //Unknown structure - causes a Monolith to spawn
        default: {
            if(!suppressWarnings) nch::Log::warnv(__PRETTY_FUNCTION__, "using placeholder", "Tried to build structure with unknown ID '%d'", id);

            TileRegion* tr = cu.findInMap(regMap, std::make_tuple((int64_t)0, (int64_t)0, (int64_t)0));
            if(tr!=nullptr) {
                TileType tt;
                tt.init();
                tt.setRGB(50, 50, 50); tt.setSolid(true); tt.setTextureXY(2, 3); tt.setVisionBlocking(true);
                int16_t tti = tr->addToPalette(tt, true);

                for(int x = 0; x<1; x++)
                for(int y = 0; y<4; y++)
                for(int z = 0; z<9; z++) {
                    tr->setTile(x, y, z, tti);
                }
            }
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
Box3X<int64_t> Structure::getBounds() { return bounds; }
std::map<Defs::t_tripleI64, TileRegion>* Structure::getRegionMap() { return &regMap; }