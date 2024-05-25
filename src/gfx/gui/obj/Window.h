#pragma once
#include "GUIAlignable.h"
#include "TextOld.h"
#include "Texture.h"
#include "WindowData.h"

class Window : public GUIAlignable
{
public:
    /**/
    Window(int x, int y, WindowData* winData, int id);
    Window(int x, int y, int width, int height, std::string upperPanel, std::string lowerPanel, int id);
    Window(int id);
    virtual ~Window();
    void init(SDLHandler* sh, Controls* ctrls);
    void destroy();
    /**/
    void draw();
    void tick();
    void onWindowUpdate(bool preventInvalidTPos);
    /**/
    WindowData* getWindowData();
    bool isActive();
    /**/
    void setActive(bool isActive);
    void updateColorSelectorUI(double hue);

protected:

private:
    Texture windowTex;
    TextOld upperPanelText;
    TextOld lowerPanelText;
    WindowData* winData = nullptr;

    bool active = true;
    static int bkgdScroll;
};
