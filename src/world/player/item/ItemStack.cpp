#include "ItemStack.h"
#include "PlayerMenu.h"
#include "TextOld.h"
#include "Tile.h"

ItemStack::ItemStack(){}
ItemStack::ItemStack(int type, int count)
{
    ItemStack::type = type;
    ItemStack::count = count;
}

ItemStack::ItemStack(int type, int count, std::string extraData)
{
    ItemStack::type = type;
    ItemStack::count = count;
    ItemStack::extraData = extraData;
}

void ItemStack::draw(SDLHandler* sdlHandler, TileDict* td, int drawX, int drawY)
{
    if(type==-1) return;

    //Draw the item...
    SDL_Rect isrc; isrc.x = PlayerMenu::getItemTexSrcX(type)+1; isrc.y = PlayerMenu::getItemTexSrcY(type)+1; isrc.w = 30; isrc.h = 30;
    SDL_Rect idst; idst.x = drawX; idst.y = drawY; idst.w = 60; idst.h = 60;
    if(type!=Items::WORLDTILE) {
        sdlHandler->renderCopy(TextureLoader::PLAYER_items, &isrc, &idst);
    } else {
        drawEDTileType(sdlHandler, td, idst.x, idst.y);
    }

    drawCount(sdlHandler, drawX, drawY);
}

void ItemStack::drawCount(SDLHandler* sdlHandler, int drawX, int drawY)
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

void ItemStack::drawEDTileType(SDLHandler* sdlHandler, TileDict* td, int drawX, int drawY)
{
    Tile t = td->at(extraData);

    auto tileSrc = td->getTileSrcFromCamDir(t, Camera::DOWN);
    Texture* srcTex = std::get<0>(tileSrc);
    int srcX = 32*std::get<1>(tileSrc);
    int srcY = 32*std::get<2>(tileSrc);
    
    SDL_Rect src;
    src.x = srcX; src.y = srcY; src.w = 32; src.h = 32;
    SDL_Rect dst;
    dst.x = drawX+14; dst.y = drawY+14; dst.w = 32; dst.h = 32;

    if(srcTex==nullptr) return;
    SDL_RenderCopy(sdlHandler->getRenderer(), srcTex->getSDLTexture(), &src, &dst);
}

int ItemStack::getType() { return type; }
int ItemStack::getCount() { return count; }
std::string ItemStack::getExtraData() { return extraData; }

std::string ItemStack::toString()
{
    std::stringstream res;
    res << "\"item\":{";
    res << "\"type\":" << type << ",";
    res << "\"count\":" << count << ",";
    res << "\"data\":\"" << extraData << "\"";
    res << "}";

    return res.str();
}