#pragma once
#include "GUIAlignable.h"
#include "Text.h"
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

protected:

private:
    Texture windowTex;
    Text upperPanelText;
    Text lowerPanelText;
    WindowData* winData = nullptr;

    bool bkgd = false;
    static int bkgdScroll;
};
