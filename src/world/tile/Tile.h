#pragma once
#include <vector>
#include <nch/cpp-utils/color.h>
#include <nlohmann/json.hpp>

class Tile {
public:
    enum RenderFace {
        UNKNOWN, WEST, EAST, NORTH, SOUTH, UP, DOWN, ALL, X, Y, Z   //Do NOT change order of these
    };
    enum Material {
        GENERIC, METAL, ROCK, SOIL, SAND
    };
    struct AtlasObjDef {
        bool visionBlocking = true;
        std::pair<int, int> resrc = std::make_pair(4, 0);
        nch::Color color = nch::Color(255, 255, 255);
    };
    struct TexSpec {
        RenderFace type = RenderFace::ALL;
        AtlasObjDef aod;
    };
    
    Tile(std::string id, nlohmann::json tileDef, bool tileEntity);
    Tile(std::string id, nlohmann::json tileDef);
    Tile(std::string id, bool solid, std::pair<int, int> resrc, nch::Color color, bool visionBlocking);
    Tile();
    ~Tile();
    static RenderFace strToRenderFaceType(std::string rfStr);
    static std::string renderFaceTypeToStr(RenderFace rf);
    static RenderFace camDirToRenderFace(int camDir);
    static Material strToMaterialType(std::string mtStr);
    static std::string materialTypeToStr(Material m);
    int getTextureHolderID();
    bool operator==(const Tile& other);
    bool operator!=(const Tile& other);
    bool isVisionBlocking(int renderFace);

    Tile& operator=(const Tile& other);

    std::string id = "null";
    bool skipRendering = false;
    bool solid = true;
    Material material = GENERIC;
    bool tileEntity = false;
    nch::Color mapColor = nch::Color(0, 0, 0);
    std::string textureHolder = "tile_type_a";
    std::vector<TexSpec> textureSpecs = {
        TexSpec()
    };
private:
    void construct(std::string id, nlohmann::json tileDef);
};