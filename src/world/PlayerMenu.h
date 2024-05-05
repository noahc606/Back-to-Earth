#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
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
	Player* player = nullptr;
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
	
	int windowW = 0;
	int windowH = 0;
	int windowX = 0;
	int windowY = 0;
	int invScreenW = 0;
	int invScreenH = 0;
	int invScreenX = 0;
	int invScreenY = 0;
	
	uint8_t sandboxTexX = 1;
	uint8_t sandboxTexY = 3;
	uint8_t sandboxRed = 0;
	uint8_t sandboxGreen = 255;
	uint8_t sandboxBlue = 255;
	
	
	
	enum IDs {
		SANDBOX = -100,
		UNDEFINED = -1,
		ION_BLASTER = 0,
		GEOPORTER,
		SOLID_PLASMA_ANNIHILATOR,
		ATOM_PRINTER,
		NYLON_EXOSUIT_HELMET,
		NYLON_EXOSUIT_BODY,
		NYLON_EXOSUIT_LEGGINGS,
		FOOD_RATION_A,
		FOOD_RATION_B,
		FOOD_RATION_C,
		FOOD_RATION_D,
		FOOD_RATION_E,
		FOOD_RATION_F,
		INFINITE_OXYGEN_TANK,

	};
	
	Texture uiOverlay;

	/**/
	void drawInventory();
	/**/
	std::string getItemName(int itemID);
	/**/
	void updateMenuCoordinates();
	void moveItemsBetween(int x1, int y1, int x2, int y2);
	void selectInventorySlot(int x, int y);

	void putItemInterfaceDesc(Window* win, int dX, int ttpDY, std::string itemDesc, int rowNum);
	void putItemInterfaceTitle(Window* win, int dX, int ttpDY, int itemID);
	void putItemInterface(int itemID);
	/**/
};