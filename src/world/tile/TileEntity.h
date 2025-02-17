#pragma once
#include "Inventory.h"
#include <nch/math-utils/vec3.h>
#include <nlohmann/json.hpp>

class TileEntity {
public:
    enum TileEntityType {
        NONE,
        ANTIMATTER_WARHEAD,
        CABINET,
        PLASMA_MATTER_STORAGE,
    };

    TileEntity(nlohmann::json teJson);
    TileEntity(std::string id);
    ~TileEntity();

    nlohmann::json jsonify();
    Inventory* getInventory();
    int getType();

    static int getTypeFromID(std::string id);

private:
    Inventory* inv = nullptr;
    int type = NONE;

    static std::map<std::string, int> idToTypeMap;
};