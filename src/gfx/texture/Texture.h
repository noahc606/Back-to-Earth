#pragma once
#include <cstdint>
#include <SDL.h>
#include "Color.h"
#include "SDLHandler.h"

class Texture
{
public:
    Texture();
    virtual ~Texture();

    void init(SDLHandler* sdlHandler);
    void destroy();

    void test();

    bool isInitialized();
    SDL_Texture* getSDLTexture();

    int getTexWidth();
    int getTexHeight();
    double getDrawScale();
    void queryTexInfo(int &texW, int &texH);
    void queryDrawInfo(int &drawX, int &drawY, double &drawScale);


    //Texture manipulation
    void lock();
    void lock(SDL_Rect* rect);
    void lock(int x, int y, int w, int h);
    void unlock();
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
    void blit(int id);
    void fill(SDL_BlendMode bm);
    void fill();
    void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
    void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    void clear();

    void setColorMod();
    void setColorMod(const Color& c);
    void setColorMod(uint8_t r, uint8_t g, uint8_t b);
    void setColorMod(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void setBlendMode(SDL_BlendMode bm);

    //Texture transformations
    void setTexDimensions(int w, int h, bool scale);
    void setTexDimensions(int w, int h);
    void setTexWidth(int newWidth, bool scale);
    void setTexWidth(int newWidth);
    void setTexHeight(int newHeight, bool scale);
    void setTexHeight(int newHeight);

    void setTexRect(int x, int y, int w, int h);
    void setTexRect(SDL_Rect* rect);

    void setDrawX(int x);
    void setDrawY(int y);
    void setDrawPos(int x, int y);
    void setDrawScale(double scale);
    void translate(int dX, int dY);
    void dilate(double factor);

    //Texture rendering
    void draw();
    void draw(SDL_Rect* src, SDL_Rect* dst);


    enum Flip {
        NONE = 1001,
        H    = 1002,
        V    = 1003,
        HV   = 1006,
    };
protected:

private:
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

    //Locking information
    int lockedXPos = 0;
    int lockedYPos = 0;
    int lockedWidth = 0;
    int lockedHeight = 0;

    //Texture width and height
    int texW = 0;
    int texH = 0;

    //Texture rendering properties
    int drawX = 0;
    int drawY = 0;
    double drawScale = 1;

    //Private helper methods
    void initSDLInfo();
    void initTex(bool scale);

    void validateLock();
};
