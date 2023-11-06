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
    int getTX(); int getTY();   //Get tX or tY
    int getTPos(bool dim);      //Get both tX or tY
    Window* getParentWindow();  //Get parent window of this window component
    /**/
    void setTX(int tx); void setTY(int ty);                 //Set tX or tY
    void setTPos(int tx, int ty);                           //Set both tX and Ty
    virtual void translateSPos();

protected:
    Window* parentWindow = nullptr;
    int tX = 0; int tY = 0;         //Position relative to the ingame GUI window (NOT computer's window). Negative only in special circumstances (special alignment)

private:
};
