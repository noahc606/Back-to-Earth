#include "Tile.h"
#include <nch/cpp-utils/log.h>
#include "Camera.h"

Tile::Tile(std::string id, nlohmann::json tileDef)
{
    construct(id, tileDef);
}

Tile::Tile(std::string id, bool solid, std::pair<int, int> resrc, nch::Color color, bool visionBlocking)
{
    nlohmann::json tileDef;
    tileDef["id"] = id;
    tileDef["solid"] = solid;
    
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

bool Tile::operator==(Tile& other)
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
    if(textureHolder!=other.textureHolder) return false;

    //If no mismatches found, return true
    return true;
}

bool Tile::operator!=(Tile& other)
{
    return !((*this)==other);
}

bool Tile::isVisionBlocking(int camDirection)
{
    if(skipRendering) return false;

    for(int i = 0; i<textureSpecs.size(); i++) {
        TexSpec ts = textureSpecs[i];
        
        if((ts.type==RenderFaces::ALL || ts.type==camDirection) && ts.aod.visionBlocking) {
            return true;
        }
    }
    return false;
}

Tile::RenderFaces Tile::strToRenderFacesType(std::string rfStr)
{
    if(rfStr=="all")    return RenderFaces::ALL;
    if(rfStr=="west")   return RenderFaces::WEST;
    if(rfStr=="east")   return RenderFaces::EAST;
    if(rfStr=="north")  return RenderFaces::NORTH;
    if(rfStr=="south")  return RenderFaces::SOUTH;
    if(rfStr=="up")     return RenderFaces::UP;
    if(rfStr=="down")   return RenderFaces::DOWN;
    return RenderFaces::UNKNOWN;
}

std::string Tile::renderFacesTypeToStr(Tile::RenderFaces rf)
{
    switch(rf) {
        case RenderFaces::ALL:   return "all";
        case RenderFaces::WEST:  return "west";
        case RenderFaces::EAST:  return "east";
        case RenderFaces::NORTH: return "north";
        case RenderFaces::SOUTH: return "south";
        case RenderFaces::UP:    return "up";
        case RenderFaces::DOWN:  return "down";
    }
    return "unknown";
}

Tile::RenderFaces Tile::camDirToRenderFace(int camDir)
{
    switch(camDir)
    {
        case Camera::WEST:  return RenderFaces::EAST;
        case Camera::EAST:  return RenderFaces::WEST;
        case Camera::NORTH: return RenderFaces::SOUTH;
        case Camera::SOUTH: return RenderFaces::NORTH;
        case Camera::UP:    return RenderFaces::DOWN;
        case Camera::DOWN:  return RenderFaces::UP;
    }
    return RenderFaces::UNKNOWN;
}

void Tile::construct(std::string id, nlohmann::json tileDef)
{
    Tile::id = id;

    /* Get properties within the tileDef json, load them into vars */
    //Simple properties
    try { skipRendering = tileDef["skipRendering"]; }   catch(...) {}
    try { solid = tileDef["solid"]; }                   catch(...) {}
    try { textureHolder = tileDef["textureHolder"]; }  catch(...) {}
    
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
            try { ts.type = strToRenderFacesType(ctrj["type"]); }               catch(...) {}
            try { ts.aod.visionBlocking = (ctrj["visionBlocking"]); }            catch(...) {}
            try { ts.aod.resrc = std::make_pair(ctrj["src"][0], ctrj["src"][1]); } catch(...) {}
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