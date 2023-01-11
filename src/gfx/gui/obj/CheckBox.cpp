#include "CheckBox.h"
#include "TextureBuilder.h"

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_cbs, int p_id)
: Button::Button(p_parentWindow, p_x, p_y, 0, p_text, p_id)
{
    setSubType(BTEObject::Type::GUI_checkbox);
    checkBoxState = p_cbs;

    btnString = p_text;

    if(p_text=="") {
        width = 10;
        height = 10;
    } else {
        height = 16;
    }
}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, p_text, FALSE, p_id){};

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, int p_cbs, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, "", p_cbs, p_id){};

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, "", FALSE, p_id){};

CheckBox::~CheckBox(){}

void CheckBox::init(SDLHandler* sh, Controls* ctrls)
{
    std::cout << "initted" << "\n";
    GUI::init(sh, ctrls);

    btnText.init(sh);
    btnText.setString(btnString);
    width = btnText.getWidth()/2+2*24;

    //Build Tex
    texBtn.init(sdlHandler, 10, 10);
    texBtn.setDrawScale(2);
    texBtn.lock();
    texBtn.blit(TextureLoader::GUI_button, 0, 68);

    texBtnHovering.init(sdlHandler, width/2, 16, 2);
    texBtnHovering.setColorMod(100, 95, 51);
    texBtnHovering.lock(0, 0, 3, 16); texBtnHovering.blit(TextureLoader::GUI_button, 41, 34);
    texBtnHovering.lock(3, 0, width/2-6, 16); texBtnHovering.blit(TextureLoader::GUI_button, 45, 34, 32, 16);
    texBtnHovering.lock(width/2-3, 0, 3, 16); texBtnHovering.blit(TextureLoader::GUI_button, 78, 34);

    onWindowUpdate();
}

void CheckBox::draw()
{
    if(hovering) {
        texBtn.blit(TextureLoader::GUI_button, 11, 68);
        texBtnHovering.draw();
    } else {
        texBtn.blit(TextureLoader::GUI_button, 0, 68);
    }

    switch( checkBoxState ) {
        case 0: {

        } break;
        case 1: {
            texBtn.blit(TextureLoader::GUI_button, 0, 79);
        } break;
        case 2: {
            texBtn.blit(TextureLoader::GUI_button, 11, 79);
        } break;
        case 3: {
            texBtn.blit(TextureLoader::GUI_button, 0, 90);
        } break;
        default: {
            texBtn.blit(TextureLoader::GUI_button, 11, 90);
        } break;
    }

    if(clicked) {
        texBtn.blit(TextureLoader::GUI_button, 22, 68);
        clicked = false;
    }

    texBtn.draw();
    btnText.draw();
}

void CheckBox::onWindowUpdate()
{
    Button::onWindowUpdate();

    texBtn.setDrawPos(sX+4, sY+6);
    texBtnHovering.setDrawPos(sX, sY);
    btnText.setPos(sX+36, sY+10);
}

void CheckBox::tick()
{
    Button::tick();

    if(!hovering) {
        selected = false;
    }
}

void CheckBox::cycleState()
{
    checkBoxState++;
    if( checkBoxState>=UNKNOWN ) {
        checkBoxState = 0;
    }

    if( !canShuffle && checkBoxState==SHUFFLE ) {
        checkBoxState = BLANK;
    }

    if( !canLeaveBlank && checkBoxState==BLANK ) {
        checkBoxState = FALSE;
    }
}

void CheckBox::setCanLeaveBlank(bool clb) { canLeaveBlank = clb; }
void CheckBox::setCanShuffle(bool cs) { canShuffle = cs; }
