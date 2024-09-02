#pragma once
#include "Button.h"
#include "GUIHandler.h"

class ColorSelector : public Button
{
public:
    ColorSelector(Window* parentWindow, int x, int y, nch::Color col, int id);
    ColorSelector(Window* parentWindow, int x, int y, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    void destroy();
    void buildTexes();
    void setSelectorWindow(Window* sw);

    void draw();
    void drawExtras();
    void onWindowUpdate();
    void tick();
    void updateColAreaXY(int colAreaX, int colAreaY);
    void syncWithTextboxes(GUIHandler* guih);

    double getHue(); double getSat(); double getVal();
    bool justClicked();

    void unclick();
    void deselect();
    void setSatValFromXY(int selX, int selY);
    void setHue(double newHue); void setSat(double newSat); void setVal(double newVal);

private:
    Window* selectorWindow = nullptr;
    Texture texCrosshair;
    Texture texSelectorEdges;
    int colAreaX = -100000;
    int colAreaY = -100000;

    std::pair<bool, bool> drawStateLast = std::make_pair(false, false);
    std::pair<bool, bool> drawState = std::make_pair(false, false);

    nch::Color color;
    double lastHue = 0; double hue = 50; uint64_t lastHueUpdate = 0;
    double lastSat = 0; double sat = 50;
    double lastVal = 0; double val = 50;
};