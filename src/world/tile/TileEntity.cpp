#include "TileEntity.h"
#include "Items.h"

std::map<std::string, int> TileEntity::idToTypeMap = {
    { "antimatter_warhead",     ANTIMATTER_WARHEAD },
    { "cabinet",                CABINET },
    { "plasma_matter_storage",  PLASMA_MATTER_STORAGE },
};

TileEntity::TileEntity(nlohmann::json teJson)
{
    
}
TileEntity::TileEntity(std::string id)
{
    type = getTypeFromID(id);

    //Set inventory type from tile entity type (if applicable, may be NONE)
    int inventoryType = Inventory::NONE;
    switch(type) {
        case PLASMA_MATTER_STORAGE: { inventoryType = Inventory::TE_PLASMA_MATTER_STORAGE; } break;
        case CABINET:               { inventoryType = Inventory::TE_CABINET; } break;
    }
    inv = new Inventory(inventoryType);

    //Depending on inventory type, set some items.
    if(type==PLASMA_MATTER_STORAGE) {
        for(int ix = 0; ix<6; ix++)
        for(int iy = 0; iy<6; iy++)
        inv->setSlotItemStack(ix, iy, ItemStack(Items::WORLDTILE, 100, "hab_futuristic_hull"));   
    }
}
TileEntity::~TileEntity()
{
    if(inv!=nullptr) delete inv;
}

nlohmann::json TileEntity::jsonify()
{
    if(type==NONE) return nlohmann::json::object();

    nlohmann::json res;
    res["type"] = type;
    res["inventory"] = inv->jsonify();

    return res;
}
Inventory* TileEntity::getInventory() { return inv; }
int TileEntity::getType() { return type; }

int TileEntity::getTypeFromID(std::string id)
{
    auto itr = idToTypeMap.find(id);
    if(itr!=idToTypeMap.end())
        return itr->second;
    return NONE;
}