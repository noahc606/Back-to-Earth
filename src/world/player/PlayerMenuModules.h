#pragma once
#include "SDLHandler.h"
#include <map>
#include <vector>
#include "GUIHandler.h"
#include "MissionHolder.h"
#include "TileEntity.h"

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

    void setDrawPos(int scrX, int scrY);
    void drawWidgets(SDLHandler* sh, int playerGamemode, int selectedWidgetID);
    void drawModuleSandbox(Texture& uiOverlay, SDL_Rect invRect, int invSX, int invSY);
    void drawModuleMissionLog(SDLHandler* sh, SDL_Rect invRect, MissionHolder& mh);

    std::string getWidgetHoverText(int playerGameMode, int shx, int shy);
    int widgetClicked(int playerGamemode, int shx, int shy);
    
    void setInteractionType(int tety);

    void putMenuInterface(GUIHandler* gh, nch::Color sandboxRGB, int widgetID);
    void putMenuInterface(GUIHandler* gh, int widgetID);
private:
    std::map<int, std::vector<int>> widgetsGroupList;
    int tety = TileEntity::NONE;

    int scrX = 0, scrY = 0;
};