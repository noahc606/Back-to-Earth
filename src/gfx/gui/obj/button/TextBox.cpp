#include "TextBox.h"
#include <nch/cpp-utils/log.h>
#include <sstream>
#include "TextOld.h"
#include "TextureBuilder.h"

TextBox::TextBox(Window* p_parentWindow, int p_x, int p_y, int p_width, int p_inputType, std::string text, int p_id)
: Button::Button(p_parentWindow, p_x, p_y, p_width, "", p_id)
{
	inputType = p_inputType;
	setSubType(BTEObject::Type::GUI_textbox);

	btnInitString = text;
}

TextBox::TextBox(Window* p_parentWindow, int p_x, int p_y, int p_width, int p_inputType, int p_id)
: TextBox::TextBox(p_parentWindow, p_x, p_y, p_width, p_inputType, "", p_id){}

TextBox::TextBox(Window* p_parentWindow, int p_x, int p_y, int p_width, int p_id)
: TextBox::TextBox(p_parentWindow, p_x, p_y, p_width, FREE_TEXT, p_id){}

TextBox::TextBox(int p_x, int p_y, int p_width, int p_id)
: TextBox::TextBox(nullptr, p_x, p_y, p_width, p_id ){}

void TextBox::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
	//Set input type depending on the ID of the UI element
	switch( getID() ) {
		case GUIHandler::tbx_CONTROLS_set_keybind: {
			inputType = CONTROL_BINDINGS;
		} break;
		case GUIHandler::tbx_GRAPHICS_SETTINGS_maxFps: {
			inputType = FREE_NUMBERS_BASIC;
		} break;
	}

    Button::init(sh, ctrls);
    fileHandler = fh;
	
	TextureBuilder tb(sdlHandler);
	tb.buildButton(texInvalidInput, 82, 17, width/2);

    onWindowUpdate();
}

void TextBox::destroy()
{
	Button::destroy();
	texInvalidInput.destroy();
}

void TextBox::draw()
{
    Button::draw();

	if(colorInput && inputType==FREE_HEX_BASIC && btnText.getString().size()==7 ) {
		nch::Color c(255, 0, 0);
		c.setFromB16Str(btnText.getString()+"FF");
		btnText.draw(c);
	} else {
		btnText.draw();
	}
	
	if(invalidInput) {
		texInvalidInput.draw();
		btnText.draw();
	}
}

void TextBox::tick()
{
    Button::tick();

    if( entered && inputType!=CONTROL_BINDINGS ) {
        selected = false;
    }

    if( inputType==CONTROL_BINDINGS ) {
        btnText.selected = false;

        if( selected ) {
            btnText.setString("Enter input...");
            btnText.foreground = nch::Color(0, 255, 0);
        } else {
            btnText.setString(setCB.toCtrlString());
			
        }
    }

	if( !selected ) {
		invalidInput = false;
	} 

}

void TextBox::onWindowUpdate()
{
    Button::onWindowUpdate();

	texInvalidInput.setDrawPos(sX, sY);
}

void TextBox::passFreeTextInput(std::string s, int type)
{
    int ip = btnText.getInsertionPoint();
    std::string btn = btnText.getString();

    std::string s1 = btn.substr(0, ip); //Substring from beginning up to insertion point
    std::string s2 = s;                 //Substring to be inserted
    std::string s3 = btn.substr(ip);    //Substring from insertion point to the end
    try {
        if( type==ControlBinding::KEYBOARD_ACTION ) {
            int i = std::stoi(s2);
            s2="";

			switch( i ) {
			case SDLK_BACKSPACE: {
				s1 = s1.substr(0, s1.size()-1);
				ip--;
			} break;
			case SDLK_DELETE: {
				s3 = s3.substr(1);
			} break;
			
			case SDLK_RETURN: {
				if( getID()==GUIHandler::tbx_DEBUG ) {
					entered = true;
					return;
				}
			} break;
			
			case SDLK_LEFT: ip--; break;
			case SDLK_RIGHT: if(s3!="") ip++; break;
			case SDLK_HOME: ip = 0; break;
			case SDLK_END: ip = btn.size(); break;
			
			case SDLK_ESCAPE: actionID = SDLK_ESCAPE; break;
			case SDLK_UP: actionID = UP_ARROW; break;
			case SDLK_DOWN: actionID = DOWN_ARROW; break;
			}
		}
	} catch(...) {}
	
	//Get old string, resulting string, and reset 'invalidInput'
	std::string old = btnText.getString();
	std::string res = s1+s2+s3;
	invalidInput = false;
	
	//If resulting string after typing is longer than the textbox can hold, invalidate input and revert btnText.
	//If not, set new insertion point to be after whatever string was typed.
	btnText.setString(res);
	if( btnText.getWidth()>width*2-24 ) {
		invalidInput = true;
		btnText.setString(old);
	} else {
		btnText.setInsertionPoint( ip+s2.size() );
	}
	
	//Invalidate input when needed
	if(
		inputType==FREE_NUMBERS_BASIC ||
		inputType==FREE_NUMBERS_INTEGERS ||
		inputType==FREE_HEX_BASIC ||
		inputType==LEVELNAME_TEXT
	) {
		if(!validateString()) {
			invalidInput = true;
		}
	}
}

void TextBox::passSpecialInput(ControlBinding& cb)
{
    switch( inputType ) {
        case FREE_TEXT:
        case FREE_NUMBERS_BASIC:
		case FREE_NUMBERS_INTEGERS:
		case FREE_HEX_BASIC:
		case LEVELNAME_TEXT:
		{
            if(cb.getType()==cb.KEYBOARD_ACTION) {
                std::stringstream ss; ss << cb.keyboardAction;
                passFreeTextInput(ss.str(), cb.KEYBOARD_ACTION);
            } else {
                passFreeTextInput(cb.textInput, cb.TEXT_INPUT);
            }
        } break;
        case CONTROL_BINDINGS: {
            if( cb.keyboardAction!=-1 ) {
                //Set setCB
                setCB = ControlBinding(cb.KEYBOARD_ACTION, cb.keyboardAction);
                //Set setting to the last input
                Settings* stngs = fileHandler->getSettings();
                std::string key = stngs->getKey( stngs->getKvMap(Settings::controls), getExtraID());
                stngs->kv(Settings::controls, key, setCB.keyboardAction);
                fileHandler->saveSettings(Settings::controls);
                //Deselect
                deselect();
            }

        } break;
    }
}

int TextBox::getActionID() { return actionID; }
int TextBox::getInputType() { return inputType; }
bool TextBox::isEntered() { return entered; }
bool TextBox::hasInvalidInput() { return invalidInput; }

void TextBox::resetActionID(std::string methodName)
{
    if( actionID==-1 ) {
        std::stringstream ss;
        ss << "Tried to reset Action ID that already = -1.";
        nch::Log::warn(methodName, ss.str());
    }

    actionID = -1;
}

void TextBox::resetSettingData(int stngMapIndex)
{
	//Get helper objects
    Settings* stngs = fileHandler->getSettings();
    Settings::t_kvMap defaultMap = stngs->getDefaultSettings(stngMapIndex);
	if(defaultMap.size()==0) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Nonexistent setting map '%d' retrieved from resetting textbox ID '%d'.", stngMapIndex, getID());
		return;
	}

	//Get key and default value
    std::string key = stngs->getKey( stngs->getKvMap(stngMapIndex), getExtraID());
    std::string defval = stngs->get( defaultMap, key);

	//Set default value
    stngs->kv(stngMapIndex, key, defval);
    fileHandler->saveSettings(stngMapIndex);

	//Set control
	switch(stngMapIndex) {
		case Settings::controls: {
			setCB = ControlBinding(defval);
		} break;
		case Settings::character: {
			nch::Color defCol;
			defCol.setFromB10Str(defval);
			setString(defCol.toStringB16(false));
		} break;
	}
    
    deselect();
}

void TextBox::setEntered(bool p_entered) { entered = p_entered; }
void TextBox::setString(std::string s)
{
	switch(inputType) {
		case FREE_TEXT:
		case FREE_NUMBERS_BASIC:
		case FREE_NUMBERS_INTEGERS:
		case FREE_HEX_BASIC:
		case LEVELNAME_TEXT:
		break;
		default: return;
	}

	int i = btnText.getInsertionPoint();
	Button::setString(s);
	if( i>=(int)btnText.getString().size() ) {
		i = btnText.getString().size();
	}
	btnText.setInsertionPoint(i);
}
bool TextBox::validateString()
{
	bool res = true;
	std::string old = btnText.getString();

	//Find newString, which will be btnText.getString() but with all invalid characters removed
	if(inputType!=FREE_TEXT) {
		//Find allowedChars
		std::string allowedChars = "";
		switch(inputType) {
			case FREE_NUMBERS_BASIC:	allowedChars = "0123456789"; break;
			case FREE_NUMBERS_INTEGERS: allowedChars = "-0123456789"; break;
			case FREE_HEX_BASIC:		allowedChars = "#0123456789abcdefABCDEF"; break;
			case LEVELNAME_TEXT:		allowedChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 `~!@#$%^&*()-_=+[{]}|;:'\",<.>?"; break;
		}

		std::stringstream newStream;
		std::string newString = "";
		for( int i = 0; i<old.size(); i++ ) {
			if( allowedChars.find(old[i])!=std::string::npos ) {
				newStream << old[i];
			} else {
				res = false;
			}
		}
		newString = newStream.str();

		//Little more work for hex input validation
		if(inputType==FREE_HEX_BASIC) {
			//Remove all #'s and add a # at the beginning
			std::stringstream hexStream;
			hexStream << "#";
			for(int i = 0; i<newString.size(); i++) {
				if(newString[i]!='#') {
					hexStream << newString[i];
				}
			}
			newString = hexStream.str();
			
			//Capitalize all lowercase letters
			for(int i = 0; i<newString.size(); i++) {
				if(newString[i]>='a' && newString[i]<='f') {
					newString[i] = newString[i]-32;
				}
			}

			//Limit input to 7 chars (including the #)
			if(newString.size()>7) {
				newString = newString.substr(0, 7);
				res = false;
			}
		}
	
		//Set btnText's string and insertion point from newStirng
		btnText.setString(newString);
	}

	//Make sure insertion point does not exceed new string length
	if(res==false) {
		if(btnText.getInsertionPoint()>btnText.getString().size())
			btnText.setInsertionPoint(btnText.getString().size());
	}

	return res;
}
void TextBox::setControlBinding(ControlBinding& cb) { setCB = cb; }
void TextBox::setInsertionPoint(int ip) { btnText.setInsertionPoint(ip); }
void TextBox::deselect() {

    if( inputType==CONTROL_BINDINGS ) {
        if( setCB.getType()==setCB.NOTHING ) {
            btnText.setString("Unset Binding");
        }
    }

    btnText.selected = false;
    selected = false;
}

void TextBox::setColorInput(bool ci) { colorInput = true; }