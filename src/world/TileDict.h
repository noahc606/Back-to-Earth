#pragma once
#include <unordered_map>
#include <string>
#include "Texture.h"
#include "SDLHandler.h"
#include "SpriteSheet.h"
#include "Tile.h"

class TileDict {
public:
    TileDict(SDLHandler* sh);
    ~TileDict();

    bool add(std::string tileID, Tile tileDef);

private:
    SpriteSheet tileAtlas;
    std::string instanceID = "default";

    
    /*
        The list ("dictionary") of all tiles within the current loaded instance (planet).
        This data is saved in & loaded from 'saved/games/[save-name]/tilemap/[planet-name]/tiledict.json'.

        Notes:
        - The dict["null"] Tile must exist and represents an empty vacuum. This special null tile is also used in world and structure generation to represent nothing.
        - Lookup time (e.g. dict["a_tile_id"]) will be O(1)
    */
    std::unordered_map<std::string, Tile> dict;

};