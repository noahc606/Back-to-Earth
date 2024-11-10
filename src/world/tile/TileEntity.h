#pragma once
#include "Inventory.h"

class TileEntity {
public:
    enum TileEntityType {
        NONE,
        PMT_STORAGE_UNIT,
        BACKPACK,
        CABINET,
    };

    TileEntity(int type);
    ~TileEntity();

private:
    Inventory* inv = nullptr;
    int type = NONE;

};