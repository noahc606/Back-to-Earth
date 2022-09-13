#pragma once
#include "GUIAlignable.h"
#include "Window.h"

class WindowComponent : public GUIAlignable
{
public:
    /**/
    WindowComponent();
    WindowComponent(Window* p_parentWindow, int p_x, int p_y, int p_width);
    WindowComponent(Window* p_parentWindow, int p_x, int p_y);
    virtual ~WindowComponent();
    /**/
    void draw();
    void tick();
    /**/
    int getTX(); int getTY();
    int getTPos(bool dim);              //Get tX or tY

    Window* getParentWindow();

    void setPos(int x, int y);
    virtual void updateProperties(bool preventInvalidTPos);

protected:
    Window* parentWindow = nullptr;
    int tX = 0; int tY = 0;             //Position relative to the ingame GUI window (NOT computer's window). Negative only in special circumstances (special alignment)
    int scale = 2;

private:
};
