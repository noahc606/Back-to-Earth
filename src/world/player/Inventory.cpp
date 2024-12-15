#include "Inventory.h"
#include "NoahAllocTable.h"

Inventory::Inventory(){}
Inventory::~Inventory(){}

bool Inventory::slotExists(int x, int y)
{
	bool inMain8x8 = x>=0 && x<=7 && y>=0 && y<=7;
	bool inBody1x3 = x==-4 && y>=2 && y<=4;

	switch(type) {
		case BACKPACK: 	{ if(inMain8x8 || inBody1x3) return true; } break;
		case PMT: 	    { if(inMain8x8) return true; } break;
		case SANDBOX: 	{ if(inMain8x8) return true; } break;
	}

	return false;
}

ItemStack Inventory::getSlotItemStack(int x, int y)
{
	if(slotExists(x, y) && inv.find(std::make_pair(x, y))!=inv.end()) {
		return inv.at(std::make_pair(x, y));
	}

	return ItemStack(-1, 0);
}

void Inventory::setSlotItemStack(int x, int y, ItemStack is)
{
	if(inv.find(std::make_pair(x, y))!=inv.end()) {
		inv.at(std::make_pair(x, y)) = is;
	} else {
		inv.insert( std::make_pair(std::make_pair(x, y), is) );
	}
}

void Inventory::load()
{

}

nlohmann::json Inventory::jsonify()
{
    nlohmann::json res;
    for(auto itr = inv.begin(); itr!=inv.end(); itr++) {
        std::stringstream slotEnt;
        slotEnt << "{";
        slotEnt << "\"x\":" << itr->first.first << ",";
        slotEnt << "\"y\":" << itr->first.second << ",";
        slotEnt << itr->second.toString();
        slotEnt << "}";

        nlohmann::json se = nlohmann::json::parse(slotEnt.str());
        res.push_back(se);
    }

    return res;
}