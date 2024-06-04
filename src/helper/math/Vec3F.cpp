#include "Vec3F.h"
#include <cmath>

Vec3F::Vec3F(float x, float y, float z)
{
    Vec3F::x = x;
    Vec3F::y = y;
    Vec3F::z = z;
}
Vec3F::Vec3F(float c) : Vec3F(c, c, c){}
Vec3F::Vec3F() : Vec3F(0){}

float Vec3F::length2() { return x*x+y*y+z*z; }
float Vec3F::length() { return std::sqrt(length2()); }


/*
    Scale a vector to be length one while preserving its direction
*/
void Vec3F::normalize(Vec3F& vec)
{
    float len2 = vec.length2();
    if (len2 > 0) {
        float invLen = 1 / std::sqrt(len2);
        vec.x *= invLen, vec.y *= invLen, vec.z *= invLen;
    }
}