#include "CheckBox.h"
#include "TextureBuilder.h"

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_checkBoxState, bool p_largerButton, int p_id)
: Button::Button(p_parentWindow, p_x, p_y, 0, p_text, p_id)
{
	setSubType(BTEObject::Type::GUI_checkbox);
	largerButton = p_largerButton;
	state = p_checkBoxState;

	btnString = p_text;

	if(p_text=="") {
		width = 10;
		height = 10;
	} else {
		height = 32;
	}
}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_checkBoxState, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, p_text, p_checkBoxState, false, p_id) {}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, std::string p_checkBoxState, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, p_text, getStateFromString(p_checkBoxState), p_id) {}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, std::string p_text, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, p_text, CBX_FALSE, p_id) {}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, int p_cbs, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, "", p_cbs, p_id) {}

CheckBox::CheckBox(Window* p_parentWindow, int p_x, int p_y, int p_id)
: CheckBox::CheckBox(p_parentWindow, p_x, p_y, "", CBX_FALSE, p_id) {}

CheckBox::~CheckBox(){}

void CheckBox::init(SDLHandler* sh, Controls* ctrls)
{
	GUI::init(sh, ctrls);

	btnText.init(sh);
	btnText.setString(btnString);

	if(btnString!="") {
		width = btnText.getWidth()/2+49;
	} else {
		if(!largerButton) {
			width = 20;
			height = 20;
		} else {
			width = 32;
			height = 32;
		}
	}

	texBtn.init(sdlHandler);
	texBtnHovering.init(sdlHandler);
	buildTexes();

	onWindowUpdate();
}

void CheckBox::buildTexes()
{
	//Build 'texBtn' (primary button which includes the button icon)
	texBtn.clear();
	if(!largerButton) {
		texBtn.setTexDimensions(10, 10);
		texBtn.setDrawScale(2);
		texBtn.lock();
		texBtn.blit(TextureLoader::GUI_button, 0, 68);
	} else {
		texBtn.setTexDimensions(16, 16);
		texBtn.setDrawScale(2);
		texBtn.lock();
		texBtn.blit(TextureLoader::GUI_button, 0, 112);
	}

	//Build 'texBtnHovering' (Hover outline overlay)
	texBtnHovering.clear();
	if(!largerButton) {
		texBtnHovering.setTexDimensions(width/2, 16);
		texBtnHovering.setDrawScale(2);
		texBtnHovering.setColorMod(53, 221, 213);
		//Button selection overlay left part
		texBtnHovering.lock(0, 0, 3, 16); texBtnHovering.blit(TextureLoader::GUI_button, 41, 34);
		//Button selection overlay center part
		texBtnHovering.lock(3, 0, (width)/2-6, 16); texBtnHovering.blit(TextureLoader::GUI_button, 45, 34, 32, 16);
		//Button selection overlay right part
		texBtnHovering.lock((width)/2-3, 0, 3, 16); texBtnHovering.blit(TextureLoader::GUI_button, 78, 34);
	}
}

void CheckBox::draw()
{
	texBtn.lock();
	if(hovering) {
		if(largerButton) {
			texBtn.blit(TextureLoader::GUI_button, 17, 112);
		} else {
			texBtn.blit(TextureLoader::GUI_button, 11, 68);
		}
		texBtnHovering.draw();
	} else {
		if(largerButton) {
			texBtn.blit(TextureLoader::GUI_button, 0, 112);
		} else {
			texBtn.blit(TextureLoader::GUI_button, 0, 68);
		}
	}

	if(largerButton) {
		texBtn.lock(3, 3, 10, 10);
	}

	switch( state ) {
		case CBX_BLANK: {

		} break;
		case CBX_FALSE: {
			texBtn.blit(TextureLoader::GUI_button, 0, 79);
		} break;
		case CBX_TRUE: {
			texBtn.blit(TextureLoader::GUI_button, 11, 79);
		} break;
		case CBX_SHUFFLE: {
			texBtn.blit(TextureLoader::GUI_button, 0, 90);
		} break;
		case CBX_RESET: {
			texBtn.blit(TextureLoader::GUI_button, 22, 79);
		} break;
		default: {
			texBtn.blit(TextureLoader::GUI_button, 11, 90);
		} break;
	}

	texBtn.lock();
	if(clicked) {
		if(largerButton) {
			texBtn.blit(TextureLoader::GUI_button, 34, 112);
		} else {
			texBtn.blit(TextureLoader::GUI_button, 22, 68);
		}
	}

	texBtn.draw();
	btnText.draw();
}

void CheckBox::onWindowUpdate()
{
	Button::onWindowUpdate();


	if(!largerButton) {
		texBtn.setDrawPos(sX+4, sY+6);
		texBtnHovering.setDrawPos(sX, sY);
		btnText.setPos(sX+36, sY+10);
	} else {
		texBtn.setDrawPos(sX, sY);
		texBtnHovering.setDrawPos(sX, sY);
		btnText.setPos(sX+36, sY);
	}

}

void CheckBox::tick()
{
	Button::tick();

	if(!hovering) {
		selected = false;
	}
}

int CheckBox::getStateFromString(std::string s)
{
	if( s=="true" ) {
		return CBX_TRUE;
	} else if( s=="false" ) {
		return CBX_FALSE;
	}
	return CBX_UNKNOWN;
}
int CheckBox::getState() { return state; }
bool CheckBox::isLargerButton() { return largerButton; }
bool CheckBox::justClicked() { return clicked; }

void CheckBox::cycleState()
{
	if( state==CBX_RESET ) {

	} else {
		state++;
		if( state>=CBX_UNKNOWN ) {
			state = 0;
		}

		if( !canShuffle && state==CBX_SHUFFLE ) {
			state = CBX_BLANK;
		}

		if( !canLeaveBlank && state==CBX_BLANK ) {
			state = CBX_FALSE;
		}
	}
}
void CheckBox::unclick() { clicked = false; }

void CheckBox::setLargerButton(bool ilb)
{
	largerButton = ilb;
}

void CheckBox::setCanLeaveBlank(bool clb) { canLeaveBlank = clb; }
void CheckBox::setCanShuffle(bool cs) { canShuffle = cs; }
