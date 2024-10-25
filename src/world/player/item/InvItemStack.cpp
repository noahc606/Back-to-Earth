#include "InvItemStack.h"
#include "TextOld.h"
#include "Tile.h"

InvItemStack::InvItemStack(){}
InvItemStack::InvItemStack(int type, int count)
{
    InvItemStack::type = type;
    InvItemStack::count = count;
}

InvItemStack::InvItemStack(int type, int count, int64_t extraData)
{
    InvItemStack::type = type;
    InvItemStack::count = count;
    InvItemStack::extraData = extraData;
}

void InvItemStack::drawCount(SDLHandler* sdlHandler, int drawX, int drawY)
{
    if(count!=1) {
        if(count<0) {
            TextOld::draw(sdlHandler, "INFIN", drawX, drawY+46, 2);
        } else {
            std::stringstream ss;
            ss << count;
            TextOld::draw(sdlHandler, ss.str(), drawX, drawY+46, 2);
        }
    }
}

void InvItemStack::drawEDTileType(SDLHandler* sdlHandler, int drawX, int drawY)
{
    /*
    Tile tt;
    tt.setVal(extraData);

    sdlHandler->setColorMod(TextureLoader::WORLD_TILE_type_a, tt.getRGB(0), tt.getRGB(1), tt.getRGB(2));
    
    SDL_Rect src; src.x = tt.getTextureXYZ(0)*32+8; src.y = tt.getTextureXYZ(1)*32+8; src.w = 16; src.h = 16;
    SDL_Rect dst; dst.x = drawX+14; dst.y = drawY+14; dst.w = 32; dst.h = 32;
    sdlHandler->renderCopy(TextureLoader::WORLD_TILE_type_a, &src, &dst);

    //tt.getRGB()*/
}

int InvItemStack::getType() { return type; }
int InvItemStack::getCount() { return count; }
int64_t InvItemStack::getExtraData() { return extraData; }