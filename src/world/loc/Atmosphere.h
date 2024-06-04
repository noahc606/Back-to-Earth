#pragma once
#include "Vec3F.h"

class Atmosphere 
{
public:
    Atmosphere(Vec3F sundir, float plntRad, float atmRad, float hr, float hm);
    Atmosphere(Vec3F sundir);
    Atmosphere();
 
    Vec3F computeIncidentLight(const Vec3F& orig, const Vec3F& dir, float tmin, float tmax) const; 
    static bool raySphereIntersect(const Vec3F& orig, const Vec3F& dir, const float& radius, float& t0, float& t1);

    Vec3F sunDirection;         //The sun direction (normalized) 
    float earthRadius;          //In the paper this is usually Rg or Re (radius ground, eart) 
    float atmosphereRadius;     //In the paper this is usually R or Ra (radius atmosphere) 
    float thickR;               //Thickness of the atmosphere if density was uniform (Hr) 
    float thickM;               //Same as above but for Mie scattering (Hm) 
 
    static const Vec3F betaR;
    static const Vec3F betaM;
    static const float inf;
}; 
 
