#pragma once
#include "GUIAlignable.h"
#include "Text.h"
#include "Texture.h"
#include "WindowPanelData.h"

class Window : public GUIAlignable
{
public:
    /**/
    Window(int x, int y, WindowPanelData* panelData, int id);
    Window(int x, int y, int width, int height, std::string upperPanel, std::string lowerPanel, int id);
    Window(int id);
    void init(SDLHandler* sh, Controls* ctrls);
    void destroy();
    /**/
    void draw();
    void tick();
    void onWindowUpdate(bool preventInvalidTPos);
    /**/
    WindowPanelData* getWindowPanelData();

protected:

private:

    Texture windowTex;
    Text upperPanelText;
    Text lowerPanelText;
    WindowPanelData* panelData = nullptr;

    bool bkgd = false;
};
