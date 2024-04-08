#pragma once
#include "Button.h"
#include "GUIHandler.h"

class ColorSelector : public Button
{
public:
    ColorSelector(Window* parentWindow, int x, int y, Color col, int id);
    ColorSelector(Window* parentWindow, int x, int y, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    void buildTexes();

    void draw();
    void updateColorSelectorUI(Texture* windowTex);
    void drawCrosshair(int colAreaX, int colAreaY);
    void onWindowUpdate();
    void tick();
    void trackMouse(int colAreaX, int colAreaY);
    void syncWithTextboxes(GUIHandler* guih);

    double getSat(); double getVal();
    bool justClicked();

    void unclick();
    void setSVFromXY(int selX, int selY);

private:
    Texture* texCrosshair = nullptr;

    std::pair<bool, bool> drawStateLast = std::make_pair(false, false);
    std::pair<bool, bool> drawState = std::make_pair(false, false);

    Color color;
    double lastHue = 0; double hue = 100;
    double lastSat = 0; double sat = 50;
    double lastVal = 0; double val = 50;
};