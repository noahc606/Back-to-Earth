#include "STexture.h"
#include "Log.h"
#include "Timer.h"

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
    
    tex = SDL_CreateTexture(renderer, pixelFormat, access, texW, texH);

    //drawScale = texScale;
    //Texture::init(p_sdlHandler);
}

void STexture::init(SDLHandler* p_sdlHandler, int texWidth, int texHeight)
{ init(p_sdlHandler, texWidth, texHeight, 1); }

/*
void STexture::lock(SDL_Rect lr)
{
    locked = true;

    //Set lockArea from lr (clipped so that xywh are a rectangle within the texture)
    Texture::lock(lr.x, lr.y, lr.w, lr.h);

    //SDL lock with validated lockArea
    if( SDL_LockTexture(tex, &lockArea, &dstPixels, &dstPitch)<0 ) {
        Log::errorv(__PRETTY_FUNCTION__, SDL_GetError(), "Texture locking failed");
        Log::throwException();
    }
}

void STexture::lock(int lx, int ly, int lw, int lh)
{
    SDL_Rect lr;
    lr.x = lx; lr.y = ly; lr.w = lw; lr.h = lh;

    lock(lr);
}
*/

void STexture::lock()
{
    locked = true;

    //Set lockarea to be the entire texture
    Texture::setLock(0, 0, texW, texH);

    //lock entire texture
    if( SDL_LockTexture(tex, NULL, &pixels, &pitch)<0 ) {
        Log::errorv(__PRETTY_FUNCTION__, SDL_GetError(), "Texture locking failed");
        Log::throwException();
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
    ((Uint32*)pixels)[x+(y*texW)] = SDL_MapRGB(sdlHandler->getPixelFormat(), r, g, b);
}

/*
void STexture::allocDestinationPixels()
{
    dstPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*dstPixels));
}
*/
