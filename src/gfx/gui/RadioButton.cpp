#include "RadioButton.h"

RadioButton::RadioButton(int p_x, int p_y, int p_width, int p_id, int p_minGMID, int p_maxGMID)
{
    setType(BTEObject::Type::GUI_radiobutton);

    //Take in p_x, p_w
    sX = p_x/2*2;
    sY = p_y/2*2;
    //Take in p_w
    width = (p_width/64*64);
    if(p_width<8) width = 8;
    //Take in p_btnID
    setID(p_id);
    minGroupMemberID = p_minGMID;
    maxGroupMemberID = p_maxGMID;

    height = 16;

    buttonType = RADIO;
}

RadioButton::~RadioButton()
{
    //dtor
}

int RadioButton::getMinGroupMemberID() { return minGroupMemberID; }
int RadioButton::getMaxGroupMemberID() { return maxGroupMemberID; }
