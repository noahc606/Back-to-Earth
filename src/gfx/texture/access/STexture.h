#pragma once
#include "Texture.h"
#include "SpriteSheet.h"

class STexture : public Texture
{
public:
    /**/
    STexture();
    virtual ~STexture();
    void init(SDLHandler* sdlHandler, int texWidth, int texHeight, double texScale);
    void init(SDLHandler* sdlHandler, int texWidth, int texHeight);
    /**/

    //void lock(SDL_Rect l);
    //void lock(int lx, int ly, int lw, int lh);
    void lock();
    void unlock();

    void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);

    /**/

protected:

private:
    void* pixels;
    int pitch;
    bool locked = false;
};
