#include "RadioButton.h"
#include "TextureLoader.h"
#include "TextureBuilder.h"

RadioButton::RadioButton(Window* p_parentWindow, int p_x, int p_y, std::string p_s, bool p_selected, int p_id, int p_minGMID, int p_maxGMID)
: Button::Button(p_parentWindow, p_x, p_y, 8, p_s, p_id )
{
    setSubType(BTEObject::Type::GUI_radiobutton);

    buttonType = RADIO;

    minGroupMemberID = p_minGMID;
    maxGroupMemberID = p_maxGMID;

    width = 10;
    height = 10;

    selected = p_selected;
}

RadioButton::RadioButton(Window* p_parentWindow, int p_x, int p_y, std::string p_s, int p_id, int p_minGMID, int p_maxGMID):
RadioButton(p_parentWindow, p_x, p_y, p_s, false, p_id, p_minGMID, p_maxGMID ){}

void RadioButton::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    //Build Tex
    btnText.init(sdlHandler);
    btnText.setString(btnString);

    TextureBuilder tb(sdlHandler);
    tb.buildRadioButton(texRbtn, 33, 68);
    tb.buildRadioButton(texRbtnHovering, 44, 68);
    tb.buildRadioButton(texRbtnSelected, 33, 79);
    tb.buildRadioButton(texRbtnSelectedH, 44, 79);

    tb.buildRadioButton(texRbtnClickOverlay, Color(100, 50, 100), 55, 68);
}

void RadioButton::draw()
{
    if(hovering) {
        texRbtnHovering.draw();
    } else {
        texRbtn.draw();
    }

    if( selected ) {
        if(hovering) {
            texRbtnSelectedH.draw();
        } else {
            texRbtnSelected.draw();
        }
    }

    if(clicked) {
        texRbtnClickOverlay.draw();
    }

    btnText.draw();
}

bool RadioButton::justClicked() { return clicked; }
int RadioButton::getMinGroupMemberID() { return minGroupMemberID; }
int RadioButton::getMaxGroupMemberID() { return maxGroupMemberID; }

void RadioButton::unclick() { clicked = false; }
void RadioButton::deselect() { selected = false; }

void RadioButton::onWindowUpdate()
{
    translateSPos();
    texRbtn.setDrawPos(sX, sY);
    texRbtnHovering.setDrawPos(sX, sY);
    texRbtnSelected.setDrawPos(sX, sY);
    texRbtnSelectedH.setDrawPos(sX, sY);
    texRbtnClickOverlay.setDrawPos(sX, sY);

    btnText.setPos(sX+28, sY+4);
}
