#include "Terrain.h"
#include "Noise.h"

Terrain::Terrain(NoiseMap* nMap)
{
    TileType tt;
    tt.init();
    
    Terrain::nMap = nMap;

    //Air
    worldTiles.push_back(tt);
	
    //Regolith
    tt.setVisionBlocking(true); tt.setTextureXY(1, 5); tt.setRGB(204, 153, 0); tt.setSolid(true);
    worldTiles.push_back(tt);

    //Soil
    tt.setVisionBlocking(true); tt.setTextureXY(3, 1); tt.setRGB(128, 50, 0); tt.setSolid(true);
    worldTiles.push_back(tt);

    //Rock
    tt.setVisionBlocking(true); tt.setTextureXY(2, 1); tt.setRGB(190, 150, 100); tt.setSolid(true);
    worldTiles.push_back(tt);

    //Gravel
    tt.setVisionBlocking(true); tt.setTextureXY(2, 5); tt.setRGB(110, 90, 40); tt.setSolid(true);
    worldTiles.push_back(tt);

    //Magma
    tt.setVisionBlocking(true); tt.setTextureXY(1, 4); tt.setRGB(255, 40, 40); tt.setSolid(false);
    worldTiles.push_back(tt);
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

    int air = tr.addToPalette(worldTiles[0]);
    int regolith = tr.addToPalette(worldTiles[1], natural);
    int soil = tr.addToPalette(worldTiles[2], natural);
    int rock = tr.addToPalette(worldTiles[3], natural);
    int gravel = tr.addToPalette(worldTiles[4], natural);
    int magma = tr.addToPalette(worldTiles[5], natural);

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
                        tr.setTile(sx, sy, sz, gravel);
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