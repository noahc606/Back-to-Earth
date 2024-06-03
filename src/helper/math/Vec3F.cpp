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