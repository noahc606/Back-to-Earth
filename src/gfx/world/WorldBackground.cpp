#include "WorldBackground.h"
#include "Atmosphere.h"
#include "Timer.h"

void WorldBackground::init(SDLHandler* sh, Camera* cam) {
    sdlHandler = sh;
    camera = cam;

	renderAll();
}

/*
    May freeze the game for a brief moment.
*/
void WorldBackground::renderAll()
{
    int scale = 2;
    int texW = 1920/scale;
    int texH = 1080/scale;

//Sky
    skyTex.init(sdlHandler, texW, texH, scale);
    {
        Timer t("sky rendering", true);
        float thetaY = 92.f*M_PI/180.f;
        SDL_SetTextureBlendMode(skyTex.getSDLTexture(), SDL_BLENDMODE_BLEND);
        renderSkyWhole( Vec3F(0, cos(thetaY), -sin(thetaY)) );
    }

//Stars
    starsTex.init(sdlHandler, texW, texH, scale);
    starsTex.setBlendMode(SDL_BLENDMODE_BLEND);
    //Black background
    starsTex.lock();
    starsTex.setColorMod(0, 0, 0);
    starsTex.fill();
    //background_space_interstellar texture
    starsTex.lock();
    starsTex.setColorMod(255, 255, 255);
    starsTex.blit(TextureLoader::WORLD_background_space_interstellar);

}

void WorldBackground::renderSkyWhole(const Vec3F& sunDir)
{
    Atmosphere atmosphere(sunDir);

    // Params
    float rHDivisor = 1.0f; // Render Height Divisor
    unsigned width = skyTex.getTexWidth();
    unsigned height = skyTex.getTexHeight();

    // Render from a normal camera
    Vec3F* image = new Vec3F[width * height];       // Create 1D array of vectors whose (x,y,z) components will be transformed into (r,g,b).
    memset(image, 0x0, sizeof(Vec3F)*width*height); // Allocate appropriate amount of memory for 'image' array.
    Vec3F* p = image;                               // Create accessor for 'image' array.

    float aspectRatio = width / float(height); 
    float fov = 65; 
    float angle = std::tan(fov * M_PI / 180 * 0.5f); 
    Vec3F orig(0, atmosphere.earthRadius+10*10, 0);  //camera position
    
    for (unsigned y = 0; y<height/rHDivisor; y++) { 
        for (unsigned x = 0; x<width; x++, p++) { 

                    float rayX = (2 * (x+0.5f) / float(width) - 1) * aspectRatio * angle; 
                    float rayY = (1 - (y+0.5f) / float(height) * 2) * angle; 
                    Vec3F dir(rayX, rayY, -1);
                    Vec3F::normalize(dir);
                    float t0, t1, tMax = Atmosphere::inf;
                    if (Atmosphere::raySphereIntersect(orig, dir, atmosphere.earthRadius, t0, t1) && t1 > 0) {
                        tMax = std::max(0.f, t0); 
                    }

                    // The *viewing or camera ray* is bounded to the range [0:tMax]
                    *p += atmosphere.computeIncidentLight(orig, dir, 0, tMax); 

            *p *= 1.f; 
        } 
        fprintf(stderr, "\b\b\b\b%3d%c", (int)(100 * y / (width - 1)), '%'); 
    }
    printf("\n");

    p = image;

    float powval = 1.0f / 2.2f;
    float tone = 0.38317f;

    {
        Timer t("texture building", true);

        skyTex.lock();
        for (unsigned j = 0; j < height/rHDivisor; ++j) { 
            for (unsigned i = 0; i < width; ++i, ++p) {
                // Apply tone mapping function
                p->x = p->x<1.413f ? pow(p->x*tone, powval) : 1.0f-exp(-p->x); //Red [0-1]
                p->y = p->y<1.413f ? pow(p->y*tone, powval) : 1.0f-exp(-p->y); //Green [0-1]
                p->z = p->z<1.413f ? pow(p->z*tone, powval) : 1.0f-exp(-p->z); //Blue [0-1]


                uint8_t r = (unsigned char)(std::min(1.f, p->x)*255);
                uint8_t g = (unsigned char)(std::min(1.f, p->y)*255);
                uint8_t b = (unsigned char)(std::min(1.f, p->z)*255);

                int val = Color(r, g, b).getHSV2();
                uint8_t opacity = 255;
                if(val<25) {
                    opacity = val*10;
                }

                skyTex.pixel(i, j, r, g, b, opacity);
            }
        }
        skyTex.unlock();
    }

    delete[] image;
}

void WorldBackground::draw() {
    if(camera->getDirection()!=Camera::DOWN) {
    	starsTex.setDrawPos( (sdlHandler->getWidth()/2-starsTex.getTexWidth())/2*2, 0 );
        starsTex.draw();

        skyTex.setDrawPos( (sdlHandler->getWidth()/2-skyTex.getTexWidth())/2*2, 0 );
	    skyTex.draw();
    }
}