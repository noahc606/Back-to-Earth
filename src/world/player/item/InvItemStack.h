#pragma once
#include "SDLHandler.h"
#include "TileDict.h"

class InvItemStack {
public:
    InvItemStack();
    InvItemStack(int type, int count);
    InvItemStack(int type, int count, std::string extraData);

    void drawCount(SDLHandler* sdlHandler, int drawX, int drawY);
    void drawEDTileType(SDLHandler* sdlHandler, TileDict* td, int drawX, int drawY);

    int getType();
    int getCount();
    std::string getExtraData();
    std::string toString();

private:
    int type = -1;
    int count = 0;                  //negative values counted as infinity
    std::string extraData = "";     //Extra data
};