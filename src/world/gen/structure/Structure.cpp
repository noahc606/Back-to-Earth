#include "Structure.h"
#include "CollectionUtils.h"
#include "Log.h"

bool Structure::suppressWarnings = true;

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
        default: {
            if(!suppressWarnings) Log::warnv(__PRETTY_FUNCTION__, "using placeholder", "Tried to build bounds of structure with unknown ID '%d'", id);
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
        case 2: {

        }

        //Unknown structure - causes a Monolith to spawn
        default: {
            if(!suppressWarnings) Log::warnv(__PRETTY_FUNCTION__, "using placeholder", "Tried to build structure with unknown ID '%d'", id);

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


Box3X<int64_t> Structure::getBounds() { return bounds; }

std::map<Defs::t_tripleI64, TileRegion>* Structure::getRegionMap() { return &regMap; }