#pragma once
#include "GUIHandler.h"

class GUIAligner : GUIHandler
{
public:
    static void alignWindows(SDLHandler* sdlHandler, std::vector<GUI*>& guis);
    static void alignWindowComponents(std::vector<GUI*>& guis, int align);

protected:

private:
};
