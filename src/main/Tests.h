#pragma once
#include <vector>
#include "BTEObject.h"
#include "DebugObject.h"
#include "SpriteSheet.h"
#include "STexture.h"
#include "TTexture.h"
#include "Texture.h"
#include "TileRegion.h"

class Tests : public BTEObject, public DebugObject
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
    /**/
    void info(std::stringstream& ss, int& tabs);

protected:

private:
    /**/
    /**/
    /**/
    /* Testing */

    Texture tex;

    STexture stex;
    TTexture ttex;
    SpriteSheet spsh;

    Window* win;

    int counter = 0;
};
