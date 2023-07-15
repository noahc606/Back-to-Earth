#pragma once
#include "Controls.h"
#include "GUIHandler.h"
#include "SDLHandler.h"
#include "World.h"

class ButtonAction
{
public:
    ButtonAction(SDLHandler* sh,  GUIHandler* gh, FileHandler* fh, Controls* ctrls);
    virtual ~ButtonAction();

protected:

private:
};
