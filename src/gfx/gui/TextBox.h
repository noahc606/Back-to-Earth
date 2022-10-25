#pragma once
#include <string>
#include "GUIHandler.h"
#include "Texture.h"
#include "Button.h"

class TextBox : public Button
{
public:
    /**/
    TextBox(int x, int y, int width, int id);
    void init(SDLHandler* sh, Controls* ctrls);
    virtual ~TextBox();
    /**/
    void draw();
    void tick();
    void onWindowUpdate(bool preventInvalidTPos);
    void passKeyboardInput(std::string s, bool specialInput);
    /**/
    int getActionID();
    bool isEntered();
    /**/
    void resetActionID(std::string methodName);
    void setEntered(bool entered);
    void setString(std::string s);
    void setInsertionPoint(int ip);
    void deselect();
    /**/
    bool entered = false;

    enum Actions {
        NONE = -1,
        UP_ARROW = 1,
        DOWN_ARROW,
    };

protected:

private:

    int actionID = 0;

};
