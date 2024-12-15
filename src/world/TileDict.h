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
    void drawAtlas(int idx);

    Tile at(std::string tileID);
    Tile at(uint64_t idx);
    Tile getBaseTileDef(std::string baseTileID);
    std::vector<std::string> getIDsMatchingDef(Tile tileDef);
    std::tuple<Texture*, int, int> getTileSrcFromCamDir(Tile tileDef, int camDirection);
    uint64_t getNumSandboxTiles();
    uint64_t getNumericID(std::string id);
    
    bool setTileDef(std::string tileID, Tile tileDef);
    bool addTileDef(std::string tileID, Tile tileDef);
    bool addTileDefUnique(std::string tileID, Tile tileDef);
    void saveTileDict();

private:
    SDLHandler* sdlHandler;
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
    
    //Tile atlas + atlas src definitions
    std::unordered_map<std::string, TileSrcDef> tileSrcDefs;
    nch::ArrayList<SpriteSheet> tileAtlases;
    //Atlas objects list
    std::vector<std::vector<Tile::AtlasObjDef>> atlasObjDefs;
    uint64_t numSandboxTiles = 0;
    uint64_t numTiles = 0;

    static std::map<std::string, Tile> baseTiles;

    void rebuildAtlasesEtcFromDict();
    bool addToDict(std::string tileID, Tile tileDef);
    void addToAtlasEtc(std::string tileID, Tile tileDef);
    void appendTexFromSpecs(Texture& tex, Tile::TexSpec specs, int textureHolderID);
    std::tuple<int, int, int> insertAtlasObj(std::vector<Tile::AtlasObjDef>& aods);
};