#pragma once
#include "SDLHandler.h"
#include <map>
#include <vector>
#include "GUIHandler.h"
#include "MissionHolder.h"

class PlayerMenuModules {
public:
    enum WidgetID {
        PLASMA_MATTER_TRANSFORMER,
        MISSION_LOG,
        CRAFTING,
        SANDBOX,
        BACKPACK,
        STARMAP,
    };

    PlayerMenuModules();
    ~PlayerMenuModules();

    void drawWidgets(SDLHandler* sh, int playerGamemode, int selectedWidgetID, int isx, int isy, int odx, int ody);
    void drawModuleSandbox(Texture& uiOverlay, SDL_Rect invRect, int invSX, int invSY);
    void drawModuleMissionLog(SDLHandler* sh, SDL_Rect invRect, MissionHolder& mh);

    std::string getWidgetHoverText(int playerGameMode, int shx, int shy);
    int widgetClicked(int playerGamemode, int shx, int shy);
    
    void putMenuInterface(GUIHandler* gh, nch::Color sandboxRGB, int widgetID);
    void putMenuInterface(GUIHandler* gh, int widgetID);
private:
    std::map<int, std::vector<int>> widgetsGroupList;
};