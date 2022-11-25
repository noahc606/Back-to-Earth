#pragma once
#include "Texture.h"

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

    void blit();

    //Streaming texture must be updated unlike others
    void update(SDL_Rect ur);
    void update();
    /**/

protected:

private:
};
