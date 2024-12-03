#pragma once
#include <map>
#include "GUIHandler.h"
#include "InvItemStack.h"
#include "PlayerMenuModules.h"
#include "Tile.h"
#include "TileDict.h"

class Inventory {
public:
    Inventory();
    ~Inventory();

	void gfxUpdateCoordinates(GUIHandler* guiHandler);

	int getMod();
	PlayerMenuModules* getPMM();
	int getScrX(); int getScrY();
	int getUIODX(); int getUIODY();
	int getWidth(); int getHeight();
	int8_t getSelLoc();
	int8_t getSelX();
	int8_t getSelY();
	InvItemStack getSelItemStack();
	bool slotExists(int loc, int x, int y);
	bool slotExists(int x, int y);
	int getSlotLoc(int x, int y);
	InvItemStack getSlotItemStack(int loc, int x, int y);
	InvItemStack getSlotItemStack(int x, int y);
	int getSlotItemType(int loc, int x, int y);
	int getSlotItemType(int x, int y);
	InvItemStack getHeldItemStack();

	void reset();
	void setStarterItems(TileDict* td);
	void setMod(int newMod);
	bool selectSlot(int loc, int x, int y);
	bool selectSlot(int x, int y);
	void setSlotItemStack(int loc, int x, int y, InvItemStack iis);
	void setSlotItemStack(int x, int y, InvItemStack iis);
	void setSlotItem(int loc, int x, int y, int i);
	void setSlotItem(int x, int y, int i);
	void setHeldItemStack(InvItemStack iis);
	void decrementSelectedItemStack();

	void load(TileDict* td, std::string worldDirPath);
	void save(std::string worldDirPath);

private:
	/* Inventory feautres */
	//Slots
	std::map<int, std::map<std::pair<int, int>, InvItemStack>> inv;
	//Selection &  Holding
	int8_t sl, sx, sy = -1;		//(L)ocation, X, Y of the currently selected inventory slot
	InvItemStack heldStack;     //Currently held item stack
	InvItemStack selectedStack; //Currently selected item stack (right click)
	//Player Menu Modules
	int mod = -1;
	PlayerMenuModules pmm;

	/* Inventory graphics */
	const int uiOverlayDX = -2;
	const int uiOverlayDY = -2;
	const int w = 8; const int h = 8;
	int windowX = 0, windowY = 0, windowW = 0, windowH = 0;
	int screenX = 0, screenY = 0, screenW = 0, screenH = 0;
};