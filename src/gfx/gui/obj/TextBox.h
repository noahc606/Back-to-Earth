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
    TextBox(Window* parentWindow, int x, int y, int width, int id);
    TextBox(int x, int y, int width, int id);
    void init(SDLHandler* sh, FileHandler* fh, Controls* ctrls);
    virtual ~TextBox();
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
    /**/

    enum Actions {
        NONE = -1,
        UP_ARROW = 1,
        DOWN_ARROW,
    };

    enum InputType {
        LOCKED = -1,
        FREE_TEXT = 0,
		FREE_NUMBERS_BASIC,
        CONTROL_BINDINGS,
    };

protected:

private:
    int inputType = NONE;
    int actionID = 0;
    bool entered = false;
	bool invalidInput = false;
	
	Texture texInvalidInput;
	
    ControlBinding setCB;

};