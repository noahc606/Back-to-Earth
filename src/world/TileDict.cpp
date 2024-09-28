#include "TileDict.h"
#include <nch/cpp-utils/log.h>

TileDict::TileDict(SDLHandler* sh)
{
    add("null", Tile());

    tileAtlas.init(sh);
    tileAtlas.setSheetDimensions(32*32, 32*32);
    
    
}

TileDict::~TileDict()
{
    tileAtlas.destroy();
}

bool TileDict::add(std::string tileID, Tile tileDef)
{
    if(dict.find(tileID)!=dict.end()) {
        nch::Log::warn(__PRETTY_FUNCTION__, "Tried to add duplicate tile ID \"%s\" to %s's tileDict", tileID.c_str(), instanceID.c_str());
        return false;
    }

    dict.insert(std::make_pair(tileID, tileDef));
    return true;
}