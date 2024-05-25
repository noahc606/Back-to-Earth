#pragma once
#include "TileRegion.h"

class Noise
{
public:
    /**/
    typedef long long t_ll;
    struct vec2D { float x, y; };
    struct vec3D { float x, y, z; };

    /**/
    Noise(int64_t seed);
    Noise(std::string seed);
    virtual ~Noise();
    /**/
    //Utils
    static inline int32_t fastfloor(float fp);
    static int64_t stringSeedToI64Seed(std::string s);
    static float interpolate(float a0, float a1, float w);
    //Hashing functions
    unsigned hash2ToUint(int ix, int iy);
    uint8_t hash1ToUint8(int val);
    static float hashedGI3dToF(int32_t hash, float x, float y, float z);
    //Gradient generation
    vec2D gradient2D(int ix, int iy);
    float dotGridGradient2D(int ix, int iy, float x, float y);
    //Noise generation
    float noise2D(float x, float y);
    float sNoise3D(float x, float y, float z);
    float clampedNoise2D(float x, float y);
    float clampedSNoise3D(float x, float y, float z);
    /**/
    /**/
protected:

private:
    unsigned gHashSeed = 1;
};
