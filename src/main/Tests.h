#pragma once
#include <vector>
#include "SpriteSheet.h"
#include "Texture.h"
#include "BTEObject.h"
#include "DebugObject.h"

class Tests : public BTEObject, public DebugObject
{
public:
    /**/
    Tests();
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual ~Tests();
    /**/
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
    SpriteSheet spsh;

    Window* win;

    int counter = 0;
};
