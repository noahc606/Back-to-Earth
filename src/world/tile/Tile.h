#pragma once
#include <vector>
#include <nch/cpp-utils/color.h>
#include <nlohmann/json.hpp>

class Tile {
public:
    enum RenderFaces {
        UNKNOWN, WEST, EAST, NORTH, SOUTH, UP, DOWN, ALL
    };
    struct AtlasObjDef {
        bool visionBlocking = true;
        std::pair<int, int> resrc = std::make_pair(4, 0);
        nch::Color color = nch::Color(255, 0, 255);
    };
    struct TexSpec {
        RenderFaces type = RenderFaces::ALL;
        AtlasObjDef aod;
    };
    
    Tile(std::string id, nlohmann::json tileDef);
    Tile(std::string id, bool solid, std::pair<int, int> resrc, nch::Color color, bool visionBlocking);
    Tile();
    ~Tile();
    static RenderFaces strToRenderFacesType(std::string rfStr);
    static std::string renderFacesTypeToStr(RenderFaces rf);
    static RenderFaces camDirToRenderFace(int camDir);

    bool operator==(Tile& other);
    bool operator!=(Tile& other);
    bool isVisionBlocking(int renderFace);

    std::string id = "null";
    bool skipRendering = false;
    bool solid = true;
    nch::Color mapColor = nch::Color(0, 0, 0);
    std::string textureHolder = "tile_type_a";
    std::vector<TexSpec> textureSpecs = {
        TexSpec()
    };
private:
    void construct(std::string id, nlohmann::json tileDef);
};