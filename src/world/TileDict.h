#pragma once
#include <nch/cpp-utils/arraylist.h>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <set>
#include <string>
#include "Texture.h"
#include "SDLHandler.h"
#include "SpriteSheet.h"
#include "Tile.h"


class TileDict {
public:
    struct TileSrcDef {
        //tuple<srcX, srcY, atlasIndex>
        std::tuple<int, int, int> loc[6]; //WENSUD
    };

    TileDict();
    ~TileDict();

    void init(SDLHandler* sh, std::string saveGameName, std::string instanceID);
    void drawAtlas(SDLHandler* sh, int idx);

    Tile at(std::string tileID);
    Tile at(uint64_t idx);
    static nlohmann::json getBaseTileDictJson();
    std::vector<std::string> getIDsMatchingDef(Tile tileDef);
    std::tuple<Texture*, int, int> getTileSrcFromCamDir(Tile tileDef, int camDirection);
    uint64_t getNumSandboxTiles();
    uint64_t getNumericID(std::string id);
    

    bool addTileDef(SDLHandler* sh, std::string tileID, Tile tileDef);
    bool addTileDefUnique(SDLHandler* sh, std::string tileID, Tile tileDef);
    void saveTileDict();

private:
    std::string saveGameName = "world_unknown";
    std::string instanceID = "default";
    
    /*
        The list ("dictionary") of all tiles that corresponds to the current loaded instance (planet).
        This data is saved in & loaded from 'saved/games/[save-name]/tiledict/[planet-name].json'.

        Notes:
        - The dict["null"] Tile must exist and represents an empty vacuum. This special null tile is also used in world and structure generation to represent nothing.
        - Lookup time (e.g. dict["a_tile_id"]) will be O(1)
    */
    std::map<std::string, Tile> dict;
    std::map<std::string, uint64_t> numericDict;
    std::map<uint64_t, std::string> numericDictEx;
    std::unordered_map<std::string, TileSrcDef> tileSrcDefs;
    nch::ArrayList<SpriteSheet> tileAtlases;
    nch::ArrayList<Tile::AtlasObjDef> atlasObjDefs;
    uint64_t numSandboxTiles = 0;
    uint64_t numTiles = 0;

    void rebuildAtlasesEtcFromDict(SDLHandler* sh);
    bool addToDict(std::string tileID, Tile tileDef);
    void addToAtlasEtc(SDLHandler* sh, std::string tileID, Tile tileDef);
    void appendTexFromSpecs(SDLHandler* sh, Texture& tex, Tile::TexSpec specs);
    std::tuple<int, int, int> insertAtlasObj(SDLHandler* sh, Tile::AtlasObjDef& aod);
};