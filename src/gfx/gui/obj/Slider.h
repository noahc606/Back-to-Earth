#pragma once
#include "Button.h"
#include "GUIHandler.h"
#include "Window.h"

class Slider : public Button
{
public:
	/**/
    Slider(Window* parentWindow, int x, int y, double min, double max, std::string val, int id);
	void init(SDLHandler* sh, Controls* ctrls);
	void draw();
	void tick();
	void syncWithRelatedUIs(GUIHandler* guih);
	/**/
	std::string getSelectorVal();
	/**/
	void onWindowUpdate();
	void setNumSpaces(int spaces);
	void setSelectorVal(std::string newSelectorVal);
	void validateSelectorVal();
	/**/
private:
	Texture selectorTex;
	int selectorPtx = 0;	//How many pixels (size 2) to the right should the selector be drawn
	int spaces = 32;		//Recommended to be 64, 32, 16, 8, etc. for the selector to match up with the markers
	
	double selectorMin = 0.0;
	std::string selectorVal = "0";
	double selectorMax = 100.0;
	
	void updateSelectorByPtx();
};