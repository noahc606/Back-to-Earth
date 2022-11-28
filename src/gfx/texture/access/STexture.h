#pragma once
#include "Texture.h"
#include "SpriteSheet.h"

class STexture : public Texture
{
public:
    /**/
    STexture();
    virtual ~STexture();
    void init(SDLHandler* sdlHandler);
    /**/

    void lock(SDL_Rect l);
    void lock(int lx, int ly, int lw, int lh);
    void lock();
    void unlock();

    void blit(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH);
    void fblit(SpriteSheet* src, int srcX, int srcY);
    void blit(SpriteSheet* src, int srcX, int srcY);
    void blit(SpriteSheet* src);

    void blitRedAndBlue();



    void fill(uint32_t rgba);
    void fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void fill(uint8_t r, uint8_t g, uint8_t b);

    //Streaming texture must be updated unlike others
    void update(SDL_Rect ur);
    void update();
    /**/

protected:

private:
};
