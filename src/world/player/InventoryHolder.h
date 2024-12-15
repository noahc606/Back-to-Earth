#pragma once
#include <map>
#include "GUIHandler.h"
#include "Inventory.h"
#include "ItemStack.h"
#include "PlayerMenuModules.h"
#include "Tile.h"
#include "TileDict.h"

class InventoryHolder {
public:
    InventoryHolder();
    ~InventoryHolder();

	int getMod();
	PlayerMenuModules* getPMM();
	int8_t getSelLoc();
	int8_t getSelX();
	int8_t getSelY();
	ItemStack getSelItemStack();
	bool slotExists(int loc, int x, int y);
	bool slotExists(int x, int y);
	int getSlotLoc(int x, int y);
	ItemStack getSlotItemStack(int loc, int x, int y);
	ItemStack getSlotItemStack(int x, int y);
	int getSlotItemType(int loc, int x, int y);
	int getSlotItemType(int x, int y);
	ItemStack getHeldItemStack();

	void reset();
	void setStarterItems(TileDict* td);
	void setMod(int newMod);
	bool selectSlot(int loc, int x, int y);
	bool selectSlot(int x, int y);
	void setSlotItemStack(int loc, int x, int y, ItemStack is);
	void setSlotItemStack(int x, int y, ItemStack is);
	void setSlotItem(int loc, int x, int y, int i);
	void setSlotItem(int x, int y, int i);
	void setHeldItemStack(ItemStack is);
	void decrementSelectedItemStack();

	void load(TileDict* td, std::string worldDirPath);
	void save(std::string worldDirPath);

private:
	/* Inventory feautres */
	//Slots
	std::map<int, Inventory> invs;
	Inventory* interactingInv = nullptr;

	//Selection &  Holding
	int8_t sl, sx, sy = -1;		//(L)ocation, X, Y of the currently selected inventory slot
	ItemStack heldStack;     //Currently held item stack
	ItemStack selectedStack; //Currently selected item stack (right click)
	//Player Menu Modules
	int mod = -1;
	PlayerMenuModules pmm;
};