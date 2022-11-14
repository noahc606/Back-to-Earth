#pragma once
#include <cstdint>
#include <SDL.h>
#include "Color.h"
#include "FileHandler.h"
#include "SDLHandler.h"

class Texture
{
public:
    /**/
    //ATexture Static texture
    //STexture Streaming texture
    //TTexture Target texture

    Texture();
    virtual ~Texture();
    void init(SDLHandler* sdlHandler);
    void destroy();
    /**/
    bool isInitialized();
    SDL_Texture* getSDLTexture();
    SDL_Surface* createSurfaceFromTexture();
    int getTexWidth();
    int getTexHeight();
    double getDrawScale();
    void queryTexInfo(int &texW, int &texH);
    void queryDrawInfo(int &drawX, int &drawY, double &drawScale);
    /**/
    //Texture manipulation
    virtual void lock();
    virtual void lock(SDL_Rect* rect);
    virtual void lock(int x, int y, int w, int h);
    virtual void unlock();
    virtual void blitEx(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH, int mod);
    virtual void blitEx(Texture* src, int srcX, int srcY, int srcW, int srcH, int mod);
    virtual void blitEx(SDL_Texture* src, int srcX, int srcY, int mod);
    virtual void blitEx(Texture* src, int srcX, int srcY, int mod);
    virtual void blitEx(int id, int srcX, int srcY, int mod);
    virtual void blit(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH);
    virtual void blit(Texture* src, int srcX, int srcY, int srcW, int srcH);
    virtual void blit(int id, int srcX, int srcY, int srcW, int srcH);
    virtual void blit(SDL_Texture* src, int srcX, int srcY);
    virtual void blit(Texture* src, int srcX, int srcY);
    virtual void blit(int id, int srcX, int srcY);
    virtual void blit(int id);
    virtual void fill(SDL_BlendMode bm);
    virtual void fill();
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b);
    virtual void pixel(int x, int y, uint32_t rgba);
    virtual void clear();

    virtual void setColorMod();
    virtual void setColorMod(const Color& c);
    virtual void setColorMod(uint8_t r, uint8_t g, uint8_t b);
    virtual void setColorMod(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    virtual void setBlendMode(SDL_BlendMode bm);

    //Texture transformations
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

    /**/
    //Texture rendering
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
private:

};
