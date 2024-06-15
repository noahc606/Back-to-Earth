#pragma once
#include "WindowComponent.h"
#include "CurlHandler.h"

class ProgressBar : public WindowComponent {
public:
    ProgressBar(Window* parentWindow, int x, int y, int width, int id);
    void init(SDLHandler* sh);
    void initWorkTypeA(CurlHandler* ch, std::vector<std::string>* assetList, bool forceDisableUpdateDLs);
    void destroy();
    void draw();
    void rebuildTexOverlay();
    void tick();
    void onWindowUpdate();

    void resetWorkFinished();

    bool hasWorkToDo();
    bool isWorkFinished();
private:

    CurlHandler* ch = nullptr;
    std::vector<std::string>* assetList = nullptr;
    bool forceDisableUpdateDLs = false;

    int currentTask = 0;
    int totalTasks = 100;
    std::string currentTaskDesc = "";

    char workType = ' ';
    bool workToDo = false;
    bool workFinished = false;

    Texture pbrTex;
    Texture pbrTexOverlay;
};