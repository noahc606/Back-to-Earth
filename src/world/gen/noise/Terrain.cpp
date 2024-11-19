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

    //From (stored or newly computed) heightmap, set tiles at this (rX, rY, rZ).
    for( char sx = 0; sx<32; sx++ ) {
        for( char sy = 0; sy<32; sy++ ) {
            int64_t nd = -z+heightMap->at(sx).at(sy);

            for( int sz = 0; sz<32; sz++ ) {
                int64_t ld = sz-nd;
                //https://opentextbc.ca/geology/chapter/9-2-the-temperature-of-earths-interior/
                //Divide real-life depth distances by 60. A tile is 1x1x1 meters.
                if(ld>3400) {                       //Deep into mantle
                    tr.setTile(sx, sy, sz, rock);
                } else if(ld>1700) {                //Asthenosphere (liquid-ish)
                    tr.setTile(sx, sy, sz, magma);
                } else if(ld>20) {                   //Crust 2
                    tr.setTile(sx, sy, sz, copper_1);
                } else if(ld>5) {                   //Crust
                    tr.setTile(sx, sy, sz, rock);
                } else
                if(ld>0) {
                    tr.setTile(sx, sy, sz, soil);
                } else
                if(ld==0) {
                    tr.setTile(sx, sy, sz, regolith);
                } else {

                    if( z+sz>-20 ) {
                        tr.setTile(sx, sy, sz, silt);
                    } else {
                        tr.setTile(sx, sy, sz, air);
                    }
                }
            }
        }
    }

    
}

void Terrain::genericRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, false); }
void Terrain::populateRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, true); }