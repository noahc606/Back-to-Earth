#include "Tests.h"
#include <map>
#include <math.h>
#include <bitset>
#include <codecvt>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <math.h>
#include <random>
#include <set>
#include <sstream>
#include <SDL2/SDL_net.h>
#include "Color.h"
#include "DataStream.h"
#include "Grid.h"
#include "Log.h"
#include "MainLoop.h"
#include "Noise.h"
#include "Terrain.h"
#include "Real.h"
#include "TextOld.h"
#include "TileMap.h"
#include "TileMapScreen.h"
#include "Timer.h"
#include "Vec3F.h"
#include "Window.h"

Tests::Tests(){}

float dist(float x0, float y0, float z0, float x1, float y1, float z1)
{
    return std::sqrt( (x1-x0)*(x1-x0) + (y1-y0)*(y1-y0) + (z1-z0)*(z1-z0) );
}

void assetList(FileHandler* fileHandler)
{
    std::vector<std::string> contents = fileHandler->listDirContents("resources", false, true);
    for(std::string s : contents) {
        if(!fileHandler->dirExists(s)) {
            printf("backtoearth/%s\n", s.c_str());
        }
    }
}

const float kInfinity = std::numeric_limits<float>::max();

class Atmosphere 
{ 
public: 
    Atmosphere(
        Vec3F sd = Vec3F(0, 0, 1), 
        float er = 6360000, float ar = 6420000, 
        float hr = 7994, float hm = 1200) : 
        sunDirection(sd), 
        earthRadius(er), 
        atmosphereRadius(ar), 
        Hr(hr), 
        Hm(hm) 
    {} 
 
    Vec3F computeIncidentLight(const Vec3F& orig, const Vec3F& dir, float tmin, float tmax) const; 
 
    Vec3F sunDirection;      //The sun direction (normalized) 
    float earthRadius;       //In the paper this is usually Rg or Re (radius ground, eart) 
    float atmosphereRadius;  //In the paper this is usually R or Ra (radius atmosphere) 
    float Hr;                //Thickness of the atmosphere if density was uniform (Hr) 
    float Hm;                //Same as above but for Mie scattering (Hm) 
 
    static const Vec3F betaR; 
    static const Vec3F betaM; 
}; 
 
const Vec3F Atmosphere::betaR(3.8e-6f, 13.5e-6f, 33.1e-6f); 
const Vec3F Atmosphere::betaM(21e-6f);

bool solveQuadratic(float a, float b, float c, float& x1, float& x2)
{
    if (b == 0) {
        // Handle special case where the the two vector ray.dir and V are perpendicular
        // with V = ray.orig - sphere.centre
        if (a == 0) return false;
        x1 = 0; x2 = std::sqrt(-c / a);
        return true;
    }
    float discr = b * b - 4 * a * c;

    if (discr < 0) return false;

    float q = (b < 0.f) ? -0.5f * (b - std::sqrt(discr)) : -0.5f * (b + std::sqrt(discr));
    x1 = q / a;
    x2 = c / q;

    return true;
}

float dot(const Vec3F& va, const Vec3F& vb)
{
    return va.x * vb.x + va.y * vb.y + va.z * vb.z;
}

/*
    Scale a vector to be length one while preserving its direction
*/
void normalize(Vec3F& vec)
{
    float len2 = vec.length2();
    if (len2 > 0) {
        float invLen = 1 / std::sqrt(len2);
        vec.x *= invLen, vec.y *= invLen, vec.z *= invLen;
    }
}

bool raySphereIntersect(const Vec3F& orig, const Vec3F& dir, const float& radius, float& t0, float& t1)
{
    // They ray dir is normalized so A = 1 
    float A = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
    float B = 2 * (dir.x * orig.x + dir.y * orig.y + dir.z * orig.z);
    float C = orig.x * orig.x + orig.y * orig.y + orig.z * orig.z - radius * radius;

    if (!solveQuadratic(A, B, C, t0, t1)) return false;

    if (t0 > t1) std::swap(t0, t1);

    return true;
}

Vec3F Atmosphere::computeIncidentLight(const Vec3F& orig, const Vec3F& dir, float tmin, float tmax) const
{
    float t0, t1;
    if (!raySphereIntersect(orig, dir, atmosphereRadius, t0, t1) || t1 < 0) return 0;
    if (t0 > tmin && t0 > 0) tmin = t0;
    if (t1 < tmax) tmax = t1;
    uint32_t numSamples = 8;
    uint32_t numSamplesLight = 4;
    float segmentLength = (tmax - tmin) / numSamples;
    float tCurrent = tmin;
    Vec3F sumR(0), sumM(0); // rayleigh and mie contribution
    float opticalDepthR = 0, opticalDepthM = 0;
    float mu = dot(dir, sunDirection); // mu in the paper which is the cosine of the angle between the sun direction and the ray direction
    float phaseR = 3.f/(16.f*M_PI)*(1+mu*mu);
    float g = 0.76f;
    float phaseM = 3.f/(8.f*M_PI)*((1.f-g*g)*(1.f+mu*mu))/((2.f+g*g)*pow(1.f+g*g-2.f*g*mu, 1.5f));
    
    
    for (uint32_t i = 0; i<numSamples; ++i) {
        Vec3F samplePosition = orig + (tCurrent + segmentLength * 0.5f) * dir;
        float height = samplePosition.length() - earthRadius;
        // compute optical depth for light
        float hr = exp(-height / Hr) * segmentLength;
        float hm = exp(-height / Hm) * segmentLength;
        opticalDepthR += hr;
        opticalDepthM += hm;
        // light optical depth
        float t0Light, t1Light;
        raySphereIntersect(samplePosition, sunDirection, atmosphereRadius, t0Light, t1Light);
        float segmentLengthLight = t1Light / numSamplesLight, tCurrentLight = 0;
        float opticalDepthLightR = 0, opticalDepthLightM = 0;
        uint32_t j;
        for (j = 0; j < numSamplesLight; ++j) {
            Vec3F samplePositionLight = samplePosition + (tCurrentLight + segmentLengthLight * 0.5f) * sunDirection;
            float heightLight = samplePositionLight.length() - earthRadius;
            if (heightLight < 0) break;
            opticalDepthLightR += exp(-heightLight / Hr) * segmentLengthLight;
            opticalDepthLightM += exp(-heightLight / Hm) * segmentLengthLight;
            tCurrentLight += segmentLengthLight;
        }
        if (j == numSamplesLight) {
            Vec3F tau = betaR * (opticalDepthR + opticalDepthLightR) + betaM * 1.1f * (opticalDepthM + opticalDepthLightM);
            Vec3F attenuation(exp(-tau.x), exp(-tau.y), exp(-tau.z));
            sumR += attenuation * hr;
            sumM += attenuation * hm;
        }
        tCurrent += segmentLength;
    }

    return (sumR * betaR * phaseR + sumM * betaM * phaseM) * 20;
}

void renderSkydomeFisheye(const Vec3F& sunDir, Texture& tex) 
{ 
    Atmosphere atmosphere(sunDir); 
 
    const unsigned width = tex.getTexWidth(), height = tex.getTexHeight();


    Vec3F* image = new Vec3F[width * height];
    Vec3F* p = image; 
    memset(image, 0x0, sizeof(Vec3F) * width * height); 
    for (unsigned j = 0; j < height; ++j) { 
        float y = 2.f * (j + 0.5f) / float(height - 1) - 1.f; 
        for (unsigned i = 0; i < width; ++i, ++p) {
            float x = 2.f * (i + 0.5f) / float(width - 1) - 1.f; 
            float z2 = x*x + y*y; 
            if (z2 <= 1) { 
                float phi = std::atan2(y, x); 
                float theta = std::acos(1 - z2); 
                Vec3F dir(sin(theta) * cos(phi), cos(theta), sin(theta) * sin(phi)); 
                // 1 meter above sea level
                *p = atmosphere.computeIncidentLight(Vec3F(0, atmosphere.earthRadius+1, 0), dir, 0, kInfinity); 
            } 
        }
        fprintf(stderr, "\b\b\b\b\%3d%c", (int)(100 * j / (width - 1)), '%'); 
    }

    p = image;

    float powval = 1.0f / 2.2f;
    float tone = 0.38317f;

    for (unsigned j = 0; j < height; ++j) { 
        for (unsigned i = 0; i < width; ++i, ++p) { 
            // Apply tone mapping function
            p->x = p->x<1.413f ? pow(p->x*tone, powval) : 1.0f-exp(-p->x); //Red [0-1]
            p->y = p->y<1.413f ? pow(p->y*tone, powval) : 1.0f-exp(-p->y); //Green [0-1]
            p->z = p->z<1.413f ? pow(p->z*tone, powval) : 1.0f-exp(-p->z); //Blue [0-1]


            uint8_t r = (unsigned char)(std::min(1.f, p->x)*255);
            uint8_t g = (unsigned char)(std::min(1.f, p->y)*255);
            uint8_t b = (unsigned char)(std::min(1.f, p->z)*255);
            tex.pixel(i, j, r, g, b);
        } 
    } 
    
    delete[] image; 
}

void renderSkydome(const Vec3F& sunDir, STexture* stex, SDLHandler* sh)
{
    Atmosphere atmosphere(sunDir);

    // Params
    float rHDivisor = 1.0f; // Render Height Divisor
    unsigned width = stex->getTexWidth();
    unsigned height = stex->getTexHeight();

    // Render from a normal camera
    Vec3F* image = new Vec3F[width * height];       // Create 1D array of vectors whose (x,y,z) components will be transformed into (r,g,b).
    memset(image, 0x0, sizeof(Vec3F)*width*height); // Allocate appropriate amount of memory for 'image' array.
    Vec3F* p = image;                               // Create accessor for 'image' array.

    float aspectRatio = width / float(height); 
    float fov = 65; 
    float angle = std::tan(fov * M_PI / 180 * 0.5f); 
    unsigned numPixelSamples = 1; 
    Vec3F orig(0, atmosphere.earthRadius+10*60, 0);  //camera position
    
    for (unsigned y = 0; y<height/rHDivisor; y++) { 
        for (unsigned x = 0; x<width; x++, p++) { 
            for (unsigned m = 0; m < numPixelSamples; m++) { 
                for (unsigned n = 0; n < numPixelSamples; n++) { 
                    float rayX = (2 * (x + (m + 0.5f) / numPixelSamples) / float(width) - 1) * aspectRatio * angle; 
                    float rayY = (1 - (y + (n + 0.5f) / numPixelSamples) / float(height) * 2) * angle; 
                    Vec3F dir(rayX, rayY, -1);
                    normalize(dir);
                    // Does the ray intersect the planetory body? (the intersection test is against the Earth here
                    // not against the atmosphere). If the ray intersects the Earth body and that the intersection
                    // is ahead of us, then the ray intersects the planet in 2 points, t0 and t1. But we
                    // only want to comupute the atmosphere between t=0 and t=t0 (where the ray hits
                    // the Earth first). If the viewing ray doesn't hit the Earth, or course, the ray
                    // is then bounded to the range [0:INF]. In the method computeIncidentLight() we then
                    // compute where this primary ray intersects the atmosphere, and we limit the max t range 
                    // of the ray to the point where it leaves the atmosphere.
                    float t0, t1, tMax = kInfinity;
                    //float tMax = kInfinity;


                    if (raySphereIntersect(orig, dir, atmosphere.earthRadius, t0, t1) && t1 > 0) {
                        tMax = std::max(0.f, t0); 
                    }

                    // The *viewing or camera ray* is bounded to the range [0:tMax]
                    *p += atmosphere.computeIncidentLight(orig, dir, 0, tMax); 
                }
            }
            *p *= 1.f / (numPixelSamples * numPixelSamples); 
        } 
        fprintf(stderr, "\b\b\b\b%3d%c", (int)(100 * y / (width - 1)), '%'); 
    }
    printf("\n");

    p = image;

    float powval = 1.0f / 2.2f;
    float tone = 0.38317f;

    {
        Timer t("texture building", true);

        stex->lock();
        for (unsigned j = 0; j < height/rHDivisor; ++j) { 
            for (unsigned i = 0; i < width; ++i, ++p) {
                // Apply tone mapping function
                p->x = p->x<1.413f ? pow(p->x*tone, powval) : 1.0f-exp(-p->x); //Red [0-1]
                p->y = p->y<1.413f ? pow(p->y*tone, powval) : 1.0f-exp(-p->y); //Green [0-1]
                p->z = p->z<1.413f ? pow(p->z*tone, powval) : 1.0f-exp(-p->z); //Blue [0-1]


                uint8_t r = (unsigned char)(std::min(1.f, p->x)*255);
                uint8_t g = (unsigned char)(std::min(1.f, p->y)*255);
                uint8_t b = (unsigned char)(std::min(1.f, p->z)*255);
                
                stex->pixel(i, j, r, g, b, 255);
                //Color col(255, 0, 0, 0);
                //tex.pixel(i, j, r, g, b);
                //pixels[i + (j*surf->w)] = SDL_MapRGB(surf->format, 255, 0, 0);//col.getRGBA();
            }
        }
        stex->unlock();
    }

    delete[] image;
}

void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	sdlHandler = sh;
	fileHandler = fh;
	controls = ctrls;

    int texW = 960/2;
    int texH = 540/2;
    tex.init(sdlHandler, texW, texH, 2);
    {
        Timer t("sky rendering", true);
        float thetaY = 80.f*M_PI/180.f;
        renderSkydome(Vec3F(0, cos(thetaY), -sin(thetaY)), &tex, sdlHandler); 
    }
}

Tests::~Tests(){}

/**/

void drawlevel()
{
    /*
    counter++;
	
	SDL_Rect dst;
	dst.w = 32*6;
	dst.h = dst.w*4;
	dst.x = sdlHandler->getWidth()/2-dst.w/2;
	dst.y = sdlHandler->getHeight()/2-dst.h/2;
	
	tex.draw(&lvlImgSrc, &dst);
	
	int mw = controls->getMouseWheel();
	if( mw!=0 ) {
		
		if(mw<0) {
			lvlImgSrc.y += 32;
		}
		if (mw>0) {
			lvlImgSrc.y -= 32;
		}
		
		if(lvlImgSrc.y<0) {
			lvlImgSrc.y = 0;
		}
		
		controls->resetWheel(__PRETTY_FUNCTION__);
	}

    //sdlHandler->renderCopy(3, &dst, &dst);
    */
	

    for(int i = 0; i<10; i++) {
        //stex.lock(rx*32, ry*32, 32, 32);
        //stex.
    }

    //test123.draw();
    //stex.draw();
    //ttex.draw();

    /*

    test123.lock(200, 200, 40, 200);
    test123.blit(TextureLoader::GUI_FONT_robot, 0, 0);
    test123.draw();*/
}

void Tests::draw()
{
    //tex.draw();
    SDL_RenderCopy(sdlHandler->getRenderer(), tex.getSDLTexture(), NULL, NULL);
    
    //tex.draw();

    double scale = 2;
    
    //tex.setDrawScale(scale);
    //tex.setDrawPos(sdlHandler->getWidth()/2-tex.getTexWidth()*scale/2, sdlHandler->getHeight()/2-tex.getTexHeight()*scale/2);
}

void Tests::tick()
{
	
}

/**/

void Tests::putInfo(std::stringstream& ss, int& tabs)
{
	ss << "Test...\n";
}

/*
void test1()
{
    //SpriteSheet ss;
    //ColorPalette cpal; cpal.init(cpal.DEFAULT_PLAYER);

    //SpriteSheetBuilder ssb(sh);

    //ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

    stex.init(sdlHandler);
    stex.setTexDimensions(1024, 1024);

    SpriteSheet ss;
    ss.init(sdlHandler, TextureLoader::WORLD_TILE_type_a);

    stex.lock(0, 0, 512, 512);
    stex.fill(127, 127, 255);
    stex.update();
    Timer tim1("test1", false);
    {
        stex.setColorMod(255, 0, 0);
        stex.setBlendMode(SDL_BLENDMODE_MOD);
        stex.lock(128, 128, 128, 128);
        stex.sblit(&ss, 0, 0, 512, 512);
        stex.update();
    }

    tim1.debugElapsedTimeMS();


    ttex.init(sdlHandler, 1024, 1024);
    ttex.lock(0, 0, 512, 512);
    ttex.setColorMod(127, 127, 255);
    ttex.fill();
    Timer tim2("test2", false);
    for( int i = 0; i<1; i++ ) {
        ttex.setColorMod(255, 0, 0);
        ttex.setBlendMode(SDL_BLENDMODE_MOD);
        ttex.lock(128, 128, 128, 128);
        ttex.blit(TextureLoader::WORLD_TILE_type_a, 0, 0, 512, 512);
    }
    tim2.debugElapsedTimeMS();

    //stex.lock(10, 10, 32, 32);
    //stex.unlock();
    //stex.update();
    //stex.blit(TextureLoader::WORLD_TILE_type_a, 64, 64);
    //stex.unlock();

    std::string trPath = "saved\\games\\tutorial\\tr";

    Timer fw("file writing");

    TileRegion treg;
    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            for( int z = 0; z<32; z++ ) {
                treg.setTile(x, y, z, x*1024+y*32+z);
            }
        }
    }

    treg.save(sdlHandler, fileHandler, false);
}
*/
