#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
#include "Items.h"
#include "InvItemStack.h"
#include "Player.h"
#include "PlayerMenuModules.h"
#include <map>

class PlayerMenu {
public:
	/**/
	/**/
	void init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl);
	void tick();
	void draw();
	/**/
	bool inventorySlotExists(int x, int y);
	int getInventorySlotItemType(int x, int y);
	InvItemStack getInventorySlotItemStack(int x, int y);
	static int getItemTexSrcX(int itemID);
	static int getItemTexSrcY(int itemID);
	int getModule();
	uint8_t getSelectedSlotX();
	uint8_t getSelectedSlotY(); 
	uint8_t getSandboxTexRed();
	uint8_t getSandboxTexGreen();
	uint8_t getSandboxTexBlue();
	InvItemStack getSelectedItemStack();
	/**/
	void giveItemStack(InvItemStack iis);
	void decrementSelectedItemStack();
	void setModule(int newMod);
	void save(FileHandler* fh, std::string worldDataPath);
	/**/
private:
	/**/
	void tickInventoryOpen();
	void drawInventory();
	void drawInventoryElements(int oscillation);
	void drawHoverText();
	/**/
	int getSlotHoveringX();
	int getSlotHoveringY();
	/**/
	void updateMenuCoordinates();
	void updateSandboxRGB();
	void selectInventorySlot(int x, int y);
	void setInventorySlotItem(int x, int y, int i);
	void setInventorySlotItemStack(int x, int y, InvItemStack iis);
	/**/

	/**/
	SDLHandler* sdlHandler = nullptr;
	GUIHandler* guiHandler = nullptr;
	Controls* controls = nullptr;
	PlayerMenuModules pmm;
	int mod = -1;
	int lastMod = -1;
	
	//Inventory
	//Set texture settings for uiOverlay
	const int uiOverlayDX = -2;
	const int uiOverlayDY = -2;
	const int invW = 8; const int invH = 8;
	std::map<std::pair<int, int>, InvItemStack> inventorySlots;
	
	//Inventory selection and holding
	int invSX = -1; int invSY = -1;		//X, Y of the currently selected inventory slot
	InvItemStack invHeldStack;			//Currently held item stack
	InvItemStack invSelectedStack;		//Currently selected item stack (right click)
	
	bool itemUIShouldUpdate = true;
	bool itemHeldShouldUpdate = true;
	
	//Window and inventory screen
	int windowX = 0, windowY = 0, windowW = 0, windowH = 0;
	int invScreenX = 0, invScreenY = 0, invScreenW = 0, invScreenH = 0;
	
	Texture uiOverlay;
	Texture heldItem;
	Items items;

	int playerGamemode = 0;
};