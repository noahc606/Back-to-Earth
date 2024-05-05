#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
#include "Items.h"
#include "Player.h"

class PlayerMenu {
public:
	/**/
	/**/
	void init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl);
	void tick();
	void draw();
	/**/
	int getState();
	uint8_t getSandboxTexX();
	uint8_t getSandboxTexY();
	uint8_t getSandboxTexRed();
	uint8_t getSandboxTexGreen();
	uint8_t getSandboxTexBlue();
	/**/
	void setState(int newState);
	/**/
private:
	/**/
	SDLHandler* sdlHandler = nullptr;
	GUIHandler* guiHandler = nullptr;
	Controls* controls = nullptr;
	int state = 0;
	int lastState = 0;
	int oscillation = 0;
	
	//Inventory slot and selected inventory slot
	const int invW = 8; const int invH = 8;
	int inventorySlots[8][8];
	int invSX = -1; int invSY = -1;
	bool newItemSelected = true;
	
	//Item move animation
	int itemMoveTimer = -1;
	Texture movingItemA; Texture movingItemB;
	int invLSX = -1; int invLSY = -1;
	
	int windowX = 0; int windowY = 0; int windowW = 0; int windowH = 0;
	int invScreenX = 0; int invScreenY = 0; int invScreenW = 0; int invScreenH = 0;
	
	uint8_t sandboxTexX = 1;
	uint8_t sandboxTexY = 3;

	
	Texture uiOverlay;
	Items items;

	/**/
	void drawInventory();
	/**/
	/**/
	void updateMenuCoordinates();
	void moveItemsBetween(int x1, int y1, int x2, int y2);
	void selectInventorySlot(int x, int y);
	/**/
};