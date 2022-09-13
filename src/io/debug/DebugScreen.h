#pragma once
#include "Controls.h"
#include "GUIHandler.h"
#include "SDLHandler.h"

class DebugScreen
{
public:
    /**/
    DebugScreen();
    void init(SDLHandler* sH, Controls* ctrls);
    virtual ~DebugScreen();
    /**/
    void draw();
    void tick(GUIHandler* guiHandler);
    /**/
    bool getVisible();
    /**/
    void setDebugString(std::string s);
    static void newGroup(std::stringstream& ss, int& indents, std::string s);
    static void endGroup(int& indents);
    static void indentLine(std::stringstream& ss, int& indents);
    static void newLine(std::stringstream& ss);
    /**/
    //Foreground and Background colors
    Color fg;
    Color bg;
protected:

private:
    /**/
    /**/
    void setHax0rMode(bool val);
    /**/
    //MainLoop objects
    SDLHandler* sdlHandler;
    Controls* controls;
    //Debug text properties
    bool visible = true;
    std::string debugString = "";

    //Debug text rgb
    bool hax0rMode = false;
    bool rgbCycleState[3] = {false, false, false};

};
