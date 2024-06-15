#pragma once
#include "Camera.h"
#include "Planet.h"
#include "STexture.h"
#include "Vec3F.h"

class WorldBackground
{
public:
    void init(SDLHandler* sh, Camera* cam, Planet* plnt);
    void draw();
    void tick();

    void renderAll();
    void renderSkyPartial(int pxCol);
    

private:
    SDLHandler* sdlHandler = nullptr;
    Camera* camera = nullptr;
    Planet* planet = nullptr;

    Texture starsTex;   //Stars             - #1
    STexture skyTex;    //Sky               - #2
    STexture celTex;    //Celestial objects - #3

    Vec3F sunDirection;

    //Parameters deciding when sky is supposed to change visually
    int64_t skyUpdateIntervalMS = 5000;
    int64_t lastSkyUpdateMS = 0;

    /*
        Parameters deciding when sky texture is supposed to be drawn onto.
        
        Because light ray-casting is expensive, we draw parts of the sky individually rather than the whole sky every update interval
        This prevents stuttering every 5000ms.
    */
    int64_t numSkyPartDraws = 25;   int64_t currSkyPartDraw = 0;
    int64_t lastSkyPartDrawMS = 0;
    bool visibleSkyPartDraws = false;

    int timer = 0;

};


