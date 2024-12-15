#include "Tile.h"
#include <nch/cpp-utils/log.h>
#include "Camera.h"

Tile::Tile(std::string id, nlohmann::json tileDef, bool tileEntity)
{
    construct(id, tileDef);
    Tile::tileEntity = tileEntity;
}

Tile::Tile(std::string id, nlohmann::json tileDef): Tile::Tile(id, tileDef, false){}

Tile::Tile(std::string id, bool solid, std::pair<int, int> resrc, nch::Color color, bool visionBlocking)
{
    nlohmann::json tileDef;
    tileDef["id"] = id;
    tileDef["solid"] = solid;
    tileDef["material"] = ROCK;

    nlohmann::json texSpecJson;
    texSpecJson["type"] = "all";
    texSpecJson["src"] = { resrc.first, resrc.second };
    texSpecJson["color"] = color.getRGBTriple();
    texSpecJson["visionBlocking"] = visionBlocking;
    tileDef["textureSpecs"].push_back(texSpecJson);

    construct(id, tileDef);
}

Tile::Tile(){}
Tile::~Tile(){}

/*
    NOTE: TileA==TileB does NOT imply TileA.id==TileB.id.
    Only the "tile definition" (set of properties which does not include ID) has to be the same.
*/
bool Tile::operator==(const Tile& other)
{
    //Check (# of texture specs) mismatch
    if(textureSpecs.size()!=other.textureSpecs.size()) {
        return false;
    }

    //Check texture specs for mismatches
    for(int i = 0; i<textureSpecs.size(); i++) {
        TexSpec ts0 = textureSpecs[i];
        TexSpec ts1 = other.textureSpecs[i];

        if(ts0.aod.color!=ts1.aod.color) return false;
        if(ts0.aod.resrc!=ts1.aod.resrc) return false;
        if(ts0.aod.visionBlocking!=ts1.aod.visionBlocking) return false;
        if(ts0.type!=ts1.type) return false;
    }

    //Check tileDict properties for mismatches
    if(skipRendering!=other.skipRendering) return false;
    if(solid!=other.solid) return false;
    if(material!=other.material) return false;
    if(tileEntity!=other.tileEntity) return false;
    if(mapColor!=other.mapColor) return false;
    if(textureHolder!=other.textureHolder) return false;

    //If no mismatches found, return true
    return true;
}

bool Tile::operator!=(const Tile& other)
{
    return !((*this)==other);
}

bool Tile::isVisionBlocking(int camDirection)
{
    if(skipRendering) return false;

    for(int i = 0; i<textureSpecs.size(); i++) {
        TexSpec ts = textureSpecs[i];
        
        if((ts.type==RenderFace::ALL || ts.type==camDirection) && ts.aod.visionBlocking) {
            return true;
        }
    }
    return false;
}

Tile& Tile::operator=(const Tile& other)
{
    id = other.id;
    skipRendering = other.skipRendering;
    solid = other.solid;
    material = other.material;
    tileEntity = other.tileEntity;
    mapColor = other.mapColor;
    textureHolder = other.textureHolder;
    textureSpecs = other.textureSpecs;

    return (*this);
}

Tile::RenderFace Tile::strToRenderFaceType(std::string rfStr)
{
    if(rfStr=="all")    return RenderFace::ALL;
    if(rfStr=="x")    return RenderFace::X;
    if(rfStr=="y")    return RenderFace::Y;
    if(rfStr=="z")    return RenderFace::Z;
    if(rfStr=="west")   return RenderFace::WEST;
    if(rfStr=="east")   return RenderFace::EAST;
    if(rfStr=="north")  return RenderFace::NORTH;
    if(rfStr=="south")  return RenderFace::SOUTH;
    if(rfStr=="up")     return RenderFace::UP;
    if(rfStr=="down")   return RenderFace::DOWN;
    return RenderFace::UNKNOWN;
}

std::string Tile::renderFaceTypeToStr(Tile::RenderFace rf)
{
    switch(rf) {
        case RenderFace::ALL:   return "all";
        case RenderFace::WEST:  return "west";
        case RenderFace::EAST:  return "east";
        case RenderFace::NORTH: return "north";
        case RenderFace::SOUTH: return "south";
        case RenderFace::UP:    return "up";
        case RenderFace::DOWN:  return "down";
    }
    return "unknown";
}

Tile::RenderFace Tile::camDirToRenderFace(int camDir)
{
    switch(camDir)
    {
        case Camera::WEST:  return RenderFace::EAST;
        case Camera::EAST:  return RenderFace::WEST;
        case Camera::NORTH: return RenderFace::SOUTH;
        case Camera::SOUTH: return RenderFace::NORTH;
        case Camera::UP:    return RenderFace::DOWN;
        case Camera::DOWN:  return RenderFace::UP;
    }
    return RenderFace::UNKNOWN;
}

Tile::Material Tile::strToMaterialType(std::string mtStr)
{
    if(mtStr=="metal") return METAL;
    if(mtStr=="rock") return ROCK;
    if(mtStr=="soil") return SOIL;
    if(mtStr=="sand") return SAND;
    return GENERIC;
}

std::string Tile::materialTypeToStr(Material m)
{
    switch(m) {
        case METAL: return "metal";
        case ROCK: return "rock";
        case SOIL: return "soil";
        case SAND: return "sand";
    }
    return "generic";
}

int Tile::getTextureHolderID()
{
    if(textureHolder=="tile_type_a") return 0;
    if(textureHolder=="tile_type_b") return 1;
    return 0;
}


void Tile::construct(std::string id, nlohmann::json tileDef)
{
    Tile::id = id;

    /* Get properties within the tileDef json, load them into vars */
    //Simple properties
    try { skipRendering = tileDef["skipRendering"]; }           catch(...) {}
    try { solid = tileDef["solid"]; }                           catch(...) {}
    try { material = strToMaterialType(tileDef["material"]); }  catch(...) {}
    try { textureHolder = tileDef["textureHolder"]; }           catch(...) {}
    
    bool mapColorChanged = false;
    try {
        mapColor = nch::Color(
            (uint8_t)tileDef["mapColor"][0],
            (uint8_t)tileDef["mapColor"][1],
            (uint8_t)tileDef["mapColor"][2]
        );
        mapColorChanged = true;
    } catch(...) {}

    //textureSpecs property
    textureSpecs.clear();
    nlohmann::json texSpecsJson = tileDef["textureSpecs"];
    if(!texSpecsJson.is_null()) {
        for(int i = 0; i<texSpecsJson.size(); i++) {
            auto ctrj = texSpecsJson[i];    //Current textureSpecs json
            std::stringstream ss;
            ss << "textureSpecs[" << i << "]";
            TexSpec ts = TexSpec();
            
            //Populate as many fields as possible (warn if error)
            try { ts.type = strToRenderFaceType(ctrj["type"]); }                    catch(...) {}
            try { ts.aod.visionBlocking = (ctrj["visionBlocking"]); }               catch(...) {}
            try { ts.aod.resrc = std::make_pair(ctrj["src"][0], ctrj["src"][1]); }  catch(...) {}
            try {
                ts.aod.color = nch::Color(
                    (uint8_t)ctrj["color"][0],
                    (uint8_t)ctrj["color"][1],
                    (uint8_t)ctrj["color"][2]
                );

                if(!mapColorChanged) {
                    mapColor = ts.aod.color;
                    mapColorChanged = true;
                }
            } catch(...) {}


            if(skipRendering) {
                ts.aod.visionBlocking = false;
            }

            textureSpecs.push_back(ts);
        }
    } else {
        if(!skipRendering) {
            nch::Log::warnv(__PRETTY_FUNCTION__, "using default textureSpecs", "Could not find textureSpecs property for tile ID \"%s\"", id.c_str());
        }
        textureSpecs = { TexSpec() };
    }
}