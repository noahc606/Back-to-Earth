/**
    Many functions within this file were taken from:
    - https://en.wikipedia.org/wiki/Perlin_noise#Implementation
    - https://github.com/SRombauts/SimplexNoise/blob/master/src/SimplexNoise.cpp
*/

#include "Noise.h"
#include <cstdint>
#include <math.h>

static const uint8_t perm[256] = {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

Noise::Noise(int64_t seed)
{
    //Set randGradientFactor based on seed
    if(seed!=0) {
        srand(seed);
        gHashSeed = rand();
        if(gHashSeed==0) {
            gHashSeed = 1;
        }
    }
}

Noise::Noise(std::string seed):
Noise( stringSeedToI64Seed(seed) ){}
Noise::~Noise(){}

inline int32_t Noise::fastfloor(float fp) {
    int32_t i = static_cast<int32_t>(fp);
    return (fp<i) ? (i-1) : (i);
}

int64_t Noise::stringSeedToI64Seed(std::string s)
{
    int64_t val = INT64_MIN;
    int strIndex = 0;
    for( char c : s ) {
        val += c*pow(2, strIndex);
        strIndex++;
    }

    return val;
}


float Noise::interpolate(float a0, float a1, float w) {
    return (a1-a0)*(3.0-w*2.0)*(w*w)+a0;
}

uint32_t Noise::hash2ToUint32(int ix, int iy)
{
    //Copy ix, iy into a, b
    uint32_t a = ix;
    uint32_t b = iy;

    //Return random unsigned value (a) based on ix, iy, and gHashSeed.
    //For any input (ix, iy, gHashSeed), the same output will be returned every single time. 
    const uint32_t w = 8*sizeof(uint32_t);
    const uint32_t s = w/2; //rotation width
    a *= 3284157443; b ^= a<<s|a>>(w-s);
    b *= 1911520717; a ^= b<<s|b>>(w-s);
    a *= 2048419325;
    a *= gHashSeed;

    //Return value
    return a;
}

uint32_t Noise::hash3ToUint32(int ix, int iy, int iz)
{
    return hash2ToUint32((int)hash2ToUint32(ix, iy), iz);
}

uint8_t Noise::hash1ToUint8(int val) {
    return perm[static_cast<uint8_t>(val)];
}

float Noise::hashedGI3dToF(int32_t hash, float x, float y, float z) {
    int h = hash & 15;                                  // Convert low 4 bits of hash code into 12 simple
    float u = h < 8 ? x : y;                            // gradient directions, and compute dot product.
    float v = h < 4 ? y : h == 12 || h == 14 ? x : z;   // Fix repeats at h = 12 to 15
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

Noise::vec2D Noise::gradient2D(int ix, int iy) {    
    //Generate random value within [0, 2*Pi]
    double random = hash2ToUint32(ix, iy)*(3.14159265/~(~0u>>1));
    
    //Take random value as an angle, return corresponding 2D vector
    vec2D v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

/**
    Grid coordinates = (ix, iy). ix and iy are both any integer value.
    Gradient vector components = (x, y). (x, y) are the components of a unit vector.
*/
float Noise::dotGridGradient2D(int32_t ix, int32_t iy, float x, float y) {
    //Get gradient from integer coordinates
    vec2D gradient = gradient2D(ix, iy);

    //Compute distance vector
    float dx = x-(float)ix;
    float dy = y-(float)iy;

    //Dot product
    float dot = (dx*gradient.x + dy*gradient.y);
    return dot;
}

/*
    2D Perlin Noise generator
    - Mostly taken from https://en.wikipedia.org/wiki/Perlin_noise#Implementation
*/
float Noise::noise2D(float x, float y)
{
    //Grid cell coordinates
    int32_t x0 = fastfloor(x); int32_t x1 = x0+1;
    int32_t y0 = fastfloor(y); int32_t y1 = y0+1;

    //Interpolation weights
    float sx = x-(float)x0;
    float sy = y-(float)y0;

    //Interpolate between grid point gradients
    float n0, n1, ix0, ix1;
    n0 = dotGridGradient2D(x0, y0, x, y);
    n1 = dotGridGradient2D(x1, y0, x, y);
    ix0 = interpolate(n0, n1, sx);

    n0 = dotGridGradient2D(x0, y1, x, y);
    n1 = dotGridGradient2D(x1, y1, x, y);
    ix1 = interpolate(n0, n1, sx);

    // Will return a value within [-1, 1].
    return interpolate(ix0, ix1, sy);
}

/*
    3D Simplex noise generator
    - Mostly taken from https://github.com/SRombauts/SimplexNoise/blob/master/src/SimplexNoise.cpp
*/
float Noise::sNoise3D(float x, float y, float z)
{
    float n0, n1, n2, n3; // Noise contributions from the four corners

    // Skewing/Unskewing factors for 3D
    static const float F3 = 1.0f / 3.0f;
    static const float G3 = 1.0f / 6.0f;

    // Skew the input space to determine which simplex cell we're in
    float s = (x+y+z)*F3; // Very nice and simple skew factor for 3D
    int i = fastfloor(x+s);
    int j = fastfloor(y+s);
    int k = fastfloor(z+s);
    float t = (i+j+k)*G3;
    float X0 = i-t; // Unskew the cell origin back to (x,y,z) space
    float Y0 = j-t;
    float Z0 = k-t;
    float x0 = x-X0; // The x,y,z distances from the cell origin
    float y0 = y-Y0;
    float z0 = z-Z0;

    // For the 3D case, the simplex shape is a slightly irregular tetrahedron.
    // Determine which simplex we are in.
    int i1, j1, k1; // Offsets for second corner of simplex in (i,j,k) coords
    int i2, j2, k2; // Offsets for third corner of simplex in (i,j,k) coords
    if (x0>=y0) {
        if (y0>=z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // X Y Z order
        } else if (x0 >= z0) {
            i1 = 1; j1 = 0; k1 = 0; i2 = 1; j2 = 0; k2 = 1; // X Z Y order
        } else {
            i1 = 0; j1 = 0; k1 = 1; i2 = 1; j2 = 0; k2 = 1; // Z X Y order
        }
    } else { // x0<y0
        if (y0<z0) {
            i1 = 0; j1 = 0; k1 = 1; i2 = 0; j2 = 1; k2 = 1; // Z Y X order
        } else if (x0<z0) {
            i1 = 0; j1 = 1; k1 = 0; i2 = 0; j2 = 1; k2 = 1; // Y Z X order
        } else {
            i1 = 0; j1 = 1; k1 = 0; i2 = 1; j2 = 1; k2 = 0; // Y X Z order
        }
    }

    // A step of (1,0,0) in (i,j,k) means a step of (1-c,-c,-c) in (x,y,z),
    // a step of (0,1,0) in (i,j,k) means a step of (-c,1-c,-c) in (x,y,z), and
    // a step of (0,0,1) in (i,j,k) means a step of (-c,-c,1-c) in (x,y,z), where
    // c = 1/6.
    float x1 = x0 - i1 + G3; // Offsets for second corner in (x,y,z) coords
    float y1 = y0 - j1 + G3;
    float z1 = z0 - k1 + G3;
    float x2 = x0 - i2 + 2.0f * G3; // Offsets for third corner in (x,y,z) coords
    float y2 = y0 - j2 + 2.0f * G3;
    float z2 = z0 - k2 + 2.0f * G3;
    float x3 = x0 - 1.0f + 3.0f * G3; // Offsets for last corner in (x,y,z) coords
    float y3 = y0 - 1.0f + 3.0f * G3;
    float z3 = z0 - 1.0f + 3.0f * G3;

    // Work out the hashed gradient indices of the four simplex corners
    int gi0 = hash1ToUint8(i + hash1ToUint8(j + hash1ToUint8(k)));
    int gi1 = hash1ToUint8(i + i1 + hash1ToUint8(j + j1 + hash1ToUint8(k + k1)));
    int gi2 = hash1ToUint8(i + i2 + hash1ToUint8(j + j2 + hash1ToUint8(k + k2)));
    int gi3 = hash1ToUint8(i + 1 + hash1ToUint8(j + 1 + hash1ToUint8(k + 1)));

    // Calculate the contribution from the four corners
    float t0 = 0.6f - x0*x0 - y0*y0 - z0*z0;                //C1...
    if (t0 < 0) {
        n0 = 0.0;
    } else {
        t0 *= t0;
        n0 = t0*t0*hashedGI3dToF(gi0, x0, y0, z0);
    }
    float t1 = 0.6f-x1*x1-y1*y1-z1*z1;                      //C2...
    if (t1 < 0) {
        n1 = 0.0;
    } else {
        t1 *= t1;
        n1 = t1*t1*hashedGI3dToF(gi1, x1, y1, z1);
    }
    float t2 = 0.6f-x2*x2-y2*y2-z2*z2;                      //C3...
    if (t2 < 0) {
        n2 = 0.0;
    } else {
        t2 *= t2;
        n2 = t2*t2*hashedGI3dToF(gi2, x2, y2, z2);
    }
    float t3 = 0.6f-x3*x3-y3*y3-z3*z3;                      //C4...
    if (t3 < 0) {
        n3 = 0.0;
    } else {
        t3 *= t3;
        n3 = t3*t3*hashedGI3dToF(gi3, x3, y3, z3);
    }
    
    //Add contributions from each corner to get the final noise value.
    //Result is scaled to stay within [-1, 1]
    return 32.0f*(n0+n1+n2+n3);
}

/*
    Same as noise2D, but scaled to be within [0, 1].
*/
float Noise::clampedNoise2D(float x, float y) { return noise2D(x, y)*0.5+0.5; }

/*
    Same as sNoise3D, but scaled to be within [0, 1].
*/
float Noise::clampedSNoise3D(float x, float y, float z) { return sNoise3D(x, y, z)*0.5+0.5; }