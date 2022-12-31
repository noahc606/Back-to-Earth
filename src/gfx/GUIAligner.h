#pragma once
#include "GUIHandler.h"
#include <set>

class GUIAligner : GUIHandler
{
public:
    static void alignWindows(SDLHandler* sdlHandler, std::vector<GUI*>& guis);
    static void alignWindowComponents(std::vector<GUI*>& guis, int align);

protected:

private:
    static void alignWindowComponentsVH(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords);
    static void alignWindowComponentsLR(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords);
    static void alignWindowComponentsUD(std::vector<GUI*>& guis, int align, std::set<GUI*>& windows, std::set<int>& compCoords);
};
