#pragma once
#include "Button.h"

class CheckBox : public Button
{
public:
    CheckBox(Window* parentWindow, int x, int y, std::string text, int checkBoxState, int id);
    CheckBox(Window* parentWindow, int x, int y, std::string text, int id);
    CheckBox(Window* parentWindow, int x, int y, int checkBoxState, int id);
    CheckBox(Window* parentWindow, int x, int y, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    virtual ~CheckBox();

    void draw();
    void onWindowUpdate();
    void tick();

    void cycleState();
    void setCanLeaveBlank(bool clb);
    void setCanShuffle(bool cs);

    enum States {
        BLANK = 0,
        FALSE,
        TRUE,
        SHUFFLE,
        UNKNOWN
    };

protected:

private:

    bool texNeedsUpdate = false;
    int blitBG = 0; //Background
    int blitCO = 0; //Click Overlay

    int checkBoxState = 0;         //0 = blank, 1=false, 2=true, 3=shuffle, anything else=unknown
    bool canLeaveBlank = false;
    bool canShuffle = false;
};
