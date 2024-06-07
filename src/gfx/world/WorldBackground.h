#pragma once
#include "Camera.h"
#include "STexture.h"
#include "Vec3F.h"

class WorldBackground
{
public:
    void init(SDLHandler* sh, Camera* cam);
    void draw();

    void renderAll();
    void renderSkyWhole(const Vec3F& sunDir);

private:
    SDLHandler* sdlHandler;
    Camera* camera;

    Texture starsTex;   //Stars             - #1
    STexture skyTex;    //Sky               - #2
    STexture celTex;    //Celestial objects - #3
};


