#pragma once
#include <vector>
#include "BTEObject.h"
#include "CurlHandler.h"
#include "Loggable.h"
#include "SpriteSheet.h"
#include "STexture.h"
#include "TTexture.h"
#include "Texture.h"
#include "MissionHolder.h"
#include "TileRegion.h"

class Tests : public BTEObject, public Loggable
{
public:
    /**/
    Tests();
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual ~Tests();
    /**/
    void thing1();
    void draw();
    void tick();
    void putInfo(std::stringstream& ss, int& tabs);
    /**/

protected:

private:
    /**/
    /**/
    /**/
    /* Testing */

    MissionHolder mh;

    AudioLoader* al = nullptr;

    STexture tex;
    SDL_Texture* stex;
	SDL_Rect lvlImgSrc;

    CurlHandler ch;
    SpriteSheet spsh;

    int counter = 0;
};
