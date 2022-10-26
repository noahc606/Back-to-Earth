#include "Button.h"
#include "GUIHandler.h"
#include "TextureBuilder.h"

/**/

Button::Button(){}

Button::Button(Window* p_parentWindow, int p_x, int p_y, int p_width, std::string p_s, int p_id)
: WindowComponent::WindowComponent(p_parentWindow, p_x, p_y, p_width)
{
    setType(BTEObject::Type::GUI_button);
    setID(p_id);

    buttonType = BUTTON;

    //Take in p_s
    btnString = p_s;
}
Button::Button(int p_x, int p_y, int p_width, std::string p_s, int p_id):
Button(nullptr, p_x, p_y, p_width, p_s, p_id){}

void Button::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    int texW = width/2+6;
    int texH = height; //button height hardcoded

    //Build button textures
    TextureBuilder tb(sdlHandler);

    if( buttonType==TEXTBOX ) {
        tb.init(TextureBuilder::BTN_Tex, texBtn, texW, texH,  0,  0);
        tb.init(TextureBuilder::BTN_Tex, texBtnHovering, texW, texH, 41,  0);
        tb.init(TextureBuilder::BTN_Tex, texTbxSelected, texW, texH,  0, 17);
        tb.init(TextureBuilder::BTN_Tex, texBtnSelected, texW, texH,  0, 34);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex0);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex1);
    } else {
        tb.init(TextureBuilder::BTN_Tex, texBtn, texW, texH,  0,  0);
        tb.init(TextureBuilder::BTN_Tex, texBtnHovering, texW, texH, 41,  0);
        tb.init(TextureBuilder::BTN_Tex, texTbxSelected, texW, texH,  0, 17);
        tb.init(TextureBuilder::BTN_Tex, texBtnSelected, texW, texH,  0, 34);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex0);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex1);
    }


    //Build Tex
    btnText.init(sdlHandler);
    btnText.setString(btnString);

    onWindowUpdate();
}

void Button::destroy()
{
    BTEObject::destroy();

    texBtn.destroy();
    texBtnHovering.destroy();
    texTbxSelected.destroy();
    texBtnSelected.destroy();
    btnShineTex0.destroy();
    btnShineTex1.destroy();
}

/**/

void Button::draw()
{
    //If hovering, draw texture 1
    if(hovering) {
        if( buttonType==BUTTON || buttonType==TEXTBOX ) {
            texBtnHovering.draw();
        }
    } else {
    //If not hovering, draw texture 2
        if( buttonType==BUTTON || buttonType==TEXTBOX ) {
            texBtn.draw();
        }
    }

    //If button is selected, draw selection texture
    if(selected) {
        switch( buttonType ) {
            case TEXTBOX: {
                texTbxSelected.draw();
            } break;
            default: {
                texBtnSelected.draw();
            } break;
        }
    }

    //Draw button shine animation
    if( hovering && shineAnimation>=0 ) {
        btnShineTex0.draw();
        btnShineTex1.draw();
    }

    //Draw button's text
    btnText.draw();
}

void Button::tick()
{
    //Test if mouse is hovering over or not
    int mX = controls->getMouseX();
    int mY = controls->getMouseY();

    //If mouse hovering
    if(
        mX>=sX && mX<sX+width+12
     && mY>=sY && mY<sY+32
     ) {
        if(!hovering) shineAnimation = 1;
        hovering = true;
    //If mouse not hovering
    } else {
        hovering = false;
    }

    //Mouse hover animation
    if( shineAnimation>=0 ) {
        shineAnimation += width/16;
        btnShineTex0.setDrawPos( sX+shineAnimation, sY );
        btnShineTex1.setDrawPos( sX+(width+12)-36-shineAnimation, sY );
        if(shineAnimation+18 >= width)
            shineAnimation = -1;
    }

    //If mouse is hovering...
    if( hovering ) {
        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {

            if( buttonType==TEXTBOX ) {
                btnText.setInsertionPointByPx( mX-sX );
            }

            controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
            clicked = true;
            selected = true;
            btnText.setSelected(true);
        }
    } else {
        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
            if(selected && buttonType!=RADIO ) {
            controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
                selected = false;
                btnText.setSelected(false);
            }
        }
    }
}

void Button::onWindowUpdate()
{
    translateSPos();

    if(getID()==GUIHandler::ID::tb_DEBUG) {
        sY = sdlHandler->getHeight()-height*2;
    }

    texBtn.setDrawPos(sX, sY);
    texBtnHovering.setDrawPos(sX, sY);
    texTbxSelected.setDrawPos(sX, sY);
    texBtnSelected.setDrawPos(sX, sY);
    btnShineTex0.setDrawPos(sX, sY);
    btnShineTex1.setDrawPos(sX, sY);

    //If not textbox, center the text.
    int txtX = 0;
    if( buttonType==TEXTBOX ) {
        txtX = sX+4*2;
    } else {
        int txtW = btnText.getWidth()/2;
        txtX = sX+(12+width-txtW)/2;
    }
    int txtY = sY+5*2;
    btnText.setPos( txtX, txtY );
}

/**/

bool Button::isSelected() { return selected; }
std::string Button::getString() { return btnText.getString(); }

void Button::setString(std::string s) { btnText.setString(s); }
