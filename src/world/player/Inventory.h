#pragma once
#include <map>
#include "ItemStack.h"
#include <nlohmann/json.hpp>


class Inventory {
public:
    enum Type {
        BACKPACK = 0,
        PMT = 1,

        SANDBOX = 100,
    };

    Inventory();
    ~Inventory();

    bool slotExists(int x, int y);
    ItemStack getSlotItemStack(int x, int y);

    void setSlotItemStack(int x, int y, ItemStack is);

    void load();
    nlohmann::json jsonify();
private:
    int type = 0;
    std::map<std::pair<int, int>, ItemStack> inv;
};