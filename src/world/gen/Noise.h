#pragma once
#include "TileRegion.h"

class Noise
{
public:
    typedef long long t_ll;
    struct vec2D {
        float x; float y;
    };

    Noise();
    virtual ~Noise();

    float interpolate(float a0, float a1, float w);
    vec2D gradient2D(int ix, int iy);
    float dotGridGradient2D(int ix, int iy, float x, float y);
    float noise2D(float x, float y);
    float clampedNoise2D(float x, float y);

    void populateRegion(TileRegion& tr, int rX, int rY, int rZ);

protected:

private:
};
