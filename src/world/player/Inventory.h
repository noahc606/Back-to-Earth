#pragma once
#include <map>
#include "ItemStack.h"
#include <nlohmann/json.hpp>


class Inventory {
public:
    enum Type {
        NONE = -1,
        BACKPACK = 0,
        PMT = 1,
        SANDBOX = 100,

        //Tile Entity Inventories (NOT ALL TE's have inventories)
        TE_CABINET = 1000,
        TE_PLASMA_MATTER_STORAGE,
    };

    Inventory(int type);
    Inventory();
    ~Inventory();

    int getType();
    bool slotExists(int x, int y);
    ItemStack getSlotItemStack(int x, int y);

    void setSlotItemStack(int x, int y, ItemStack is);

    void load();
    nlohmann::json jsonify();
private:
    int type = NONE;
    std::map<std::pair<int, int>, ItemStack> inv;
};