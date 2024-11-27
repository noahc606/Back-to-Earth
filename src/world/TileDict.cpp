#include "TileDict.h"
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/timer.h>
#include "TextureBuilder.h"
#include "TextureLoader.h"
#include "World.h"

std::map<std::string, Tile> TileDict::baseTiles = {
//Technical
{ "debug_tile", Tile("debug_tile", R"({
    "id": "debug_tile",
    "skipRendering": false,
    "solid": true,
    "material": "rock",
    "textureHolder": "tile_type_a",
    "textureSpecs": [ { "type": "all", "src": [4, 0], "color": [255, 0, 255] } ]
})"_json) },
{ "null",                   Tile("null",                    R"({ "skipRendering": true, "solid": false })"_json) },

//Natural (part of terrain)
{ "accrio_air",             Tile("accrio_air",              R"({ "skipRendering": true, "solid": false })"_json) },
{ "accrio_native_copper_1", Tile("accrio_native_copper_1",  R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [190, 150, 100] }, { "type": "all", "src": [0, 7], "color": [179, 72, 0], "visionBlocking": false } ] })"_json) },
{ "accrio_native_copper_2", Tile("accrio_native_copper_2",  R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [190, 150, 100] }, { "type": "all", "src": [1, 7], "color": [179, 72, 0], "visionBlocking": false } ] })"_json) },
{ "accrio_native_copper_3", Tile("accrio_native_copper_3",  R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [190, 150, 100] }, { "type": "all", "src": [2, 7], "color": [179, 72, 0], "visionBlocking": false } ] })"_json) },
{ "accrio_regolith",        Tile("accrio_regolith",         R"({ "material": "soil", "textureSpecs": [ { "type": "all", "src": [1, 5], "color": [204, 153,   0] } ] })"_json) },
{ "accrio_rock",            Tile("accrio_rock",             R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [190, 150, 100] } ] })"_json) },
{ "accrio_silt",            Tile("accrio_silt",             R"({ "material": "sand", "textureSpecs": [ { "type": "all", "src": [2, 5], "color": [110,  90,  40] } ] })"_json) },
{ "accrio_soil",            Tile("accrio_soil",             R"({ "material": "soil", "textureSpecs": [ { "type": "all", "src": [3, 1], "color": [128,  50,   0] } ] })"_json) },
{ "hera_air",               Tile("hera_air",                R"({ "skipRendering": true, "solid": false })"_json) },
{ "hera_icesheet",          Tile("hera_icesheet",           R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [3, 3], "color": [ 20, 180, 140] } ], "mapColor": [60, 220, 180] })"_json) },
{ "hera_limestone",         Tile("hera_limestone",          R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [0, 1], "color": [190, 180, 160] } ] })"_json) },
{ "hera_seabed_regolith",   Tile("hera_seabed_regolith",    R"({ "material": "soil", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [150, 190, 100] } ] })"_json) },
{ "hera_seabed_rock",       Tile("hera_seabed_rock",        R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [150, 190, 100] } ] })"_json) },
{ "hera_seabed_soil",       Tile("hera_seabed_soil",        R"({ "material": "soil", "textureSpecs": [ { "type": "all", "src": [4, 1], "color": [150, 190, 100] } ] })"_json) },
{ "hera_seawater",          Tile("hera_seawater",           R"({ "textureSpecs": [ { "type": "all", "src": [0, 4], "color": [20, 180, 140] } ] })"_json) },
{ "granite",                Tile("granite",                 R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [1, 1], "color": [120,  80,  70] } ] })"_json) },
{ "magma",                  Tile("magma",                   R"({ "solid": false, "material": "rock", "textureSpecs": [ { "type": "all", "src": [0, 4], "color": [255, 40, 40] } ] })"_json) },

//Structures (part of structures)
{ "aerospace_acrylic_glass",Tile("aerospace_acrylic_glass", R"({ "material": "rock", "textureSpecs": [ { "type": "all", "src": [0, 6], "color": [ 64,  64,  48], "visionBlocking": false } ] })"_json) },
{ "artificial_air",         Tile("artificial_air",          R"({ "skipRendering": true, "solid": false })"_json) },
{ "hab_titanium_hull",      Tile("hab_titanium_hull",       R"({ "material": "metal","textureSpecs": [ { "type": "all", "src": [0, 3], "color": [255, 255, 255] } ] })"_json) },
{ "hab_futuristic_hull",    Tile("hab_futuristic_hull",     R"({ "material": "metal","textureSpecs": [ { "type": "all", "src": [6, 3], "color": [  0, 255, 200] } ] })"_json) },
{ "monolith",               Tile("monolith",                R"({ "material": "metal","textureSpecs": [ { "type": "all", "src": [2, 3], "color": [ 50,  50,  50] } ] })"_json) },

};

TileDict::TileDict(){}
TileDict::~TileDict() { saveTileDict(); }
void TileDict::init(SDLHandler* sh, std::string saveGameName, std::string instanceID)
{
    /* Store save and instance info */
    TileDict::sdlHandler = sh;
    TileDict::saveGameName = saveGameName;
    TileDict::instanceID = instanceID;

    /* Get corresponding TileDict JSON file */
    std::string jsonpath = "backtoearth/saved/games/"+saveGameName+"/tiledict/"+instanceID+".json";

    //Try to get Json data from file
    nlohmann::json dictJson;
    std::ifstream jsonstream(jsonpath);
    try {
        dictJson = nlohmann::json::parse(jsonstream);
    } catch(...) {}
    jsonstream.close();

    /* Build 'dict' */
    auto tileDefList = dictJson["tileDict"];
    for(int i = 0; i<tileDefList.size(); i++) {
        //Get info (json, ID)
        auto tileDefJson = tileDefList[i];
        std::string id = "null";
        bool failed = false;
        try {
            id = tileDefJson["id"];
        } catch(...) {
            failed = true;
            nch::Log::error(__PRETTY_FUNCTION__, "Missing mandatory \"id\" property for object #%d within \"%s/tiledict/%s.json\".", i, saveGameName.c_str(), instanceID.c_str());
        }

        //If successfully got info...
        if(!failed) {
            Tile t(id, tileDefJson);
            addToDict(id, t);
        }
    }

    //Rebuild base tile definitions (overwrites whatever was retrieved before)
    for(auto itr = baseTiles.begin(); itr!=baseTiles.end(); itr++) {
        setTileDef(itr->first, itr->second);
    }

    /* Build 'tileAtlases', 'tileSrcDefs', and 'atlasObjDefs'. */
    nch::Timer t("TileDict atlas rebuild", true);
    rebuildAtlasesEtcFromDict();
}

Tile TileDict::at(std::string tileID)
{
    auto itr = dict.find(tileID);
    if(itr!=dict.end()) {
        return itr->second;
    } else {
        return Tile();
    }
}

Tile TileDict::at(uint64_t idx)
{
    auto itr = numericDictEx.find(idx);
    if(itr!=numericDictEx.end()) {
        return at(itr->second);
    }
    return Tile();
}

Tile TileDict::getBaseTileDef(std::string baseTileID)
{
    try {
        Tile res = baseTiles[baseTileID];

        return res;
    } catch(...) {}

    nch::Log::log("WARNING");
    return Tile();
}

std::vector<std::string> TileDict::getIDsMatchingDef(Tile tileDef)
{
    std::vector<std::string> res;
    for(auto itr = dict.begin(); itr!=dict.end(); itr++) {
        Tile t = itr->second;
        if(t==tileDef) {
            res.push_back(itr->first);
        }
    }

    return res;
}

std::tuple<Texture*, int, int> TileDict::getTileSrcFromCamDir(Tile tileDef, int camDirection)
{
    //Safety check
    int face = Tile::camDirToRenderFace(camDirection);
    if(face==Tile::UNKNOWN) {
        return std::make_tuple(nullptr, 0, 0);
    }

    //Find within tileSrcDefs
    auto itr = tileSrcDefs.find(tileDef.id);
    if(itr!=tileSrcDefs.end()) {
        int texIdx = std::get<2>(itr->second.loc[face-1]);
        int srcX = std::get<0>(itr->second.loc[face-1]);
        int srcY = std::get<1>(itr->second.loc[face-1]);
        return std::make_tuple(tileAtlases[texIdx].getSheetTexture(), srcX, srcY);
    } else {
        return std::make_tuple(nullptr, 0, 0);
    }
}

uint64_t TileDict::getNumSandboxTiles()
{
    return numSandboxTiles;
}

uint64_t TileDict::getNumericID(std::string id)
{
    auto itr = numericDict.find(id);
    if(itr!=numericDict.end()) {
        return itr->second;
    }
    return -1;
}

void TileDict::drawAtlas(int idx) { tileAtlases[idx].drawSheet(); }

bool TileDict::setTileDef(std::string tileID, Tile tileDef)
{
    //If ID doesn't exist, try to add tile def as normal.
    if(dict.find(tileID)==dict.end()) {
        return addTileDef(tileID, tileDef);
    }

    //If duplicate tileID exists which has the SAME DEFINITION, ignore.
    if(at(tileID)==tileDef) {
        return false;
    }

    //If duplicate tileID exists WITHOUT same definition, remove and replace.
    dict.erase(tileID);
    dict.insert(std::make_pair(tileID, tileDef));
    addToAtlasEtc(tileID, tileDef);

    return true;
}

bool TileDict::addTileDef(std::string tileID, Tile tileDef)
{
    if(!addToDict(tileID, tileDef)) return false;
    addToAtlasEtc(tileID, tileDef);
    return true;
}

bool TileDict::addTileDefUnique(std::string tileID, Tile tileDef)
{
    if(getIDsMatchingDef(tileDef).size()>0) {
        return false;
    }
    bool res = addTileDef(tileID, tileDef);
    return res;
}

void TileDict::saveTileDict()
{
    //Get path, create file, create tileDict array
    std::string jsonPath = "backtoearth/saved/games/"+saveGameName+"/tiledict/"+instanceID+".json";
    nlohmann::json jsonFile;
    nlohmann::json tileDictArr;

    //Go thru dict and add everything to 'jsonFile'.
    for(auto itr = numericDictEx.begin(); itr!=numericDictEx.end(); itr++) {
        //Create tile dict entry
        nlohmann::json tileDictEnt;
        Tile t = at(itr->second);
        
        //Set basic properties
        tileDictEnt["id"] = itr->second;
        tileDictEnt["skipRendering"] = t.skipRendering;
        tileDictEnt["solid"] = t.solid;
        tileDictEnt["material"] = Tile::materialTypeToStr(t.material);
        tileDictEnt["textureHolder"] = t.textureHolder;
        tileDictEnt["mapColor"] = t.mapColor.getRGBTriple();
        //Create and set texture specs
        nlohmann::json texSpecs;
        std::vector<Tile::TexSpec> tss = t.textureSpecs;
        for(int j = 0; j<tss.size(); j++) {
            nlohmann::json texSpecEnt;
            texSpecEnt["type"] = Tile::renderFaceTypeToStr(tss[j].type);
            texSpecEnt["src"] = tss[j].aod.resrc;
            texSpecEnt["color"] = tss[j].aod.color.getRGBTriple();
            texSpecEnt["visionBlocking"] = tss[j].aod.visionBlocking;
            texSpecs.push_back(texSpecEnt);
        }
        tileDictEnt["textureSpecs"] = texSpecs;

        //Add tileDict entry
        tileDictArr.push_back(tileDictEnt);
    }

    //Add tileDict to the file
    jsonFile["tileDict"] = tileDictArr;

    //Save the JSON
    std::ofstream file1(jsonPath);
    file1 << jsonFile;
}

bool TileDict::addToDict(std::string tileID, Tile tileDef)
{
    if(dict.find(tileID)!=dict.end()) {
        nch::Log::warn(__PRETTY_FUNCTION__, "Tried to add duplicate tile ID \"%s\" to %s's tileDict", tileID.c_str(), instanceID.c_str());
        return false;
    }

    dict.insert(std::make_pair(tileID, tileDef));
    numericDict.insert(std::make_pair(tileID, numTiles));
    numericDictEx.insert(std::make_pair(numTiles, tileID));
    nch::Log::log("Assigned ID %d to %s.", numTiles, tileID.c_str());
    numTiles++;

    if(tileID.substr(0, 4).compare("_sbx")==0) {
        numSandboxTiles++;
    }
    return true;
}
void TileDict::addToAtlasEtc(std::string tileID, Tile tileDef)
{
    //Go thru this tile's 't.textureSpecs' and append 'tileSrcDefs' and 'atlasObjDefs'.
    auto itr = dict.find(tileID);
    if(itr==dict.end()) { return; }

    //Try to optimize: If all tile.textureSpecs.faces==ALL, we know we only need one texture.
    Tile tile = itr->second;
    int numDifferentFaces = 1;
    for(int i = 0; i<tile.textureSpecs.size(); i++) {
        if(tile.textureSpecs[i].type!=Tile::RenderFace::ALL) {
            numDifferentFaces = 6;
            break;
        }
    }

    /* Go thru all 'textureSpecs' of this Tile to build 'tsd.loc[0-5]'. */
    TileSrcDef tsd;
    for(int i = 0; i<numDifferentFaces; i++) {
        //Texture used by this face
        Texture tex; tex.init(sdlHandler, 32, 32);
        //AtlasObjectDefs used by this face
        std::vector<Tile::AtlasObjDef> aods;

        //Go thru all textureSpecs that match this face: 1) insert atlasObjectDef; 2) append tex
        for(int j = 0; j<tile.textureSpecs.size(); j++) {
            Tile::RenderFace face = tile.textureSpecs[j].type;
            switch(face) {
                case Tile::RenderFace::ALL: {
                    aods.push_back(tile.textureSpecs[j].aod);
                    appendTexFromSpecs(tex, tile.textureSpecs[j]);
                } break;
                case Tile::RenderFace::WEST: case Tile::RenderFace::NORTH: case Tile::RenderFace::UP:
                case Tile::RenderFace::EAST: case Tile::RenderFace::SOUTH: case Tile::RenderFace::DOWN: {
                    if(face-1==i) {
                        aods.push_back(tile.textureSpecs[j].aod);
                        appendTexFromSpecs(tex, tile.textureSpecs[j]);
                    }
                } break;
            }
        }

        //Find location within atlas of this 'aods' (or get the newly added location if it didn't exist)
        int beforeSize = atlasObjDefs.size();
        std::tuple<int, int, int> aodLoc = insertAtlasObj(aods);
        int afterSize = atlasObjDefs.size();

        //If a new atlas object was just added...
        if(beforeSize!=afterSize) {
            /* Update 'tileAtlases' if needed */
            //Make sure we have enough spritesheets to hold every tile.
            //Every sheet will be 1024x1024, each containing 1024 (32x32) squares.
            if(tileAtlases.size()<std::get<2>(aodLoc)+1) {
                SpriteSheet ta;
                tileAtlases.pushBack(ta);
                tileAtlases[tileAtlases.size()-1].init(sdlHandler, 32*32, 32*32);
            }

            TextureLoader* tl = sdlHandler->getTextureLoader();
            SpriteSheet* ss = &tileAtlases[std::get<2>(aodLoc)];
            SDL_Texture* stex = tex.getSDLTexture();
            ss->addSpriteToFixedSizeSheet(stex, 0, 0);
        }
        
        //Store aodLoc within new tileSrcDef.
        tsd.loc[i] = aodLoc;
    }

    //If numDifferentFaces==1, clone all the faces from tsd[0]
    if(numDifferentFaces==1) for(int i = 1; i<6; i++) tsd.loc[i] = tsd.loc[0];

    //Add 'tsd' to 'tileSrcDefs'
    tileSrcDefs.insert(std::make_pair(tileID, tsd));
}

void TileDict::rebuildAtlasesEtcFromDict()
{
    tileAtlases.clear();
    tileSrcDefs.clear();
    atlasObjDefs.clear();

    for(auto itr = dict.begin(); itr!=dict.end(); itr++) {
        addToAtlasEtc(itr->first, itr->second);
    }
}
void TileDict::appendTexFromSpecs(Texture& tex, Tile::TexSpec ts)
{
    //Draw default tile tex if VB
    if(ts.aod.visionBlocking) {
        TextureBuilder tb(sdlHandler);
        tb.buildDefaultTile(tex);
    }

    tex.setColorMod(ts.aod.color);
    tex.setBlendMode(SDL_BLENDMODE_BLEND);
    tex.lock();
    tex.blit(TextureLoader::WORLD_TILE_type_a, ts.aod.resrc.first*32, ts.aod.resrc.second*32);
}
std::tuple<int, int, int> TileDict::insertAtlasObj(std::vector<Tile::AtlasObjDef>& aods)
{
    //Search list for atlasObjDef.
    int64_t foundDupeAt = atlasObjDefs.size();      //foundDupeAt=aODs.size() indicates none found within list.
    for(int i = 0; i<atlasObjDefs.size(); i++) {
        if(aods.size()!=atlasObjDefs[i].size()) continue;
        bool allMatching = true;
        for(int j = 0; j<aods.size(); j++) {
            if(
                allMatching && (
                atlasObjDefs[i][j].color          != aods[j].color ||
                atlasObjDefs[i][j].resrc          != aods[j].resrc ||
                atlasObjDefs[i][j].visionBlocking != aods[j].visionBlocking
            ) ) {
                allMatching = false;
            }
        }

        if(allMatching) {
            foundDupeAt = i;
            break;
        }
    }

    //Get X, Y, Z of atlasObjDef given the index it was found at.
    int aodX = foundDupeAt%32;
    int aodY = (foundDupeAt/32)%32;
    int aodZ = foundDupeAt/1024;
    
    //If this is a unique atlasObjDef, add it.
    if(foundDupeAt==atlasObjDefs.size()) {
        atlasObjDefs.push_back(aods);
    }

    //Return location where the (new or old) atlasObjDef would be within the 'tileAtlas' spritesheets.
    return std::make_tuple(aodX, aodY, aodZ);
}