#include "CheckBox.h"
#include "TextureBuilder.h"

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, int p_cbs, int p_id)
: Button::Button(p_parentWindow, p_x, p_y, 0, "", p_id )
{
    setSubType(BTEObject::Type::GUI_checkbox);

    width = 10;
    height = 10;

    checkBoxState = p_cbs;
}
CheckBox::~CheckBox(){}

void CheckBox::init(SDLHandler* sh, Controls* ctrls)
{
    std::cout << "initted" << "\n";
    GUI::init(sh, ctrls);

    //Build Tex
    texBtn.init(sdlHandler, 10, 10);
    texBtn.setDrawScale(2);
    texBtn.lock();
    texBtn.blit(TextureLoader::GUI_button, 0, 68);

    onWindowUpdate();
}

void CheckBox::draw()
{
    if(hovering) {
        if( blitBG!=0 ) {
            blitBG = 1;
            texNeedsUpdate = true;
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
        }
    } else {
        if( blitBG!=2 ) {
            blitBG = 2;
            texNeedsUpdate = true;
            texBtn.blit(TextureLoader::GUI_button, 0, 68);
        }
    }

    if(clicked) {
        if( blitCO!=1 ) {
            blitCO = 1;
            texNeedsUpdate = true;
            texBtn.blit(TextureLoader::GUI_button, 22, 68);
        }
    }

    if(texNeedsUpdate) {
        blitIcon();
        texNeedsUpdate = false;
    }

    texBtn.draw();
}

void CheckBox::blitIcon()
{
    switch( checkBoxState ) {
        case 0: {
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
        } break;
        case 1: {
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
            texBtn.blit(TextureLoader::GUI_button, 0, 79);
        } break;
        case 2: {
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
            texBtn.blit(TextureLoader::GUI_button, 11, 79);
        } break;
        case 3: {
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
            texBtn.blit(TextureLoader::GUI_button, 0, 90);
        } break;
        default: {
            texBtn.blit(TextureLoader::GUI_button, 11, 68);
            texBtn.blit(TextureLoader::GUI_button, 11, 90);
        } break;
    }
}

void CheckBox::tick()
{
    Button::tick();
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
