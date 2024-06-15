#include "RadioButton.h"
#include "TextureLoader.h"
#include "TextureBuilder.h"

RadioButton::RadioButton(Window* p_parentWindow, int p_x, int p_y, std::string p_s, bool p_selected, int p_id, int p_minGMID, int p_maxGMID)
: Button::Button(p_parentWindow, p_x, p_y, 0, p_s, p_id )
{
    setSubType(BTEObject::Type::GUI_radiobutton);

    minGroupMemberID = p_minGMID;
    maxGroupMemberID = p_maxGMID;

    selected = p_selected;
}

RadioButton::RadioButton(Window* p_parentWindow, int p_x, int p_y, std::string p_s, int p_id, int p_minGMID, int p_maxGMID):
RadioButton(p_parentWindow, p_x, p_y, p_s, false, p_id, p_minGMID, p_maxGMID ){}

void RadioButton::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    //Build Tex
    btnText.init(sdlHandler);
    btnText.setString(btnInitString);
    width = btnText.getWidth()/2+2*24;
    if(width<2*32) width = 2*32;

    texRB.init(sdlHandler, 10, 10, 2);
    texRB.lock(); texRB.blit(TextureLoader::GUI_button, 33, 68);

    texRBH.init(sdlHandler, 10, 10, 2);
    texRBH.lock(); texRBH.blit(TextureLoader::GUI_button, 44, 68);

    texRBS.init(sdlHandler, 10, 10, 2);
    texRBS.lock(); texRBS.blit(TextureLoader::GUI_button, 33, 79);

    texRBSH.init(sdlHandler, 10, 10, 2);
    texRBSH.lock(); texRBSH.blit(TextureLoader::GUI_button, 44, 79);


    texRBHRect.init(sdlHandler, width/2, 16, 2);
    texRBHRect.setColorMod(100, 95, 51);
    texRBHRect.lock(0, 0, 3, 16); texRBHRect.blit(TextureLoader::GUI_button, 41, 34);
    texRBHRect.lock(3, 0, width/2-6, 16); texRBHRect.blit(TextureLoader::GUI_button, 45, 34, 32, 16);
    texRBHRect.lock(width/2-3, 0, 3, 16); texRBHRect.blit(TextureLoader::GUI_button, 78, 34);


    texRBCO.init(sdlHandler, 10, 10, 2);
    texRBCO.setColorMod(100, 50, 100);
    texRBCO.lock(); texRBCO.blit(TextureLoader::GUI_button, 55, 68);
}

void RadioButton::draw()
{
    if(hovering) {
        texRBHRect.draw();
    }

    if( selected ) {
        if(hovering) {
            texRBSH.draw();
        } else {
            texRBS.draw();
        }
    } else {
        texRB.draw();
        if(hovering) {
            texRBH.draw();
        }
    }

    if(clicked) {
        texRBCO.draw();
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
    texRB.setDrawPos(sX+4, sY+6);
    texRBH.setDrawPos(sX+4, sY+6);
    texRBHRect.setDrawPos(sX, sY);
    texRBS.setDrawPos(sX+4, sY+6);
    texRBSH.setDrawPos(sX+4, sY+6);
    texRBCO.setDrawPos(sX+4, sY+6);

    btnText.setPos(sX+36, sY+10);
}
