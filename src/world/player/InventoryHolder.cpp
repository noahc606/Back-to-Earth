#include "InventoryHolder.h"
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include <nlohmann/json.hpp>
#include "Items.h"


InventoryHolder::InventoryHolder()
{
	mod = -1;
	reset();
}
InventoryHolder::~InventoryHolder(){}

int InventoryHolder::getMod() { return mod; }
PlayerMenuModules* InventoryHolder::getPMM() { return &pmm; }

int8_t InventoryHolder::getSelLoc() { return sl; }
int8_t InventoryHolder::getSelX() { return sx; }
int8_t InventoryHolder::getSelY() { return sy; }

ItemStack InventoryHolder::getSelItemStack() { return selectedStack; }

bool InventoryHolder::slotExists(int loc, int x, int y) { return invs[loc].slotExists(x, y); }
bool InventoryHolder::slotExists(int x, int y)
{
	switch(mod) {
		case pmm.BACKPACK: 					{ return slotExists(Inventory::Type::BACKPACK, x, y); } break;
		case pmm.PLASMA_MATTER_TRANSFORMER: { return slotExists(Inventory::Type::PMT, x, y); } break;
		case pmm.CRAFTING: 					{ return (y==0) && (x>=0 && x<=7) && slotExists(Inventory::Type::BACKPACK, x, y); } break;
		case pmm.SANDBOX:					{ return slotExists(Inventory::Type::SANDBOX, x, y); } break;

		case pmm.TILE_ENTITY: {

		} break;
	}

	return false;
}

int InventoryHolder::getSlotLoc(int x, int y)
{
	int loc = -1;
	switch(mod) {
		case pmm.BACKPACK: 					{ loc = 0; } break;
		case pmm.PLASMA_MATTER_TRANSFORMER: { loc = 1; } break;
		case pmm.CRAFTING: 					{ loc = 0; } break;
		case pmm.SANDBOX:					{ loc = 100; } break;
	}
	if(!slotExists(x, y)) loc = -1;
	return loc;
}

ItemStack InventoryHolder::getSlotItemStack(int loc, int x, int y) { return invs[loc].getSlotItemStack(x, y); }
ItemStack InventoryHolder::getSlotItemStack(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return getSlotItemStack(loc, x, y);
}

int InventoryHolder::getSlotItemType(int loc, int x, int y) { return getSlotItemStack(loc, x, y).getType(); }
int InventoryHolder::getSlotItemType(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return getSlotItemType(loc, x, y);
}

ItemStack InventoryHolder::getHeldItemStack() { return heldStack; }

void InventoryHolder::reset()
{
	invs.clear();
	invs.insert( std::make_pair(Inventory::BACKPACK, Inventory()) );		//Backpack
	invs.insert( std::make_pair(Inventory::PMT, Inventory()) );		//PMT
	invs.insert( std::make_pair(Inventory::SANDBOX, Inventory()) );	//Sandbox
}

void InventoryHolder::setStarterItems(TileDict* td)
{
	reset();

	int t0 = Inventory::BACKPACK;
	int t1 = Inventory::PMT;

	setSlotItem(t0, -4, 2, Items::QUANTUM_EXOSUIT_HELMET);
	setSlotItem(t0, -4, 3, Items::QUANTUM_EXOSUIT_BODY);
	setSlotItem(t0, -4, 4, Items::QUANTUM_EXOSUIT_LEGGINGS);

	setSlotItemStack(t0, 3, 3, ItemStack(Items::ANTIMATTER_WARHEAD, 16));
	setSlotItemStack(t0, 2, 3, ItemStack(Items::PLASMA_MATTER_STORAGE, 16));

	setSlotItem(t0, 3, 0, Items::ION_BLASTER);
	setSlotItem(t0, 4, 1, Items::GEOPORTER);
	setSlotItem(t0, 6, 2, Items::SOLID_PLASMA_ANNIHILATOR);
	setSlotItem(t0, 6, 3, Items::ATOM_PRINTER);

	setSlotItemStack(t0, 7, 7, ItemStack(Items::FOOD_RATION_A, 19));
	setSlotItemStack(t0, 7, 5, ItemStack(Items::FOOD_RATION_B, 20));
	setSlotItemStack(t0, 7, 4, ItemStack(Items::FOOD_RATION_C, 14));
	setSlotItemStack(t0, 7, 3, ItemStack(Items::FOOD_RATION_D, 17));
	setSlotItemStack(t0, 6, 7, ItemStack(Items::FOOD_RATION_E, 29));
	setSlotItemStack(t0, 6, 5, ItemStack(Items::FOOD_RATION_F, 31));

	setSlotItemStack(t1, 0, 0, ItemStack(Items::WORLDTILE, -1, "hab_titanium_hull"));
}

void InventoryHolder::setMod(int newMod) { mod = newMod; }

/*
	Select the slot given by (x, y) within the current inventory. This should be called when the user right clicks.
	Different inventories have different sizes - and a maximum size of 8x8 or (0,0) to (7,7).
*/
bool InventoryHolder::selectSlot(int loc, int x, int y)
{
	bool itemUIShouldUpdate = false;

	//If selection is out of bounds
	if(!slotExists(loc, x, y)) {
		sl = -1;
		sx = -1;
		sy = -1;
		return false;
	}
	
	//If previous selected item slot not empty, we are selecting a new item
	if( getSlotItemType(loc, sx, sy)!=-1 ) {
		itemUIShouldUpdate = true;
	}
		
	//Set invSX and invSY to x, y
	sl = loc;
	sx = x;
	sy = y;
	selectedStack = getSlotItemStack(loc, x, y);
	return itemUIShouldUpdate;
}

bool InventoryHolder::selectSlot(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return selectSlot(loc, x, y);
}

void InventoryHolder::setSlotItemStack(int loc, int x, int y, ItemStack is)
{
	if(!slotExists(loc, x, y)) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Invalid inventory slot (%d, %d) within loc %d", x, y, loc);
		return;
	}

	invs[loc].setSlotItemStack(x, y, is);
}
void InventoryHolder::setSlotItemStack(int x, int y, ItemStack is) { setSlotItemStack(0, x, y, is); }
void InventoryHolder::setSlotItem(int loc, int x, int y, int i) { setSlotItemStack(loc, x, y, ItemStack(i, 1)); }
void InventoryHolder::setSlotItem(int x, int y, int i)
{
	int loc = getSlotLoc(x, y);
	setSlotItem(loc, x, y, i);
}
void InventoryHolder::setHeldItemStack(ItemStack is) { heldStack = is; }

void InventoryHolder::decrementSelectedItemStack()
{
	ItemStack is = getSlotItemStack(sl, sx, sy);
	if(is.getType()==-1 || is.getCount()==0) {
		return;
	}

	if(is.getCount()>1) {
		setSlotItemStack(sl, sx, sy, ItemStack(is.getType(), is.getCount()-1, is.getExtraData()));
	} else if(is.getCount()==1) {
		setSlotItemStack(sl, sx, sy, ItemStack(-1, 0));
	} else {
		setSlotItemStack(sl, sx, sy, ItemStack(is.getType(), -1, is.getExtraData()));
	}

	
	selectedStack = getSlotItemStack(sl, sx, sy);
	if(getSlotItemStack(sl, sx, sy).getCount()==0) {
		selectedStack = ItemStack(-1, 0);
		sx = -1;
		sy = -1;
		sl = -1;
	}
}

void InventoryHolder::load(TileDict* td, std::string worldDirPath)
{
	//Open NAT, get inventory data
	nch::NoahAllocTable nat("backtoearth/"+worldDirPath+"/playerdata");
	auto invDataRaw = nat.load("inventory");
	
	if(invDataRaw.size()==0) {  /* If no existing inventory data */
		//Give starter items
		setStarterItems(td);
	} else {					/* If inventory data does exist */
		//Load inventory from file
		reset();
		
		nlohmann::json invData = nlohmann::json::from_bson(invDataRaw);

		//Mod0
		try {
			nlohmann::json sec = invData["mod0"];
			for(int i = 0; i<sec.size(); i++) {
				nlohmann::json slot = sec[i];
				setSlotItemStack(
					0, slot["x"], slot["y"],
					ItemStack(slot["item"]["type"], slot["item"]["count"], slot["item"]["data"])
				);
			}
		} catch (...) {}

		//Mod1
		try {
			nlohmann::json sec = invData["mod1"];
			for(int i = 0; i<sec.size(); i++) {
				nlohmann::json slot = sec[i];
				setSlotItemStack(
					1, slot["x"], slot["y"],
					ItemStack(slot["item"]["type"], slot["item"]["count"], slot["item"]["data"])
				);
			}
		} catch (...) {}
	}

	

	
}

void InventoryHolder::save(std::string worldDirPath)
{

	nch::Log::log("Saving main player data within \"%s\"\n", worldDirPath.c_str());

	nlohmann::json invData;
	for(auto inv : invs) {

		nlohmann::json currModSlots = inv.second.jsonify();

		std::stringstream modTitle; modTitle << "mod" << inv.first;
		invData[modTitle.str()] = currModSlots;
	}

	auto invDataFinal = nlohmann::json::to_bson(invData);
	nch::NoahAllocTable nat("backtoearth/"+worldDirPath+"/playerdata");
	nat.save("inventory", invDataFinal);
}