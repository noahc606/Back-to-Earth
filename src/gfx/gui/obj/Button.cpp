#include "Button.h"
#include "CheckBox.h"
#include "GUIHandler.h"
#include "TextureBuilder.h"
#include "TextBox.h"

/**/

Button::Button(){}

Button::Button(Window* p_parentWindow, int p_x, int p_y, int p_width, std::string p_s, int p_id):
WindowComponent::WindowComponent(p_parentWindow, p_x, p_y, p_width)
{
    setType(BTEObject::Type::GUI_button);
    setID(p_id);

    //Take in p_s
    btnString = p_s;
}
Button::Button(int p_x, int p_y, int p_width, std::string p_s, int p_id):
Button(nullptr, p_x, p_y, p_width, p_s, p_id){}

void Button::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);

    int texW = width/2;
    int texH = height; //button height hardcoded

    //Build button textures
    TextureBuilder tb(sdlHandler);

	switch( getType() ) {
		case GUI_textbox: {
			tb.buildButton(texBtn, 82, 0, texW);
			tb.buildButton(texBtnHovering, 123, 0, texW);
			if( ((TextBox*)this)->getInputType()==TextBox::CONTROL_BINDINGS ) {
				tb.buildButton(texTbxSelected, 123, 17, texW);
			} else {
				tb.buildButton(texTbxSelected, 41, 17, texW);
			}
			
			tb.buildButton(texBtnSelected, 0, 17, texW);
		} break;
		default: {
			tb.buildButton(texBtn, 0, 0, texW);
			tb.buildButton(texBtnHovering, 41, 0, texW);
			tb.buildButton(texTbxSelected, 0, 17, texW);	//This component is unused in a non-textbox
			tb.buildButton(texBtnSelected, 0, 34, texW);
		} break;
	}
	
	tb.buildButtonShine(btnShineTex0);
	tb.buildButtonShine(btnShineTex1);

    //Build text
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
        if( getType()==GUI_button ||
			getType()==GUI_textbox ||
			getType()==GUI_slider
		) {
            texBtnHovering.draw();
        }
    } else {
    //If not hovering, draw texture 2
        if( getType()==GUI_button ||
			getType()==GUI_textbox ||
			getType()==GUI_slider
		) {
			texBtn.draw();
        }
    }

    //If button is selected, draw selection texture
    if(selected) {
        switch( getType() ) {
            case GUI_textbox: {
                texTbxSelected.draw();
            } break;
			case GUI_slider: break;
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
        mX>sX+1 && mX<=sX+1+width
     && mY>sY+1 && mY<=sY+1+32
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
        //If mouse is left clicked
        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
            //If this is a textbox
            if( getType()==GUI_textbox ) {
                btnText.setInsertionPointByPx( mX-sX );
            }
            //If this is a checkbox
            if( getType()==GUI_checkbox ) {
                CheckBox* cbObj = (CheckBox*)this;
                cbObj->cycleState();
            }

            //Stop pressing, set clicked+selected
            controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
            clicked = true;
            selected = true;
            btnText.setSelected(true);
        }
    //If mouse is NOT hovering...
    } else {
        //If mouse is left clicked
        if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
            //If this is not a radio button
            if(selected && getType()!=GUI_radiobutton) {
                //Stop pressing and end selection
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

    if(getID()==GUIHandler::ID::tbx_DEBUG) {
        sY = sdlHandler->getHeight()-height*2;
    }

    texBtn.setDrawPos(sX, sY);
    texBtnHovering.setDrawPos(sX, sY);
    texTbxSelected.setDrawPos(sX, sY);
    texBtnSelected.setDrawPos(sX, sY);
    btnShineTex0.setDrawPos(sX, sY);
    btnShineTex1.setDrawPos(sX, sY);


    int txtX = 0;
    if( getType()==GUI_textbox ) {
    //If textbox, text is going to be just to the right of the far left of button
        txtX = sX+4*2;
    } else {
    //If not textbox, center the text.
        int txtW = btnText.getWidth()/2;
        txtX = sX+(12+width-txtW)/2;
    }
    int txtY = sY+5*2;
    btnText.setPos( txtX, txtY+1 );
}

/**/

bool Button::isHovering() { return hovering; }
bool Button::isSelected() { return selected; }
std::string Button::getString() { return btnText.getString(); }

void Button::setString(std::string s) { btnText.setString(s); }