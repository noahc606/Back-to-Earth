/**
    NOTE:

    Most code in this class was taken from
    https://en.wikipedia.org/wiki/Perlin_noise#Implementation
*/

#include "Noise.h"
#include <math.h>

Noise::Noise(){}
Noise::~Noise(){}

float Noise::interpolate(float a0, float a1, float w) {

    //if (0.0 > w) return a0;
    //if (1.0 < w) return a1;

    //Default
    //return (a1-a0)*w+a0;

    //Smoothstep
    return (a1-a0)*(3.0-w*2.0)*(w*w)+a0;

    //Smootherstep
    //return (a1 - a0) * ((w * (w * 6.0 - 15.0) + 10.0) * w * w * w) + a0;
}

Noise::vec2D Noise::gradient2D(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const unsigned w = 8 * sizeof(unsigned);
    const unsigned s = w / 2; // rotation width
    unsigned a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> (w-s);
    b *= 1911520717; a ^= b << s | b >> (w-s);
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    vec2D v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

/**
    Grid coordinates = (ix, iy). ix and iy are both any integer value.
    Gradient vector components = (x, y). (x, y) are the components of a unit vector.
*/
float Noise::dotGridGradient2D(int ix, int iy, float x, float y) {
    //Get gradient from integer coordinates
    vec2D gradient = gradient2D(ix, iy);

    //Compute distance vector
    float dx = x-(float)ix;
    float dy = y-(float)iy;

    //Dot product
    float dot = (dx*gradient.x + dy*gradient.y);
    return dot;
}

float Noise::noise2D(float x, float y)
{
    // Determine grid cell coordinates
    int x0 = (int)floor(x);
    int x1 = x0 + 1;
    int y0 = (int)floor(y);
    int y1 = y0 + 1;

    //std::cout << "From (" << x0 << ", " << y0 << ") to (" << x1 << ", " << y1 << ");\n";

    // Determine interpolation weights
    // Could also use higher order polynomial/s-curve here
    float sx = x-(float)x0;
    float sy = y-(float)y0;

    //std::cout << "Weights: (sx, sy)=(" << sx << ", " << sy << ")\n";

    // Interpolate between grid point gradients
    float n0, n1, ix0, ix1, value;

    n0 = dotGridGradient2D(x0, y0, x, y);
    n1 = dotGridGradient2D(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient2D(x0, y1, x, y);
    n1 = dotGridGradient2D(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    value = interpolate(ix0, ix1, sy);
    return value; // Will return in range -1 to 1. To make it in range 0 to 1, multiply by 0.5 and add 0.5
}

float Noise::clampedNoise2D(float x, float y)
{
    return noise2D(x, y)*0.5d+0.5d;
}


void Noise::populateRegion(TileRegion& tr, int rX, int rY, int rZ)
{
    float x = rX*32;
    float y = rY*32;
    float z = rZ*32;

    TileType tt; tt.init();

    tt.setVisionBlocking(false); tt.setTextureXY(0, 0); tt.setSolid(false);
    int air = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true); tt.setTextureXY(3, 5); tt.setRGB(51, 255, 187); tt.setSolid(true);
    int topsoil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true); tt.setTextureXY(3, 1); tt.setRGB(128, 50, 0); tt.setSolid(true);
    int soil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true); tt.setTextureXY(2, 1); tt.setRGB(240, 240, 240); tt.setSolid(true);
    int rock = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true); tt.setTextureXY(0, 4); tt.setRGB(10, 80, 180); tt.setSolid(false);
    int water = tr.addToPaletteFast(tt);

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

    for( int sx = 0; sx<32; sx++ ) {
        for( int sy = 0; sy<32; sy++ ) {

            //Calculate noise components at this location
            tNoise = clampedNoise2D((x+sx)/tZoom,(y+sy)/tZoom)*verticalScaling;
            mNoise = clampedNoise2D((x+sx)/mZoom,(y+sy)/mZoom)*verticalScaling;
            lNoise = noise2D((x+sx)/lZoom,(y+sy)/lZoom)*verticalScaling;
            cNoise = noise2D((x+sx)/cZoom,(y+sy)/cZoom)*verticalScaling2;

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
                    tr.setTile(sx, sy, sz, topsoil);
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
