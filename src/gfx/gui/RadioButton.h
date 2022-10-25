#pragma once
#include "Button.h"

class RadioButton : public Button
{
public:
    RadioButton(int x, int y, int width, int id, int minGMID, int maxGMID);
    virtual ~RadioButton();

    int getMinGroupMemberID();
    int getMaxGroupMemberID();

    void unselect();
protected:

private:
    int minGroupMemberID = -10000;
    int maxGroupMemberID = -10000;

};
