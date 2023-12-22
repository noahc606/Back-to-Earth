#include "Slider.h"
#include <cmath>
#include "TextBox.h"
#include "TextureBuilder.h"
#include "GUIHandler.h"

Slider::Slider(Window* parentWindow, int x, int y, double min, double max, std::string val, int id)
: Button::Button(parentWindow, x, y, 0, "", id)
{
	setSubType(BTEObject::Type::GUI_slider);
	
	selectorMin = min;
	selectorMax = max;
	
	setSelectorVal(val);
	
	width = 256;
	height = 16;
}

void Slider::init(SDLHandler* sh, Controls* ctrls)
{
	GUI::init(sh, ctrls);
	
	btnText.init(sdlHandler);
	btnText.setString("");

	//Build button textures
	TextureBuilder tb(sdlHandler);
	int texW = 128;
	int texH = 16;
	
	//Normal button
	tb.buildButton(texBtn, 0, 0, texW);
	texBtn.lock();
	texBtn.blit( TextureLoader::GUI_button, 35, 51 );
	
	//Hovering button
	tb.buildButton(texBtnHovering, 41, 0, texW);
	texBtnHovering.lock();
	texBtnHovering.blit( TextureLoader::GUI_button, 35, 51 );
	
	//Shine animation
	tb.buildButtonShine(btnShineTex0);
	tb.buildButtonShine(btnShineTex1);

	//Selection overlay
	tb.buildButton(texBtnSelected, 41, 17, texW);

	//Selector
	selectorTex.init(sdlHandler, 8, 16);
	selectorTex.setDrawScale(2);
	
	selectorTex.lock();
	selectorTex.setColorMod(255, 255, 0);
	selectorTex.blit( TextureLoader::GUI_button, 26, 51 );
}

void Slider::draw()
{
	Button::draw();
	selectorTex.draw();
}

void Slider::tick()
{
	Button::tick();
	
	bool holdingMouse = controls->isHeld("HARDCODE_LEFT_CLICK");
	
	//If the button is selected and user is clicking
	if( selected && holdingMouse ) {
		//Set selectorPtx based on mouse position 
		selectorPtx = (controls->getMouseX()-sX)/2-4;
		
		//Calculate important variables
		updateSelectorByPtx();
	}
	
	if( !holdingMouse ) {
		selected = false;
	}
	
	//Draw selector depending on selectorPtx
	selectorTex.setDrawPos(sX+selectorPtx*2, sY);
}


void Slider::syncWithTextboxes(GUIHandler* guih)
{
	validateSelectorVal();
	
	int id = getID();
	
	switch(id) {
		case GUIHandler::sdr_GRAPHICS_SETTINGS_maxFps:
		{
			TextBox* tbx = (TextBox*)(guih->getGUI(BTEObject::GUI_textbox, id+1));
			
			if( tbx->isSelected() ) {
				setSelectorVal( tbx->getString() );
			} else {
				tbx->setString(selectorVal);
			}
		}
	}
}

std::string Slider::getSelectorVal()
{
	return selectorVal;
}

void Slider::onWindowUpdate() { Button::onWindowUpdate(); }
void Slider::setNumSpaces(int spaces) { Slider::spaces = spaces; }
void Slider::setSelectorVal(std::string newSelectorVal)
{
	double selectorNumVal = -99999.0;
	
	try {
		selectorNumVal = std::stod(newSelectorVal);
	//If we could not get a number, default to 0
	} catch(...) {
		selectorVal = "0";
		selectorNumVal = 0.0;
	}
	
	//Update selector position
	selectorPtx = selectorNumVal/(selectorMax-selectorMin)*120.0;
	updateSelectorByPtx();
}

void Slider::validateSelectorVal()
{
	if( selectorVal.size()==0 ) {
		selectorVal = "0";
	}
	
	switch( getID() ) {
		case GUIHandler::sdr_GRAPHICS_SETTINGS_maxFps: {
			if( selectorVal=="0" ) {
				selectorVal = "Vsync";
			}
		} break;
	}
}

void Slider::updateSelectorByPtx()
{
	//Clamp selectorPtx
	if(selectorPtx<0) { selectorPtx = 0; }
	if(selectorPtx>120) { selectorPtx = 120; }
	
	//Round slider position to the nearest "space increment".
	//This is needed to make sure the slider selector matches up with the halfway/quarterway/eighthway markers.
	double inc = (120.0/(double)spaces);
	int numIncs = 0;
	double currentSpacePx = 0.0;
	while( currentSpacePx<=120.0 ) {
		if( std::abs(currentSpacePx-selectorPtx)<=inc/2.0 ) {
			selectorPtx = (int)currentSpacePx;
			
			//Set selectorVal
			std::stringstream ss;
			ss << (selectorMin+numIncs*((selectorMax-selectorMin)/(double)spaces) );
			selectorVal = ss.str();
			validateSelectorVal();
			break;
		}
		currentSpacePx += inc;
		numIncs++;
	}
}