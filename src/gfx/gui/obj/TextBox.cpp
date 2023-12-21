#include "TextBox.h"
#include <sstream>
#include "Text.h"
#include "TextureBuilder.h"
#include "Log.h"

TextBox::TextBox(Window* p_parentWindow, int p_x, int p_y, int p_width, int p_id)
: Button::Button(p_parentWindow, p_x, p_y, p_width, "", p_id)
{
    setSubType(BTEObject::Type::GUI_textbox);
}

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
		default: {
			inputType = FREE_TEXT;
		} break;
	}
	
    Button::init(sh, ctrls);
    fileHandler = fh;
	
	TextureBuilder tb(sdlHandler);
	tb.buildButton(texInvalidInput, 82, 17, width/2);

    onWindowUpdate();
}
TextBox::~TextBox(){}


void TextBox::draw()
{
    Button::draw();

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
            btnText.foreground = Color(0, 255, 0);
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
				if( getInputType()==GUIHandler::tbx_DEBUG ) {
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
	
	//Invalidate input where non-numbers (not in 0-9) are entered in a numbers-only textbox
	if( inputType==FREE_NUMBERS_BASIC ) {
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
        Log::warn(methodName, ss.str());
    }

    actionID = -1;
}

void TextBox::resetEnteredData()
{
    Settings* stngs = fileHandler->getSettings();
    Settings::t_kvMap defaultCtrls = stngs->getDefaultSettings(Settings::controls);
    std::string key = stngs->getKey( defaultCtrls, getExtraID());
    std::string val = stngs->get( defaultCtrls, key);

    stngs->kv(Settings::TextFiles::controls, key, val);
    fileHandler->saveSettings(Settings::controls);

    setCB = ControlBinding(stngs->get(defaultCtrls, key));

    deselect();
}

void TextBox::setEntered(bool p_entered) { entered = p_entered; }
void TextBox::setString(std::string s)
{
    if( inputType==FREE_TEXT || inputType==FREE_NUMBERS_BASIC ) {
        int i = btnText.getInsertionPoint();
        Button::setString(s);
        if( i>=(int)btnText.getString().size() ) {
            i = btnText.getString().size();
        }
        btnText.setInsertionPoint(i);
    }
}
bool TextBox::validateString()
{
	bool res = true;
	//Remove all numbers from btnText if this textbox should only be for non-negative integers
	if( inputType==FREE_NUMBERS_BASIC ) {
		std::string old = btnText.getString();
		std::stringstream noNumsStream;
		
		for( int i = 0; i<old.size(); i++ ) {
			if( old[i]>='0' && old[i]<='9' ) {
				noNumsStream << old[i];
			} else {
				res = false;
			}
		}
		
		std::string noNums = noNumsStream.str();
		btnText.setString(noNums);
		btnText.setInsertionPoint(noNums.size());
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
