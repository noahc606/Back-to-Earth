#pragma once
#include "GUIHandler.h"
#include "SDLHandler.h"
#include "World.h"

class ButtonAction
{
public:
    ButtonAction(SDLHandler* sh, GUIHandler* gh);
    virtual ~ButtonAction();

protected:

private:
};
