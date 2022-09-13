#pragma once
#include "GUI.h"

class GUIAlignable : public GUI
{
public:
    /**/
    GUIAlignable();
    virtual ~GUIAlignable();
    /**/
    virtual void updateScreenPos();
    virtual void onWindowUpdate(bool preventInvalidTPos);
    /**/
    int getHorAlignment();
    int getVerAlignment();
    int getAlignment(bool dim);

    int getSX(); int getSY();
    int getSPos(bool dim);              //Get sX or sY

    int getWidth(); int getHeight();
    int getDim(bool dim);               //Get width or height

    enum HorizontalAlignments {
        NO_H = 10,
        L = -110000,
        CENTER_H = -120000,
        R = -130000,
    };
    enum VerticalAlignments {
        NO_V = 20,
        U = -210000,
        CENTER_V = -220000,
        D = -230000,
    };

protected:
    int horAlignment = NO_H;
    int verAlignment = NO_V;

    int width = 0; int height = 0;      //Dimensions where object is clickable, hoverable, should be aligned based upon, etc.'
    int sX = 0; int sY = 0;             //Position relative to the computer's window. May be negative or positive.
private:

};
