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

    std::tuple<int8_t, int8_t, int8_t> getLastSelectedInvSlot();
    void setLastSelectedInvSlot(int8_t sl, int8_t sx, int8_t sy);

private:
    Inventory* inv;

    bool activePMT = false;
    int8_t selectedHBSlot = 0;
    std::tuple<int8_t, int8_t, int8_t> lastSelectedISlot;
};