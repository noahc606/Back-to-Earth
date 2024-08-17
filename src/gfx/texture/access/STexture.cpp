#include "STexture.h"
#include <nch/cpp-utils/io/Log.h>
#include <nch/sdl-utils/Timer.h>

STexture::STexture(){}
STexture::~STexture(){}

void STexture::init(SDLHandler* p_sdlHandler, int texWidth, int texHeight, double texScale)
{
    //Helper class pointers
    sdlHandler = p_sdlHandler;
    pixelFormat = sdlHandler->getPixelFormat()->format;
    renderer = sdlHandler->getRenderer();
    textureLoader = sdlHandler->getTextureLoader();

    access = SDL_TEXTUREACCESS_STREAMING;
    texW = texWidth;
    texH = texHeight;
    STexture::drawScale = texScale;
    
    tex = SDL_CreateTexture(renderer, pixelFormat, access, texW, texH);
}

void STexture::init(SDLHandler* p_sdlHandler, int texWidth, int texHeight)
{ init(p_sdlHandler, texWidth, texHeight, 1); }

void STexture::lock()
{
    locked = true;

    //Set lockarea to be the entire texture
    Texture::setLock(0, 0, texW, texH);

    //lock entire texture
    if( SDL_LockTexture(tex, NULL, &pixels, &pitch)<0 ) {
        NCH_Log::errorv(__PRETTY_FUNCTION__, SDL_GetError(), "Texture locking failed");
        NCH_Log::throwException();
    }
}

void STexture::unlock()
{
    SDL_UnlockTexture(tex);
    locked = false;
}

void STexture::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if(!locked) return;
    if(x>=texW || x<0) return;
    if(y>=texH || y<0) return;

    ((Uint32*)pixels)[x+(y*texW)] = SDL_MapRGBA(sdlHandler->getPixelFormat(), r, g, b, a);
}
void STexture::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    pixel(x, y, r, g, b, 255);
}