#include "WorldBackground.h"
#include "Atmosphere.h"
#include "Log.h"
#include "Timer.h"

void WorldBackground::init(SDLHandler* sh, Camera* cam, Planet* plnt) {
    sdlHandler = sh;
    camera = cam;
    planet = plnt;

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
    SDL_SetTextureBlendMode(skyTex.getSDLTexture(), SDL_BLENDMODE_BLEND);

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

/*
    Note: this function assumes that 'skyTex' is already locked.

    Params:
    - sunDir: the Vec3F direction of the sun from the camera.
    - pxCol: the pixel column (0 indexed) that we should draw.
*/
void WorldBackground::renderSkyPartial(int pxCol)
{
/* Preparation */
// Create atmosphere with specified 'sunDir'
    Atmosphere atmosphere(sunDirection);
    unsigned inc = 2;
// Basic parameters: determined from texture
    unsigned width = skyTex.getTexWidth();
    unsigned height = skyTex.getTexHeight();
// Camera parameters: aspect ratio, FOV, angle, position
    float aspectRatio = width/float(height); 
    float fov = 65; 
    float angle = std::tan(fov*M_PI/180*0.5f); 
    Vec3F orig(0, atmosphere.planetRadius-camera->getZ(), 0);   //Position
// Create 2D array of Vec3F, with accessor 'p'
    Vec3F* image = new Vec3F[height];               // Create 1D array of vectors whose (x,y,z) components will be transformed into (r,g,b).
    memset(image, 0x0, sizeof(Vec3F)*height);       // Allocate appropriate amount of memory for 'image' array.
    Vec3F* p = image;                               // Create accessor for 'image' array.

/* Building texture */
    // Build column of Vec3F pixels.
    for (unsigned y = 0; y<height; y+=inc, p+=inc) { 
        //Build normalized vector which represents light direction
        float rayX = (2*(pxCol+0.5f)/float(width)-1)*aspectRatio*angle; 
        float rayY = (1-(y+0.5f)/float(height)*2)*angle; 
        Vec3F dir(rayX, rayY, -1);
        Vec3F::normalize(dir);

        float t0, t1, tMax = Atmosphere::inf;
        //If ray DOES intersect with planet...
        if (Atmosphere::raySphereIntersect(orig, dir, atmosphere.planetRadius, t0, t1) && t1 > 0) {
            //Compute light from planet surface
            tMax = std::max(0.f, t0);
            *p = Vec3F(0.25, 0.10, 0.05);  //Ground color at this point
        //If ray does NOT intersect with planet...
        } else {
            //Compute light from atmosphere
            *p += atmosphere.computeIncidentLight(orig, dir, 0, tMax);  //Add vector from computeIncidentLight to current element pointed by p
            *p *= 1.f;
        }
    }

    // Apply tone mapping to relevant pixels, store the colors into a new column
    p = image;

    //Create color column
    Color* clrs = new Color[height];
    Color* c = clrs;

    float powval = 1.0f / 2.2f;
    float tone = 0.38317f;
    for (unsigned y = 0; y<height; y+=inc, p+=inc, c+=inc) {
        // Apply tone mapping function
        p->x = p->x<1.413f ? pow(p->x*tone, powval) : 1.0f-exp(-p->x); //Red [0-1]
        p->y = p->y<1.413f ? pow(p->y*tone, powval) : 1.0f-exp(-p->y); //Green [0-1]
        p->z = p->z<1.413f ? pow(p->z*tone, powval) : 1.0f-exp(-p->z); //Blue [0-1]

        uint8_t r = (unsigned char)(std::min(1.f, p->x)*255);
        uint8_t g = (unsigned char)(std::min(1.f, p->y)*255);
        uint8_t b = (unsigned char)(std::min(1.f, p->z)*255);
        int val = Color(r, g, b).getHSV2();
        uint8_t a = 255;
        if(val<25) {
            a = val*10;
        }

        *c = Color(r, g, b, a);
    }

    //Interpolate pixel colors within color column
    if(true) {
        c = clrs+1;
        for(int i = 1; i<height-inc; i+=inc, c+=inc) {
            for(int j = 0; j<inc-1; j++) {
                float weight = (j+1.)*1./(float)inc;
                *(c+j) = (c-1)->getInterpolColor(*(c+inc-1), weight);
            }
        }

        //Last rows: Just copy the color of the lowest pixel.
        c = clrs+height-inc;
        for(int j = 0; j<inc; j++) {
            *(c+j) = *(c);
        }
    }

    //Finally, set pixels in skyTex corresponding to the color column we just built
    c = clrs;
    for(int i = 0; i<height; i++, c++) {
        skyTex.pixel(pxCol, i, c->r, c->g, c->b, c->a);
    }

    delete[] clrs;
    delete[] image;
}

void WorldBackground::tick()
{
    //Angle: 0=noon, 90=sunset, 180=midnight, 270=sunrise
    float thetaYD = planet->getPartOfDayAccurate(360)+180.;
    float thetaYR = thetaYD*(M_PI/180.f);
    sunDirection = Vec3F(0, cos(thetaYR), -sin(thetaYR));
}

void WorldBackground::draw() {
    //Draw part of sky every 500ms
    if( SDL_GetTicks64()>lastSkyPartDrawMS+skyUpdateIntervalMS/numSkyPartDraws ) {
        lastSkyPartDrawMS = SDL_GetTicks64();
        
        int stw = skyTex.getTexWidth();
        int pds = currSkyPartDraw*stw/numSkyPartDraws;  //(P)artial (D)raw (S)tart
        int pde = pds+stw/numSkyPartDraws;              //(P)artial (D)raw (E)nd
        
        if(visibleSkyPartDraws) skyTex.lock();
        for(int x = pds; x<=pde; x++) {
            renderSkyPartial(x);
        }
        if(visibleSkyPartDraws) skyTex.unlock();

        currSkyPartDraw++;
        if(currSkyPartDraw==numSkyPartDraws) {
            currSkyPartDraw = 0;

            if(!visibleSkyPartDraws) {
                skyTex.unlock();
                skyTex.lock();
            }
        }
    }

    if(camera->getDirection()!=Camera::DOWN) {
    	starsTex.setDrawPos( sdlHandler->getWidth()/2-starsTex.getDrawScale()*starsTex.getTexWidth()/2, 0 );
        starsTex.draw();

        skyTex.setDrawPos( sdlHandler->getWidth()/2-skyTex.getDrawScale()*skyTex.getTexWidth()/2, 0 );
	    skyTex.draw();
    }
}