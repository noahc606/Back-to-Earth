#pragma once
#include "SDLHandler.h"
#include <map>
#include <vector>
#include "GUIHandler.h"

class PlayerMenuModules {
public:
    enum WidgetID {
        PLASMA_MATTER_TRANSFORMER,
        MISSION_LOG,
        CRAFTING,
        SANDBOX
    };

    PlayerMenuModules();
    ~PlayerMenuModules();

    int widgetClicked(int playerGamemode, int shx, int shy);
    void drawWidgets(SDLHandler* sh, int playerGamemode, int selectedWidgetID, int isx, int isy, int odx, int ody);
    void putMenuInterface(GUIHandler* gh, nch::Color sandboxRGB, int widgetID);
    void putMenuInterface(GUIHandler* gh, int widgetID);
private:
    std::map<int, std::vector<int>> widgetsGroupList;
};