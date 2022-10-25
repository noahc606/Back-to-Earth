#include "RadioButton.h"

RadioButton::RadioButton(Window* p_parentWindow, int p_x, int p_y, std::string p_s, int p_id, int p_minGMID, int p_maxGMID)
: Button::Button(p_parentWindow, p_x, p_y, 8, p_s, p_id )
{
    setSubType(BTEObject::Type::GUI_radiobutton);

    buttonType = RADIO;

    minGroupMemberID = p_minGMID;
    maxGroupMemberID = p_maxGMID;
}

RadioButton::~RadioButton()
{
    //dtor
}

int RadioButton::getMinGroupMemberID() { return minGroupMemberID; }
int RadioButton::getMaxGroupMemberID() { return maxGroupMemberID; }

void RadioButton::deselect() { selected = false; }
