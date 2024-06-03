#pragma once
#include "WindowComponent.h"

class ProgressBar : public WindowComponent {
public:
    ProgressBar(Window* parentWindow, int x, int y, int width, int id);
    void init(SDLHandler* sh);
    void destroy();
    void draw();
    void tick();
    void onWindowUpdate();

    void setProgress(int numTasksDone);
    void setProgressTotal(int numTasksTotal);
private:

    int numTasksDone = 0;
    int numTasksDoneLast = 0;
    int numTasksTotal = 100;

    Texture pbrTex;
    Texture pbrTexOverlay;
};