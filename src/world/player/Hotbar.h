#pragma once
#include "Controls.h"
#include "InventoryHolder.h"
#include "SDLHandler.h"

class Hotbar {
public:
    Hotbar();
    ~Hotbar();
    void init(InventoryHolder* invhdr);

    void tick(Controls* ctrls);
    void draw(SDLHandler* sh, TileDict* td);

    std::tuple<int8_t, int8_t, int8_t> getLastSelectedInvSlot();
    bool isPMTActive();

    void setLastSelectedInvSlot(int8_t sl, int8_t sx, int8_t sy);

private:
    InventoryHolder* invhdr;

    bool activePMT = false;
    int8_t selectedHBSlot = 0;
    std::tuple<int8_t, int8_t, int8_t> lastSelectedISlot;
};