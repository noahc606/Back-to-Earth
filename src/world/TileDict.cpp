#include "TileDict.h"
#include <nch/cpp-utils/log.h>
#include "TextureBuilder.h"
#include "TextureLoader.h"
#include "World.h"

TileDict::TileDict(){}
TileDict::~TileDict() {
    saveTileDict();
}

void TileDict::init(SDLHandler* sh, std::string saveGameName, std::string instanceID)
{
    /* Store save and instance info */
    TileDict::instanceID = instanceID;
    TileDict::saveGameName = saveGameName;

    /* Get corresponding TileDict JSON file */
    std::string jsonpath = "backtoearth/saved/games/"+saveGameName+"/tiledict/"+instanceID+".json";

    //Get already-existing Json from file
    nlohmann::json dictJson;
    std::ifstream jsonstream(jsonpath);
    try {
        dictJson = nlohmann::json::parse(jsonstream);    
    //If json doesn't exist, build base tile definitions
    } catch(...) {
        dictJson = getBaseTileDictJson();
    }
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


    /* Build 'tileAtlases', 'tileSrcDefs', and 'atlasObjDefs'. */
    rebuildAtlasesEtcFromDict(sh);
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

nlohmann::json TileDict::getBaseTileDictJson()
{
    return R"(
        {"tileDict":[
        {
            "id": "debug_tile",
            "skipRendering": false,
            "solid": true,
            "textureHolder": "tile_type_a",
            "textureSpecs": [ { "type": "all", "src": [4, 0], "color": [255, 0, 255] } ]
        },
        { "id": "null", "skipRendering": true, "solid": false },
        { "id": "generic_air", "skipRendering": true, "solid": false },
        { "id": "breathable_air", "skipRendering": true, "solid": false },
        { "id": "accrio_air", "skipRendering": true, "solid": false },
        { "id": "hera_air", "skipRendering": true, "solid": false },
        { "id": "accrio_regolith",          "textureSpecs": [ { "type": "all", "src": [1, 5], "color": [204, 153, 0] } ] },
        { "id": "accrio_soil",              "textureSpecs": [ { "type": "all", "src": [3, 1], "color": [128, 50, 0] } ] },
        { "id": "accrio_rock",              "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [190, 150, 100] } ] },
        { "id": "accrio_silt",              "textureSpecs": [ { "type": "all", "src": [2, 5], "color": [110, 90, 40] } ] },
        { "id": "hera_seabed_regolith",     "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [150, 190, 100] } ] },
        { "id": "hera_seabed_soil",         "textureSpecs": [ { "type": "all", "src": [4, 1], "color": [150, 190, 100] } ] },
        { "id": "hera_seabed_rock",         "textureSpecs": [ { "type": "all", "src": [2, 1], "color": [150, 190, 100] } ] },
        { "id": "hera_limestone",           "textureSpecs": [ { "type": "all", "src": [0, 1], "color": [190, 180, 160] } ] },
        { "id": "hera_seawater",            "textureSpecs": [ { "type": "all", "src": [0, 4], "color": [20, 180, 140] } ] },
        { "id": "hera_icesheet",            "textureSpecs": [ { "type": "all", "src": [3, 3], "color": [20, 180, 140] } ], "mapColor": [60, 220, 180] },
        { "id": "granite",                  "textureSpecs": [ { "type": "all", "src": [1, 1], "color": [120, 80, 70] } ] },
        { "id": "magma",  "solid": false,   "textureSpecs": [ { "type": "all", "src": [0, 4], "color": [255, 40, 40] } ] },
        { "id": "aerospace_acrylic_glass",  "textureSpecs": [ { "type": "all", "src": [0, 6], "color": [64, 64, 48], "visionBlocking": false } ] },
        { "id": "hab_titanium_hull",        "textureSpecs": [ { "type": "all", "src": [0, 3], "color": [255, 255, 255] } ] },
        { "id": "monolith",                 "textureSpecs": [ { "type": "all", "src": [2, 3], "color": [50, 50, 50] } ] }
        ]}
    )"_json;
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

void TileDict::drawAtlas(SDLHandler* sh, int idx) { tileAtlases[idx].drawSheet(); }

bool TileDict::addTileDef(SDLHandler* sh, std::string tileID, Tile tileDef)
{
    if(!addToDict(tileID, tileDef)) return false;
    addToAtlasEtc(sh, tileID, tileDef);
    return true;
}

bool TileDict::addTileDefUnique(SDLHandler* sh, std::string tileID, Tile tileDef)
{
    if(getIDsMatchingDef(tileDef).size()>0) {
        return false;
    }
    bool res = addTileDef(sh, tileID, tileDef);
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
        tileDictEnt["textureHolder"] = t.textureHolder;
        tileDictEnt["mapColor"] = t.mapColor.getRGBTriple();
        //Create and set texture specs
        nlohmann::json texSpecs;
        std::vector<Tile::TexSpec> tss = t.textureSpecs;
        for(int j = 0; j<tss.size(); j++) {
            nlohmann::json texSpecEnt;
            texSpecEnt["type"] = Tile::renderFacesTypeToStr(tss[j].type);
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
void TileDict::addToAtlasEtc(SDLHandler* sh, std::string tileID, Tile tileDef)
{
    /* Update 'tileAtlases' if needed */
    //Make sure we have enough spritesheets to hold every tile.
    //Every sheet will be 1024x1024, each containing 1024 (32x32) squares.
    int numSheets = dict.size()/1024+1;
    if(tileAtlases.size()<numSheets) {
        SpriteSheet ta;
        tileAtlases.pushBack(ta);
        tileAtlases[tileAtlases.size()-1].init(sh, 32*32, 32*32);
    }

    /* Update */
    //Go thru this tile's 't.textureSpecs' and append 'tileSrcDefs' and 'atlasObjDefs'.
    auto itr = dict.find(tileID);
    if(itr!=dict.end()) {
        Tile tile = itr->second;
        TileSrcDef tsd;
        
        /* Go thru all 'textureSpecs' of this Tile to build 'tsd'. */
        for(int j = 0; j<tile.textureSpecs.size(); j++) {
            Texture tex;
            tex.init(sh, 32, 32);

            //Try to insert atlasObjDef (may create new image or use an old one). Store location of the atlasObjDef.
            int beforeSize = atlasObjDefs.size();
            std::tuple<int, int, int> aodLoc = insertAtlasObj(sh, tile.textureSpecs[j].aod);
            int afterSize = atlasObjDefs.size();

            //Based on the face to be rendered (all or WENSUD) build the appropriate tex within 'texes'.
            Tile::RenderFaces face = tile.textureSpecs[j].type;
            switch(face) {
                case Tile::RenderFaces::ALL: {
                    appendTexFromSpecs(sh, tex, tile.textureSpecs[j]);
                    for(int k = 0; k<6; k++) {
                        tsd.loc[k] = aodLoc;
                    }
                } break;
                case Tile::RenderFaces::WEST: case Tile::RenderFaces::NORTH: case Tile::RenderFaces::UP:
                case Tile::RenderFaces::EAST: case Tile::RenderFaces::SOUTH: case Tile::RenderFaces::DOWN: {
                    appendTexFromSpecs(sh, tex, tile.textureSpecs[j]);
                    tsd.loc[face-1] = aodLoc;
                } break;
                default: {
                    nch::Log::warnv(__PRETTY_FUNCTION__, "using default tile texture", "Encountered unknown texSpec.type %d from definition of \"%s\"", face, itr->first.c_str());
                    TextureBuilder tb(sh);
                    tb.buildDefaultTile(tex);
                    for(int k = 0; k<6; k++) {
                        tsd.loc[k] = aodLoc;
                    }
                } break;
            }

            //If a new atlas object was just added...
            if(beforeSize!=afterSize) {
                TextureLoader* tl = sh->getTextureLoader();
                SpriteSheet* ss = &tileAtlases[std::get<2>(aodLoc)];
                SDL_Texture* stex = tex.getSDLTexture();
                ss->addSpriteToFixedSizeSheet(stex, 0, 0);
                
            }
        }

        /* Add 'tsd' to 'tileSrcDefs' */
        tileSrcDefs.insert(std::make_pair(itr->first, tsd));
    }
}
void TileDict::rebuildAtlasesEtcFromDict(SDLHandler* sh)
{
    tileAtlases.clear();
    tileSrcDefs.clear();
    atlasObjDefs.clear();

    for(auto itr = dict.begin(); itr!=dict.end(); itr++) {
        addToAtlasEtc(sh, itr->first, itr->second);
    }
}
void TileDict::appendTexFromSpecs(SDLHandler* sh, Texture& tex, Tile::TexSpec ts)
{
    //Draw default tile tex if VB
    if(ts.aod.visionBlocking) {
        TextureBuilder tb(sh);
        tb.buildDefaultTile(tex);
    }

    tex.setColorMod(ts.aod.color);
    tex.setBlendMode(SDL_BLENDMODE_BLEND);
    tex.lock();
    tex.blit(TextureLoader::WORLD_TILE_type_a, ts.aod.resrc.first*32, ts.aod.resrc.second*32);
}
std::tuple<int, int, int> TileDict::insertAtlasObj(SDLHandler* sh, Tile::AtlasObjDef& aod)
{
    //Search list for atlasObjDef.
    int64_t foundDupeAt = atlasObjDefs.size();      //foundDupeAt=aODs.size() indicates none found within list.
    for(int i = 0; i<atlasObjDefs.size(); i++) {
        if(
            atlasObjDefs[i].color==aod.color &&
            atlasObjDefs[i].resrc==aod.resrc &&
            atlasObjDefs[i].visionBlocking==aod.visionBlocking
        ) {
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
        atlasObjDefs.pushBack(aod);
    }

    //Return location where the (new or old) atlasObjDef would be within the 'tileAtlas' spritesheets.
    return std::make_tuple(aodX, aodY, aodZ);
}