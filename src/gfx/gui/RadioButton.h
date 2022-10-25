#pragma once
#include "Button.h"

class RadioButton : public Button
{
public:
    RadioButton(Window* p_parentWindow, int x, int y, std::string s, int id, int minGMID, int maxGMID);
    virtual ~RadioButton();

    int getMinGroupMemberID();
    int getMaxGroupMemberID();

    void deselect();
protected:

private:
    int minGroupMemberID = -10000;
    int maxGroupMemberID = -10000;
};
