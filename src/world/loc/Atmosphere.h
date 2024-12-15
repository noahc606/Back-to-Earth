#pragma once
#include <nch/math-utils/vec3.h>

class Atmosphere 
{
public:
    Atmosphere(nch::Vec3<float> sundir, float plntRad, float atmRad, float hr, float hm);
    Atmosphere(nch::Vec3<float> sundir);
    Atmosphere();
 
    nch::Vec3<float> computeIncidentLight(const nch::Vec3<float>& orig, const nch::Vec3<float>& dir, float tmin, float tmax) const; 
    static bool raySphereIntersect(const nch::Vec3<float>& orig, const nch::Vec3<float>& dir, const float& radius, float& t0, float& t1);

    nch::Vec3<float> sunDirection;  //The sun direction (normalized) 
    float planetRadius;             //In the paper this is usually Rg or Re (radius ground, eart) 
    float atmosphereRadius;         //In the paper this is usually R or Ra (radius atmosphere) 
    float thickR;                   //Thickness of the atmosphere if density was uniform (Hr) 
    float thickM;                   //Same as above but for Mie scattering (Hm) 
 
    static const nch::Vec3<float> betaR;
    static const nch::Vec3<float> betaM;
    static const float inf;
}; 
 
