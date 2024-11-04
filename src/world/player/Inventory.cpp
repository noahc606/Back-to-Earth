#include "Inventory.h"
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include <nlohmann/json.hpp>
#include "Items.h"


Inventory::Inventory() { reset(); }
Inventory::~Inventory(){}

void Inventory::gfxUpdateCoordinates(GUIHandler* guiHandler)
{
	//Reset window's XYWH and inventory screen's XYWH
	windowX = 0; windowY = 0;	windowW = 0; windowH = 0;
	screenX = 0; screenY = 0;   screenW = 0; screenH = 0;	
	
	//Set XYWH of window and invScreen from parentWindow and invW/invH
	Window* parentWindow = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( parentWindow!=nullptr ) {
		//Window
		windowX = parentWindow->getSX(); 	windowY = parentWindow->getSY();
		windowW = parentWindow->getWidth(); windowH = parentWindow->getHeight();
		//invScreen
		screenX = windowX+64*4+2*6;		screenW = w*64;
		screenY = windowY+2*4;			screenH = h*64;
	}
}

int Inventory::getMod() { return mod; }
PlayerMenuModules* Inventory::getPMM() { return &pmm; }
int Inventory::getScrX() { return screenX; }
int Inventory::getScrY() { return screenY; }
int Inventory::getUIODX() { return uiOverlayDX; }
int Inventory::getUIODY() { return uiOverlayDY; }
int Inventory::getWidth() { return w; }
int Inventory::getHeight() { return h; }

uint8_t Inventory::getSelLoc() { return sl; }
uint8_t Inventory::getSelX() { return sx; }
uint8_t Inventory::getSelY() { return sy; }

InvItemStack Inventory::getSelItemStack() { return selectedStack; }

bool Inventory::slotExists(int loc, int x, int y)
{
	bool inMain8x8 = x>=0 && x<=7 && y>=0 && y<=7;
	bool inBody1x3 = x==-4 && y>=2 && y<=4;

	switch(loc) {
		case 0: 	{ if(inMain8x8 || inBody1x3) return true; } break;
		case 1: 	{ if(inMain8x8) return true; } break;
		case 100: 	{ if(inMain8x8) return true; } break;
	}

	return false;
}

bool Inventory::slotExists(int x, int y)
{
	switch(mod) {
		case pmm.BACKPACK: 					{ return slotExists(0, x, y); } break;
		case pmm.PLASMA_MATTER_TRANSFORMER: { return slotExists(1, x, y); } break;
		case pmm.CRAFTING: 					{ return (y==0 || y==1) && (x>=0 && x<=7) && slotExists(0, x, y); } break;
		case pmm.SANDBOX:					{ return slotExists(100, x, y); }
	}

	return false;
}

int Inventory::getSlotLoc(int x, int y)
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

InvItemStack Inventory::getSlotItemStack(int loc, int x, int y)
{
	if(slotExists(loc, x, y) && inv[loc].find(std::make_pair(x, y))!=inv[loc].end()) {
		return inv[loc].at(std::make_pair(x, y));
	}

	return InvItemStack(-1, 0);
}

InvItemStack Inventory::getSlotItemStack(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return getSlotItemStack(loc, x, y);
}


int Inventory::getSlotItemType(int loc, int x, int y) { return getSlotItemStack(loc, x, y).getType(); }

int Inventory::getSlotItemType(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return getSlotItemType(loc, x, y);
}

InvItemStack Inventory::getHeldItemStack() { return heldStack; }

void Inventory::reset()
{
	inv.clear();
	inv.insert( std::make_pair(0, std::map<std::pair<int, int>, InvItemStack>()) );	//Backpack
	inv.insert( std::make_pair(1, std::map<std::pair<int, int>, InvItemStack>()) );	//PMT
	inv.insert( std::make_pair(100, std::map<std::pair<int, int>, InvItemStack>()) );	//Sandbox
}

void Inventory::setStarterItems(TileDict* td)
{
	reset();

	setSlotItem(0, -4, 2, Items::QUANTUM_EXOSUIT_HELMET);
	setSlotItem(0, -4, 3, Items::QUANTUM_EXOSUIT_BODY);
	setSlotItem(0, -4, 4, Items::QUANTUM_EXOSUIT_LEGGINGS);

	setSlotItem(0, 3, 0, Items::ION_BLASTER);
	setSlotItem(0, 4, 1, Items::GEOPORTER);
	setSlotItem(0, 6, 2, Items::SOLID_PLASMA_ANNIHILATOR);
	setSlotItem(0, 6, 3, Items::ATOM_PRINTER);

	setSlotItemStack(0, 7, 7, InvItemStack(Items::FOOD_RATION_A, 19));
	setSlotItemStack(0, 7, 5, InvItemStack(Items::FOOD_RATION_B, 20));
	setSlotItemStack(0, 7, 4, InvItemStack(Items::FOOD_RATION_C, 14));
	setSlotItemStack(0, 7, 3, InvItemStack(Items::FOOD_RATION_D, 17));
	setSlotItemStack(0, 6, 7, InvItemStack(Items::FOOD_RATION_E, 29));
	setSlotItemStack(0, 6, 5, InvItemStack(Items::FOOD_RATION_F, 31));

	setSlotItemStack(1, 0, 0, InvItemStack(Items::WORLDTILE, -1, "hab_titanium_hull"));
}

void Inventory::setMod(int newMod) { mod = newMod; }

/*
	Select the slot given by (x, y) within the current inventory. This should be called when the user right clicks.
	Different inventories have different sizes - and a maximum size of 8x8 or (0,0) to (7,7).
*/
bool Inventory::selectSlot(int loc, int x, int y)
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

bool Inventory::selectSlot(int x, int y)
{
	int loc = getSlotLoc(x, y);
	return selectSlot(loc, x, y);
}

void Inventory::setSlotItemStack(int loc, int x, int y, InvItemStack iis)
{
	if(!slotExists(loc, x, y)) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Invalid inventory slot (%d, %d) within loc %d", x, y, loc);
		return;
	}

	if(inv[loc].find(std::make_pair(x, y))!=inv[loc].end()) {
		inv[loc].at(std::make_pair(x, y)) = iis;
	} else {
		inv[loc].insert( std::make_pair(std::make_pair(x, y), iis) );
	}
}
void Inventory::setSlotItemStack(int x, int y, InvItemStack iis) { setSlotItemStack(0, x, y, iis); }
void Inventory::setSlotItem(int loc, int x, int y, int i) { setSlotItemStack(loc, x, y, InvItemStack(i, 1)); }
void Inventory::setSlotItem(int x, int y, int i)
{
	int loc = getSlotLoc(x, y);
	setSlotItem(loc, x, y, i);
}
void Inventory::setHeldItemStack(InvItemStack iis) { heldStack = iis; }

void Inventory::decrementSelectedItemStack()
{
	InvItemStack iss = getSlotItemStack(sl, sx, sy);
	if(iss.getType()==-1 || iss.getCount()==0) {
		return;
	}

	if(iss.getCount()>1) {
		setSlotItemStack(sl, sx, sy, InvItemStack(iss.getType(), iss.getCount()-1, iss.getExtraData()));
	} else if(iss.getCount()==1) {
		setSlotItemStack(sl, sx, sy, InvItemStack(-1, 0));
	} else {
		setSlotItemStack(sl, sx, sy, InvItemStack(iss.getType(), -1, iss.getExtraData()));
	}

	
	selectedStack = getSlotItemStack(sl, sx, sy);
	if(getSlotItemStack(sl, sx, sy).getCount()==0) {
		selectedStack = InvItemStack(-1, 0);
		sx = -1;
		sy = -1;
		sl = -1;
	}
}

void Inventory::load(TileDict* td, std::string worldDirPath)
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
					InvItemStack(slot["item"]["type"], slot["item"]["count"], slot["item"]["data"])
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
					InvItemStack(slot["item"]["type"], slot["item"]["count"], slot["item"]["data"])
				);
			}
		} catch (...) {}
	}

	

	
}

void Inventory::save(std::string worldDirPath)
{

	nch::Log::log("Saving main player data within \"%s\"\n", worldDirPath.c_str());

	nlohmann::json invData;
	for(auto sec : inv) {
		
		nlohmann::json currModSlots;
		for(auto itr = sec.second.begin(); itr!=sec.second.end(); itr++) {
			std::stringstream slotEnt;
			slotEnt << "{";
			slotEnt << "\"x\":" << itr->first.first << ",";
			slotEnt << "\"y\":" << itr->first.second << ",";
			slotEnt << itr->second.toString();
			slotEnt << "}";

			nlohmann::json se = nlohmann::json::parse(slotEnt.str());
			currModSlots.push_back(se);
		}

		std::stringstream modTitle; modTitle << "mod" << sec.first;
		invData[modTitle.str()] = currModSlots;
	}

	auto invDataFinal = nlohmann::json::to_bson(invData);
	nch::NoahAllocTable nat("backtoearth/"+worldDirPath+"/playerdata");
	nat.save("inventory", invDataFinal);
}