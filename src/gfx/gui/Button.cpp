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

    //Take in p_s
    btnString = p_s;
}
Button::Button(int p_x, int p_y, int p_width, std::string p_s, int p_id):
Button(nullptr, p_x, p_y, p_width, p_s, p_id)
{

}

void Button::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    int texW = width/2+6;
    int texH = height; //button height hardcoded

    //Build button textures
    TextureBuilder tb(sdlHandler);

    if(textbox) {
        tb.init(TextureBuilder::BTN_Tex, btnTex0, texW, texH,  0,  0);
        tb.init(TextureBuilder::BTN_Tex, btnTex1, texW, texH, 41,  0);
        tb.init(TextureBuilder::BTN_Tex, btnTex2, texW, texH,  0, 17);
        tb.init(TextureBuilder::BTN_Tex, btnTex3, texW, texH,  0, 34);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex0);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex1);
    } else {
        tb.init(TextureBuilder::BTN_Tex, btnTex0, texW, texH,  0,  0);
        tb.init(TextureBuilder::BTN_Tex, btnTex1, texW, texH, 41,  0);
        tb.init(TextureBuilder::BTN_Tex, btnTex2, texW, texH,  0, 17);
        tb.init(TextureBuilder::BTN_Tex, btnTex3, texW, texH,  0, 34);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex0);
        tb.init(TextureBuilder::BTN_ShineTex, btnShineTex1);
    }


    //Build Tex
    btnText.init(sdlHandler);
    btnText.setString(btnString);

    onWindowUpdate(false);
}

void Button::destroy()
{
    BTEObject::destroy();

    btnTex0.destroy();
    btnTex1.destroy();
    btnTex2.destroy();
    btnTex3.destroy();
    btnShineTex0.destroy();
    btnShineTex1.destroy();
}

/**/

void Button::draw()
{
    //If hovering, draw texture 1
    if(hovering) {
        btnTex1.draw();
    } else {
    //If not hovering, draw texture 2
        btnTex0.draw();
    }

    //If button is selected, draw selection texture
    if(selected) {
        if(textbox) {
            btnTex2.draw();
        } else {
            btnTex3.draw();
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

            if(textbox) {
                btnText.setInsertionPointByPx( mX-sX );
            }

            controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
            clicked = true;
            selected = true;
            btnText.setSelected(true);
        }
    } else {
        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
            if(selected) {
            controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
                selected = false;
                btnText.setSelected(false);
            }
        }
    }
}

void Button::updateScreenPos()
{
    if( parentWindow!=nullptr ) {
        if( parentWindow->getType()==BTEObject::Type::GUI_window ) {
            sX = tX+parentWindow->getSX();
            sY = tY+parentWindow->getSY();
        }
    } else {
        sX = tX;
        sY = tY;
    }
}

void Button::onWindowUpdate(bool preventInvalidTPos)
{
    updateScreenPos();

    if(getID()==GUIHandler::ID::tb_DEBUG) {
        sY = sdlHandler->getHeight()-height*2;
    }

    btnTex0.setDrawPos(sX, sY);
    btnTex1.setDrawPos(sX, sY);
    btnTex2.setDrawPos(sX, sY);
    btnTex3.setDrawPos(sX, sY);
    btnShineTex0.setDrawPos(sX, sY);
    btnShineTex1.setDrawPos(sX, sY);

    //If not textbox, center the text.
    int txtX = 0;
    if( textbox ) {
        txtX = sX+4*2;
    } else {
        int txtW = btnText.getWidth()/2;
        txtX = sX+(12+width-txtW)/2;
    }
    int txtY = sY+5*2;
    btnText.setPos( txtX, txtY );

    if(preventInvalidTPos) {
        //preventInvalidTPos();
    }
}

/**/

bool Button::isSelected() { return selected; }
std::string Button::getString() { return btnText.getString(); }

void Button::setString(std::string s) { btnText.setString(s); }
