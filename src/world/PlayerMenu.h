#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"

class PlayerMenu {
public:
	/**/
	/**/
	void init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls);
	void tick();
	void draw();
	/**/
	int getState();
	/**/
	void setState(int newState);
	/**/
private:
	/**/
	SDLHandler* sdlHandler;
	GUIHandler* guiHandler;
	Controls* controls;
	int state = 0;
	int oscillation = 0;
	
	const int invW = 8;
	const int invH = 8;
	int inventorySlots[8][8];
	
	int windowW = 0;
	int windowH = 0;
	int windowX = 0;
	int windowY = 0;
	int invScreenW = 0;
	int invScreenH = 0;
	int invScreenX = 0;
	int invScreenY = 0;
	
	Texture uiOverlay;
	/**/
	void updateMenuCoordinates();
	/**/
	/**/
	/**/
};