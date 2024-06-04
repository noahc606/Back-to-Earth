#pragma once
#include "STexture.h"
#include "Vec3F.h"

class WorldBackground
{
public:
    void init(SDLHandler* sh);
    void draw();

    void renderAll();
    void renderSkyWhole(const Vec3F& sunDir);

private:
    SDLHandler* sdlHandler;

    Texture starsTex;   //Stars             - #1
    STexture skyTex;    //Sky               - #2
    STexture celTex;    //Celestial objects - #3
};


