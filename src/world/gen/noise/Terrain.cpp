#include "Terrain.h"
#include "Noise.h"

Terrain::Terrain()
{
    //ctor
}

Terrain::~Terrain()
{
    //dtor
}

void Terrain::testRegion(TileRegion& tr, int rX, int rY, int rZ, bool natural)
{
    float x = rX*32;
    float y = rY*32;
    float z = rZ*32;

    TileType tt; tt.init();
	int air = tr.addToPalette(tt);
	
    tt.setVisionBlocking(true); tt.setTextureXY(1, 5); tt.setRGB(204, 153, 0); tt.setSolid(true);
    int regolith = tr.addToPalette(tt, natural);

    tt.setVisionBlocking(true); tt.setTextureXY(3, 1); tt.setRGB(128, 50, 0); tt.setSolid(true);
    int soil = tr.addToPalette(tt, natural);

    tt.setVisionBlocking(true); tt.setTextureXY(2, 1); tt.setRGB(240, 240, 240); tt.setSolid(true);
    int rock = tr.addToPalette(tt, natural);

    tt.setVisionBlocking(true); tt.setTextureXY(0, 4); tt.setRGB(10, 80, 180); tt.setSolid(false);
    int water = tr.addToPalette(tt, natural);

    //Height scale
    float verticalScaling = 32.0;
    float verticalScaling2 = 128.0;

    //Tiny noise: Small variations
    float tNoise = 0.0; float tZoom = 64.0;
    //Medium noise: Small hills, valleys, and ponds
    float mNoise = 0.0; float mZoom = 256.0;
    //Large noise: Mountains, large valleys and seas
    float lNoise = 0.0; float lZoom = 2048.0;
    //Continental Noise: Continents and oceans
    float cNoise = 0.0; float cZoom = 16.0*32768.0;

    //ND: noise depth
    int nd = 0;
    //LD: Local depth. Based on noise depth but within a region.
    int ld = 0;

    for( char sx = 0; sx<32; sx++ ) {
        for( char sy = 0; sy<32; sy++ ) {

            //Calculate noise components at this location
            tNoise = Noise::clampedNoise2D((x+sx)/tZoom,(y+sy)/tZoom)*verticalScaling;
            mNoise = Noise::clampedNoise2D((x+sx)/mZoom,(y+sy)/mZoom)*verticalScaling;
            lNoise = Noise::noise2D((x+sx)/lZoom,(y+sy)/lZoom)*verticalScaling;
            cNoise = Noise::noise2D((x+sx)/cZoom,(y+sy)/cZoom)*verticalScaling2;

            //Calculate total noise depth.
            nd = -z-tNoise-mNoise-lNoise-cNoise;

            for( int sz = 0; sz<32; sz++ ) {
                ld = sz-nd;

                if(ld>5) {
                    tr.setTile(sx, sy, sz, rock);
                } else
                if(ld>0) {
                    tr.setTile(sx, sy, sz, soil);
                } else
                if(ld==0) {
                    tr.setTile(sx, sy, sz, regolith);
                } else {

                    if( z+sz>-20 ) {
                        tr.setTile(sx, sy, sz, water);
                    } else {
                        tr.setTile(sx, sy, sz, air);
                    }
                }
            }
        }
    }
}

void Terrain::testRegion(TileRegion& tr, int rX, int rY, int rZ) { testRegion(tr, rX, rY, rZ, false); }
void Terrain::populateRegion(TileRegion& tr, int rX, int rY, int rZ) { testRegion(tr, rX, rY, rZ, true); }