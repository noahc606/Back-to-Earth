#pragma once
#include "Button.h"

class CheckBox : public Button
{
public:
    CheckBox(Window* parentWindow, int x, int y, std::string text, int checkBoxState, bool largerButton, int id);
    CheckBox(Window* parentWindow, int x, int y, std::string text, int checkBoxState, int id);
    CheckBox(Window* parentWindow, int x, int y, std::string text, std::string checkBoxState, int id);
    CheckBox(Window* parentWindow, int x, int y, std::string text, int id);
    CheckBox(Window* parentWindow, int x, int y, int checkBoxState, int id);
    CheckBox(Window* parentWindow, int x, int y, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    void destroy();
    void buildTexes();
    virtual ~CheckBox();

    void draw();
    void onWindowUpdate();
    void tick();

    static int getStateFromString(std::string s);
    int getState();
    bool isLargerButton();
    bool justClicked();

    void cycleState();
    void unclick();
    void setLargerButton(bool isLargerButton);
    void setCanLeaveBlank(bool clb);
    void setCanShuffle(bool cs);

    enum States {
        CBX_BLANK = 0,
        CBX_FALSE,
        CBX_TRUE,
        CBX_SHUFFLE,
        CBX_RESET,
        CBX_UNKNOWN
    };

protected:

private:

    bool texNeedsUpdate = false;
    int blitBG = 0; //Background
    int blitCO = 0; //Click Overlay

    int state = 0;         //0 = blank, 1=false, 2=true, 3=shuffle, anything else=unknown
    bool largerButton = true;
    bool canLeaveBlank = false;
    bool canShuffle = false;
};
