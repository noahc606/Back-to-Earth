#include "Terrain.h"
#include "Noise.h"

Terrain::Terrain(NoiseMap* nMap, TileDict* tDict)
{
    Terrain::nMap = nMap;
    Terrain::tDict = tDict;
}

Terrain::~Terrain()
{
    //dtor
}

void Terrain::genericRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural)
{
    float x = rX*32;
    float y = rY*32;
    float z = rZ*32;

    int air = tr.addToPalette(tDict->at("accrio_air"), natural);
    int copper_1 = tr.addToPalette(tDict->at("accrio_native_copper_1"), natural);
    int regolith = tr.addToPalette(tDict->at("accrio_regolith"), natural);
    int soil = tr.addToPalette(tDict->at("accrio_soil"), natural);
    int rock = tr.addToPalette(tDict->at("accrio_rock"), natural);
    int silt = tr.addToPalette(tDict->at("accrio_silt"), natural);
    int magma = tr.addToPalette(tDict->at("magma"), natural);

    float noise3d = 0.0;

    auto heightMap = nMap->cachedBaseTerrainHeightMapAt(rX, rY)->heightMap;

    std::srand(nMap->getNoise()->hash3ToUint32(rX, rY, rZ));

    //From (stored or newly computed) heightmap, set tiles at this (rX, rY, rZ).
    for( char sx = 0; sx<32; sx++ ) {
        for( char sy = 0; sy<32; sy++ ) {
            int64_t nd = -z+heightMap->at(sx).at(sy);

            for( int sz = 0; sz<32; sz++ ) {
                int64_t ld = sz-nd;
                //https://opentextbc.ca/geology/chapter/9-2-the-temperature-of-earths-interior/
                //Divide real-life depth distances by 60. A tile is 1x1x1 meters.
                //Deep into mantle (rock again)
                if(ld>3400) {
                    tr.setTile(sx, sy, sz, rock);
                //Asthenosphere (liquid-ish)
                } else if(ld>1700) {
                    tr.setTile(sx, sy, sz, magma);
                //Main Crust (all rock)
                } else if(ld>20) {
                    tr.setTile(sx, sy, sz, rock);
                //Top Crust (rock and soil)
                } else if(ld>5) {
                    //6-20 -> 1/16 chance - 15/16 chance to be rock
                    if((rand()%16)<=ld-6) {
                        tr.setTile(sx, sy, sz, rock);
                    } else {
                        tr.setTile(sx, sy, sz, soil);
                    }
                
                //Soil
                } else if(ld>0) {
                    tr.setTile(sx, sy, sz, soil);
                //Surface regolith
                } else if(ld==0) {
                    tr.setTile(sx, sy, sz, regolith);
                //Above surface
                } else {
                    if(z+sz>-20)    { tr.setTile(sx, sy, sz, silt); }   //Below "sea level"
                    else            { tr.setTile(sx, sy, sz, air); }    //Above "sea level"
                }
            }
        }
    }

    
}

void Terrain::genericRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, false); }
void Terrain::populateRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, true); }