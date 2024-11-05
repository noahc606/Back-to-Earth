#pragma once
#include "Controls.h"
#include "Inventory.h"
#include "SDLHandler.h"

class Hotbar {
public:
    Hotbar();
    ~Hotbar();
    void init(Inventory* inv);

    void tick(Controls* ctrls);
    void draw(SDLHandler* sh, TileDict* td);

private:
    Inventory* inv;

    bool activePMT = false;
    int selectedSlot = 0;
};