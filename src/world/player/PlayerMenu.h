#pragma once
#include "SDLHandler.h"
#include "GUIHandler.h"
#include "Hotbar.h"
#include "InventoryHolder.h"
#include "ItemStack.h"
#include "Items.h"
#include "MissionHolder.h"
#include "Player.h"
#include "PlayerMenuModules.h"
#include "TileDict.h"
#include <map>
#include <nch/math-utils/vec3.h>

class PlayerMenu {
public:
	struct InvGfxManager {
		const int uiOverlayDX = -2;
		const int uiOverlayDY = -2;
		const int w = 8; const int h = 8;
		int windowX = 0, windowY = 0, windowW = 0, windowH = 0;
		int screenX = 0, screenY = 0, screenW = 0, screenH = 0;
	};

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
	InventoryHolder* getInventoryHolder();
	bool isHotbarPMTActive();

	/**/
	void giveItemStack(ItemStack is);
	void decrementSelectedItemStack();
	void setModule(int newMod);
	void setInteractingTileEntity(TileMap* tm, nch::Vec3<int64_t> pos);
	void resetInteractingTileEntity();
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
	void updateInvCoordinates();
	/**/

	/**/
	SDLHandler* sdlHandler = nullptr;
	GUIHandler* guiHandler = nullptr;
	Controls* controls = nullptr;
	TileDict* tileDict = nullptr;
	
	int playerGamemode = 0;
	//Player Inventory, graphics
	InventoryHolder invhdr;
	InvGfxManager invGfx;
	int lastInvMod = -1;
	nch::Vec3<int64_t> interactingPos;
	uint64_t timeMenuOpenedMS = 0;
	//Player Hotbar
	Hotbar hotbar;

	Texture uiOverlay;
	Texture heldItem;
	bool itemUIShouldUpdate = true;
	bool itemHeldShouldUpdate = true;

	Items items;
	Tile sandboxTile;
};