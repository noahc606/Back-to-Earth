#pragma once
#include <string>
#include "Button.h"
#include "ControlBinding.h"
#include "GUIHandler.h"
#include "Texture.h"

class TextBox : public Button
{
public:
    /**/
    TextBox(Window* parentWindow, int x, int y, int width, int inputType, std::string text, int id);
    TextBox(Window* parentWindow, int x, int y, int width, int inputType, int id);
    TextBox(Window* parentWindow, int x, int y, int width, int id);
    TextBox(int x, int y, int width, int id);
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    void destroy();
    /**/
    void draw();
    void tick();
    void onWindowUpdate();
    void passFreeTextInput(std::string s, int specialAction);
    void passSpecialInput(ControlBinding& cb);
    /**/
    int getActionID();
    int getInputType();
    bool isEntered();
	bool hasInvalidInput();
    /**/
    void resetActionID(std::string methodName);
    void resetEnteredData();
    void setEntered(bool entered);
    void setString(std::string s);
	bool validateString();
    void setControlBinding(ControlBinding& cb);
    void setInsertionPoint(int ip);
    void deselect();
    void setColorInput(bool ci);
    /**/

    enum Actions {
        NONE = -1,
        UP_ARROW = 1,
        DOWN_ARROW,
    };

    enum InputType {
        LOCKED = -1,            //Cannot enter text
        FREE_TEXT = 0,          //Any character
		FREE_NUMBERS_BASIC,     //Positive integers
		FREE_NUMBERS_INTEGERS,  //Integers
        FREE_HEX_BASIC,         //Positive hex integers
        CONTROL_BINDINGS,       //Control inputs
        LEVELNAME_TEXT,         //[ ], [aA-zZ], [0-9], [`~!@#$%^&*-_+=|:;"'<>,.?]
    };

protected:

private:
    int inputType = LOCKED;
    bool colorInput = false;
    int actionID = 0;
    bool entered = false;
	bool invalidInput = false;
	
	Texture texInvalidInput;
	
    ControlBinding setCB;

};
