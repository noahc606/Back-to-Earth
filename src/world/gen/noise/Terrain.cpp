#include "Terrain.h"
#include "Noise.h"

Terrain::Terrain(int64_t seed)
{
    TileType tt;
    tt.init();
    
    Terrain::seed = seed;

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

    //Height scale
    vScale0 = 32.0;
    vScale1 = 128.0;
    
    //Sizes of noise
    tZoom = 64.0;           //Tiny noise: Small variations
    mZoom = 256.0;          //Medium noise: Hills, valleys, and ponds
    lZoom = 2048.0;         //Large noise: Mountains, large valleys and seas
    cZoom = 16.0*32768.0;   //Continental Noise: Continents and oceans
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

    float tNoise = 0.0;
    float mNoise = 0.0;
    float lNoise = 0.0;
    float cNoise = 0.0;
    
    float noise3d = 0.0;

    //ND: Noise depth.
    int nd = 0;
    //LD: Local depth. Based on noise depth but within a region.
    int ld = 0;

    Noise n(seed);

    for( char sx = 0; sx<32; sx++ ) {
        for( char sy = 0; sy<32; sy++ ) {

            //Calculate noise components at this location
            tNoise = n.clampedNoise2D((x+sx)/tZoom,(y+sy)/tZoom)*vScale0;
            mNoise = n.clampedNoise2D((x+sx)/mZoom,(y+sy)/mZoom)*vScale0;
            lNoise = n.noise2D((x+sx)/lZoom,(y+sy)/lZoom)*vScale0;
            cNoise = n.noise2D((x+sx)/cZoom,(y+sy)/cZoom)*vScale1;

            //Calculate total noise depth.
            nd = -z-tNoise-mNoise-lNoise-cNoise;

            for( int sz = 0; sz<32; sz++ ) {
                ld = sz-nd;
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

            /*
            for(int sz = 0; sz<16; sz++) {
                noise3d = vScale0 * n.clampedSNoise3D( (x+sx)/tZoom, (y+sy)/tZoom, (z+2*sz)/tZoom );
                if(noise3d<8) {
                    tr.setTile(sx, sy, sz*2+1, gravel);
                    tr.setTile(sx, sy, sz*2, gravel);
                }
            }*/
        }
    }
}

void Terrain::genericRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, false); }
void Terrain::populateRegion(TileRegion& tr, int rX, int rY, int rZ) { genericRegion(tr, rX, rY, rZ, true); }