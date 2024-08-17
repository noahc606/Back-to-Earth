#pragma once
#include "SDLHandler.h"
#include "TileType.h"

class InvItemStack {
public:
    InvItemStack();
    InvItemStack(int type, int count);
    InvItemStack(int type, int count, int64_t extraData);

    void drawCount(SDLHandler* sdlHandler, int drawX, int drawY);
    void drawEDTileType(SDLHandler* sdlHandler, int drawX, int drawY);

    int getType();
    int getCount();
    int64_t getExtraData();



private:
    int type = -1;
    int count = 0;  //negative values counted as infinity
    int64_t extraData = 0;    //Tile type to place IF this is a tile item
};