#pragma once
#include <cstdint>
#include <SDL.h>
#include "Color.h"
#include "FileHandler.h"
#include "SDLHandler.h"

//Texture is a parent class of these base classes:
//  ATexture Static texture
//  STexture Streaming texture
//  TTexture Target texture (A generic Texture is basically the same as a TTexture)

class Texture
{
public:
    /**/
    Texture();
    virtual ~Texture();
    void init(SDLHandler* sdlHandler);
    void init(SDLHandler* sdlHandler, int texWidth, int texHeight);
    void init(SDLHandler* sdlHandler, int texWidth, int texHeight, int texScale);
    void destroy();
    /**/
    bool isInitialized();
    SDL_Texture* getSDLTexture();
    SDL_Surface* createSurfaceFromTexture();
    int getTexWidth();
    int getTexHeight();
    double getDrawScale();
    int getBlendMode();
    void queryTexInfo(int &texW, int &texH);
    void queryDrawInfo(int &drawX, int &drawY, double &drawScale);

    /**//**/
    /** Texture manipulation */
    /*
        Locking functions:
        setlock():
            Set Lock values directly without restriction (faster but unsafe)
        lock():
            For a TARGET texture, the lock functions simply set the destination for any future blits.
            For a STREAMING texture, the lock functions do the above and also store the current pixel data of the locked area into lockedPixels.
            Checks are made so that w+x does not exceed tex.w, values are not negative, etc.
        unlock():
            Sets lock data/destination to (0, 0, 0, 0) and frees lockedPixels.
    */
    virtual void setLock(int lx, int ly, int lw, int lh);
    virtual void lock(int x, int y, int w, int h);
    virtual void lock(SDL_Rect* rect);
    virtual void lock();
    virtual void unlock();
    /*
        Blitting funtions:
        blitEx():
            For a TARGET texture you can blit a rotated or flipped version of a src image onto the locked area
                Note: It may happen that a rotated pixel of an image gets blitted outside the rectangular locked region.
            For a STREAMING texture this function does nothing - blitting rotated images without SDL_BlitEx is complicated.
        blit():
            Blit a rectangular area of the source image onto the locked area (destination) specified by lock().
                TARGET and STREAMING textures effectively have the same behavior.
    */
    void blitEx(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH, int mod);
    void blitEx(Texture* src, int srcX, int srcY, int srcW, int srcH, int mod);
    void blitEx(SDL_Texture* src, int srcX, int srcY, int mod);
    void blitEx(Texture* src, int srcX, int srcY, int mod);
    void blitEx(int id, int srcX, int srcY, int mod);
    void blit(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH);
    void blit(Texture* src, int srcX, int srcY, int srcW, int srcH);
    void blit(int id, int srcX, int srcY, int srcW, int srcH);
    void blit(SDL_Texture* src, int srcX, int srcY);
    void blit(Texture* src, int srcX, int srcY);
    void blit(int id, int srcX, int srcY);
    void blit(SDL_Texture* src);
    void blit(int id);
    /*
        Simple blit functions:
        fill():
            Fill the locked area (destination) specified by lock() with a single color specified with setColorMod().
        rect():
            Fill a rectangular area separate from the locked() area. old lock data is preserved.
        pixel():
            Set a single pixel to any color.

        Note:
            For TARGET, All of these functions use a 1x1 white pixel that is then colored/scaled onto the destination image.
                BlendMode is also available for TARGET textures
            For STREAMING, we access the pixel data directly and set each pixel to a single color.
                BlendMode has no effect for STREAMING textures
    */
    virtual void fill(SDL_BlendMode bm);
    virtual void fill();
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    virtual void pixel(int x, int y, uint32_t rgba);
    /*
        Clears the texture.
            For a TARGET texture we simply call SDL_RenderClear().
            For a STREAMING texture we fill in the pixel data with (0,0,0,0) which gives the same result
    */
    virtual void clear();

    virtual void setColorMod();
    virtual void setColorMod(const Color& c);
    virtual void setColorMod(uint8_t r, uint8_t g, uint8_t b);
    virtual void setColorMod(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void setBlendMode(SDL_BlendMode bm);

    /** Texture transformations */
    virtual void setTexDimensions(int w, int h, bool scale);
    virtual void setTexDimensions(int w, int h);
    virtual void setTexWidth(int newWidth, bool scale);
    virtual void setTexWidth(int newWidth);
    virtual void setTexHeight(int newHeight, bool scale);
    virtual void setTexHeight(int newHeight);

    virtual void setTexRect(int x, int y, int w, int h);
    virtual void setTexRect(SDL_Rect* rect);

    virtual void setDrawX(int x);
    virtual void setDrawY(int y);
    virtual void setDrawPos(int x, int y);
    virtual void setDrawScale(double scale);
    virtual void translate(int dX, int dY);
    virtual void dilate(double factor);

    /** Texture rendering */
    virtual void draw();
    virtual void draw(SDL_Rect* src, SDL_Rect* dst);
    virtual void savePNG(FileHandler* fh, std::string path);

    enum Flip {
        NONE = 1001,
        H    = 1002,
        V    = 1003,
        HV   = 1006,
    };

protected:
    /**/
    /**/
    //Private helper methods
    void initSDLInfo();
    void initTex(bool scale);
    void validateLock();
    void validateTexSize();
    /**/

    //Texture's SDL info
    bool limitTexSize = true;
    int texSizeLimit = 2048;

    SDLHandler* sdlHandler;
    SDL_Renderer* renderer;
    TextureLoader* textureLoader;
    uint32_t pixelFormat;

    //Texture's SDL_Surface and SDL_Texture information
    bool initialized = false;
    SDL_Texture* tex = nullptr;
    int access = SDL_TEXTUREACCESS_TARGET;
    SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;
    Color colorMod;

    //Texture locking data
    /**
    This is used for TARGET textures*/
    SDL_Rect lockArea;
    /**
    These three are used for STREAMING textures*/
    uint32_t* dstPixels = nullptr;
    int dstPitch;
    bool keepingLastData = true;                //Keep last pixel data of locked area: If you're filling an entire rectangle with pixels, obviously this can be set to false.

    //Texture width and height
    int texW = 0;
    int texH = 0;

    //Texture rendering properties
    int drawX = 0;
    int drawY = 0;
    double drawScale = 1;
private:

};
