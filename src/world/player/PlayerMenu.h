#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
#include "Hotbar.h"
#include "Inventory.h"
#include "InvItemStack.h"
#include "Items.h"
#include "MissionHolder.h"
#include "Player.h"
#include "PlayerMenuModules.h"
#include "TileDict.h"
#include <map>

class PlayerMenu {
public:
	/**/
	/**/
	void init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl, TileDict* td);
	void tick();
	void draw(MissionHolder& mh);
	void drawHotbar();
	/**/
	static int getItemTexSrcX(int itemID);
	static int getItemTexSrcY(int itemID);
	std::string getSandboxTileID();
	Inventory* getInventory();
	/**/
	void giveItemStack(InvItemStack iis);
	void decrementSelectedItemStack();
	void setModule(int newMod);
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
	void updateSandboxTile();
	/**/

	/**/
	SDLHandler* sdlHandler = nullptr;
	GUIHandler* guiHandler = nullptr;
	Controls* controls = nullptr;
	TileDict* tileDict = nullptr;
	
	int playerGamemode = 0;
	Inventory inv;
	Hotbar hotbar;
	
	Texture uiOverlay;
	Texture heldItem;
	bool itemUIShouldUpdate = true;
	bool itemHeldShouldUpdate = true;

	Items items;
	Tile sandboxTile;
};