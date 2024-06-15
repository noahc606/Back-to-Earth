#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
#include "Items.h"
#include "Player.h"
#include <map>

class PlayerMenu {
public:
	/**/
	/**/
	void init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl);
	void tick();
	void draw();
	/**/
	int getSlotHoveringX();
	int getSlotHoveringY();
	int getItemTexSrcX(int itemID);
	int getItemTexSrcY(int itemID);
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
	
	std::map<std::pair<int, int>, int> inventorySlots;
	//int inventorySlots[8][8];
	//int clothingSlots[0][3];

	int invSX = -1; int invSY = -1;		//X, Y of the currently selected inventory slot
	int invHeldID = -1;					//ID of the currently held item
	
	bool itemUIShouldUpdate = true;
	bool itemHeldShouldUpdate = true;
	
	int windowX = 0; int windowY = 0; int windowW = 0; int windowH = 0;
	int invScreenX = 0; int invScreenY = 0; int invScreenW = 0; int invScreenH = 0;
	
	uint8_t sandboxTexX = 1;
	uint8_t sandboxTexY = 3;

	
	Texture uiOverlay;
	Texture heldItem;
	Items items;

	/**/
	void drawInventory();
	/**/
	bool inventorySlotExists(int x, int y);
	int getInventorySlotItem(int x, int y);
	/**/
	void updateMenuCoordinates();
	void updateSandboxRGB();
	void selectInventorySlot(int x, int y);
	void setInventorySlotItem(int x, int y, int i);
	/**/
};