#pragma once

class Vec3F
{
public:
    Vec3F(float x, float y, float z);
    Vec3F(float c);
    Vec3F();

    Vec3F operator + (const Vec3F& v) const { return Vec3F(x + v.x, y + v.y, z + v.z); }
    Vec3F operator * (const float& r) const { return Vec3F(x*r, y*r, z*r); }
    Vec3F operator / (const float& r) const { return Vec3F(x/r, y/r, z/r); }
    Vec3F operator * (const Vec3F& v) const { return Vec3F(x * v.x, y * v.y, z * v.z); }    // not dot product!
    Vec3F& operator += (const Vec3F& v) { x += v.x, y += v.y, z += v.z; return *this; }
    Vec3F& operator *= (const float& r) { x *= r, y *= r, z *= r; return *this; }


    friend Vec3F operator * (const float&r, const Vec3F& v) { return Vec3F(v.x * r, v.y * r, v.z * r); }

    float length2();
    float length();

    static void normalize(Vec3F& vec);

    float x = 0, y = 0, z = 0;
protected:
private:
};
