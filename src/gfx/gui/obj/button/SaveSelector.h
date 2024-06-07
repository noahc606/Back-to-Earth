#pragma once
#include "Button.h"

class SaveSelector : public Button
{
public:
    SaveSelector(Window* parentWindow, int index, std::string dir, std::string worldName, std::string sizeDesc, int id);
    void init(SDLHandler* sh, Controls* ctrls);

    void tick();
    void onWindowUpdate();
    void draw();

    int getActionID();
    std::pair<int, std::string> getActionInfo();

    void resetActionID(std::string methodName);
private:

    int relX = 0;
    int relY = 0;
    int actionID = -1;
    std::pair<int, std::string> actionInfo = std::make_pair(-1, "");


    std::string worldDirName = "";
    std::string worldName = "";
    std::string worldDirPath = "";
    std::string worldSizeDesc = "";
    int index = -100;
};