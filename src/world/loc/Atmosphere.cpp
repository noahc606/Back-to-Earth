#include "Atmosphere.h"
#include <cstdint>
#include <limits>
#include <math.h>
#include <utility>

using namespace nch;

/**
 *  Implementation of many functions in this class were inspired by:
 *  https://www.scratchapixel.com/lessons/procedural-generation-virtual-worlds/simulating-sky/simulating-colors-of-the-sky.html
 *  https://github.com/scratchapixel/scratchapixel-code/blob/main/simulating-sky/skycolor.cpp
*/


const Vec3<float> Atmosphere::betaR = Vec3<float>(3.8e-6f, 13.5e-6f, 33.1e-6f); 
const Vec3<float> Atmosphere::betaM = Vec3<float>(21e-6f);
const float Atmosphere::inf = std::numeric_limits<float>::max();

Atmosphere::Atmosphere(Vec3<float> sundir, float plntRad, float atmRad, float tr, float tm)
{
    sunDirection = sundir;
    planetRadius = plntRad;
    atmosphereRadius = atmRad;
    thickR = tr;
    thickM = tm;
}

Atmosphere::Atmosphere(Vec3<float> sundir):
Atmosphere(sundir, 6360000, 6420000, 7994, 1200){}

Atmosphere::Atmosphere():
Atmosphere(Vec3<float>(0, 0, 1)){}

bool solveQuadratic(float a, float b, float c, float& x1, float& x2)
{
    if(b==0) {
        //Handle special case where the the two vector ray.dir and V are perpendicular
        //with V = ray.orig - sphere.centre
        if(a==0) return false;
        x1 = 0; x2 = std::sqrt(-c/a);
        return true;
    }
    float discr = b*b-4*a*c;

    if(discr<0) return false;

    float q = (b<0.f) ? -0.5f*(b-std::sqrt(discr)) : -0.5f*(b+std::sqrt(discr));
    x1 = q/a;
    x2 = c/q;

    return true;
}

/*
    Finds t0 and t1 which represent the points at which the ray intersects the planet
*/
bool Atmosphere::raySphereIntersect(const Vec3<float>& orig, const Vec3<float>& dir, const float& radius, float& t0, float& t1)
{
    // They ray dir is normalized so A = 1 
    float A = dir.x*dir.x + dir.y*dir.y + dir.z*dir.z;                          //x^2 + y^2 + z^2
    float B = 2*(dir.x*orig.x + dir.y*orig.y + dir.z*orig.z);
    float C = orig.x*orig.x + orig.y*orig.y + orig.z*orig.z - radius*radius;

    if (!solveQuadratic(A, B, C, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

    return true;
}

Vec3<float> Atmosphere::computeIncidentLight(const Vec3<float>& orig, const Vec3<float>& dir, float tmin, float tmax) const
{
    //Find t0 and t1 from raySphereIntersect.
    float t0 = 0, t1 = 0;
    if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0) return 0;

    //truncating tmin and tmax
    if (t0>tmin && t0>0) tmin = t0;
    if (t1<tmax) tmax = t1;

    // parameters
    uint32_t numSamples = 8;
    uint32_t numSamplesLight = 4;
    float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    Vec3<float> sumR(0), sumM(0); // rayleigh and mie contribution
    float opticalDepthR = 0, opticalDepthM = 0;
    float mu = dir.dot(sunDirection); // mu in the paper which is the cosine of the angle between the sun direction and the ray direction
    float phaseR = 3.f/(16.f*M_PI)*(1+mu*mu);
    float g = 0.76f;
    float phaseM = 3.f/(8.f*M_PI)*((1.f-g*g)*(1.f+mu*mu))/((2.f+g*g)*pow(1.f+g*g-2.f*g*mu, 1.5f));
    
    
    for (uint32_t i = 0; i<numSamples; ++i) {
        Vec3<float> samplePosition = orig + dir*(tCurrent+segmentLength*0.5f);
        float height = samplePosition.length() - planetRadius;
        
        // compute optical depth for light
        float hr = std::exp(-height/thickR) * segmentLength;
        float hm = std::exp(-height/thickM) * segmentLength;


        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light, t1Light;
        raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
        uint32_t j;
        for (j = 0; j < numSamplesLight; ++j) {
            Vec3<float> samplePositionLight = samplePosition + sunDirection*(tCurrentLight+segmentLengthLight*0.5f);
            float heightLight = samplePositionLight.length() - planetRadius;
            if (heightLight < 0) break;
            opticalDepthLightR += std::exp(-heightLight/thickR) * segmentLengthLight;
            opticalDepthLightM += std::exp(-heightLight/thickM) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            Vec3<float> tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            Vec3<float> attenuation(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }

    return (sumR*betaR*phaseR + sumM*betaM*phaseM) * 20;
}
