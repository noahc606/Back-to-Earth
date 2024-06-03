#pragma once
#include "Button.h"
#include "Texture.h"

class RadioButton : public Button
{
public:
    RadioButton(Window* p_parentWindow, int x, int y, std::string s, bool selected, int id, int minGMID, int maxGMID);
    RadioButton(Window* p_parentWindow, int x, int y, std::string s, int id, int minGMID, int maxGMID);
    void init(SDLHandler* sh, Controls* ctrls);

    bool justClicked();
    int getMinGroupMemberID();
    int getMaxGroupMemberID();

    void unclick();
    void deselect();
    void draw();

    void onWindowUpdate();
protected:

private:
    int minGroupMemberID = -10000;
    int maxGroupMemberID = -10000;

    Texture texRB;      //Radio button not hovering
    Texture texRBH;     //Radio button hovering
    Texture texRBS;     //Radio button selected overlay
    Texture texRBSH;    //Radio button selected + hovering overlay


    Texture texRBHRect; //Radio button hovering overlay (rectangle)
    Texture texRBCO;    //Radio button click overlay
};
