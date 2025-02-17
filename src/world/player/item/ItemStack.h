#pragma once
#include "SDLHandler.h"
#include "TileDict.h"

class ItemStack {
public:
    ItemStack();
    ItemStack(int type, int count);
    ItemStack(int type, int count, std::string extraData);

    void draw(SDLHandler* sdlHandler, TileDict* td, int drawX, int drawY);
    void drawCount(SDLHandler* sdlHandler, int drawX, int drawY);
    void drawEDTileType(SDLHandler* sdlHandler, TileDict* td, int drawX, int drawY);

    int getType() const;
    int getCount() const;
    std::string getExtraData() const;
    std::string toString();

    bool stacksWith(const ItemStack& is);

private:
    int type = -1;
    int count = 0;                  //negative values counted as infinity
    std::string extraData = "";     //Extra data
};