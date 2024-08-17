#include "Button.h"
#include "CheckBox.h"
#include "GUIHandler.h"
#include "Log.h"
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
    btnInitString = p_s;
}
Button::Button(int p_x, int p_y, int p_width, std::string p_s, int p_id):
Button(nullptr, p_x, p_y, p_width, p_s, p_id){}

void Button::init(SDLHandler* sh, Controls* ctrls)
{
    GUI::init(sh, ctrls);
    
    //Reset draw color
    sdlHandler->setRenderDrawColor(NCH_Color(0, 0, 0, 0));

    int texW = width/2;
    int texH = height/2;

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
            texBtnSelected.setTexWidth( texBtnSelected.getTexWidth()-2 );
		} break;
	}
	
	tb.buildButtonShine(texBtnShine0);
	tb.buildButtonShine(texBtnShine1);
    tb.buildButton(texBtnErrFlash, 0, 17, texW);

    //Build text
    btnText.init(sdlHandler);
    btnText.setString(btnInitString);

    onWindowUpdate();
}

void Button::destroy()
{
    BTEObject::destroy();

    texBtn.destroy();
    texBtnHovering.destroy();
    texTbxSelected.destroy();
    texBtnSelected.destroy();
    texBtnShine0.destroy();
    texBtnShine1.destroy();
    texBtnErrFlash.destroy();
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
        texBtnShine0.draw();
        texBtnShine1.draw();
    }

    //During error flash animation
    if( errFlashAnimation>0 ) {
        SDL_SetTextureAlphaMod(texBtnErrFlash.getSDLTexture(), errFlashAnimation);
        SDL_SetTextureColorMod(texBtnErrFlash.getSDLTexture(), 255, 255, 255);
        texBtnErrFlash.draw();
    } else {
        SDL_SetTextureAlphaMod(texBtnErrFlash.getSDLTexture(), 255);
        SDL_SetTextureColorMod(texBtnErrFlash.getSDLTexture(), 255, 255, 255);
    }

    //Draw button's text (for textboxes there is a custom implementation)
    if( getType()!=GUI_textbox ) {
        btnText.draw();
    }
}

void Button::tick()
{
    //Error flash
    if(errFlashAnimation>0) {
        errFlashAnimation -= 10;
    }

    //Test if mouse is hovering over or not
    int mX = controls->getMouseX();
    int mY = controls->getMouseY();

    //If mouse hovering
    if(parentWindow==nullptr || parentWindow->isActive()) {
        if( mX>sX+1 && mX<=sX+1+width && mY>sY+1 && mY<=sY+1+height ) {
            if(!hovering) shineAnimation = 1;
            hovering = true;
        //If mouse not hovering
        } else {
            hovering = false;
        }
    }

    //Mouse hover animation
    if( shineAnimation>=0 ) {
        shineAnimation += width/10;
        texBtnShine0.setDrawPos( sX+shineAnimation, sY );
        texBtnShine1.setDrawPos( sX+(width+12)-36-shineAnimation, sY );
        if(shineAnimation+18 >= width)
            shineAnimation = -1;
    }

    //Test whether this button is interactable
    bool btnInteractable = false;
    if(parentWindow==nullptr || parentWindow->isActive()) btnInteractable = true;

    //If button is interactable...
    if(btnInteractable) {
        //If the mouse is hovering on the button...
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
        }

        //If    q1the mouse is NOT hovering on the button...
        if( !hovering) {
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
}

void Button::onWindowUpdate()
{
    translateSPos();

    if(getID()==GUIHandler::ID::tbx_DEBUG) {
        sY = sdlHandler->getHeight()-height;
    }

    texBtn.setDrawPos(sX, sY);
    texBtnHovering.setDrawPos(sX, sY);
    texTbxSelected.setDrawPos(sX, sY);
    texBtnSelected.setDrawPos(sX, sY);
    texBtnShine0.setDrawPos(sX, sY);
    texBtnShine1.setDrawPos(sX, sY);
    texBtnErrFlash.setDrawPos(sX, sY);


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
void Button::unclick() { clicked = false; }
void Button::deselect() { selected = false; }
void Button::errorFlash() { errFlashAnimation = 255; }