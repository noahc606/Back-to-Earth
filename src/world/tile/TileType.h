#pragma once
#include <string>
#include <tuple>

class TileType
{
public:
    TileType();
    void init();

    std::string toString();
    void info(std::stringstream& ss, int& tabs);

    bool isNull();
    uint64_t getVal();
    std::tuple<int, int, int> getRGB();
    int getRGB(int component);
    std::tuple<int, int, int> getTextureXYZ();
    int getTextureXYZ(int component);
    bool isSolid();
    bool isVisionBlocking();

    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setTextureXYZ(uint8_t x, uint8_t y, uint8_t z);
    void setTextureXY(uint8_t x, uint8_t y);
    void setVisionBlocking(bool visible);
    void setSolid(bool solid);

    friend bool operator==( TileType& left, TileType& right );

protected:

private:

    // Theoretical limit of 2^64=18.44 quintillion possible TileTypes.
    // Player will never actually see every possible tile type in game, probably only < 100k different types
    uint64_t value = 0x0000000000000000;

    // Masks used to get a tile type property within the value.
    // For example: To get the 15 bit color value, use value&HexMasks:COLOR
    enum HexMasks {
        COLOR           = 0x0000000000007FFF, //15bits: render color
        COLOR_BLUE      = 0x000000000000001F, //    ...blue (5bits)
        COLOR_GREEN     = 0x00000000000003E0, //    ...green (5bits)
        COLOR_RED       = 0x0000000000007C00, //    ...red (5bits)
        TEXTURE         = 0x000000003FFF8000, //15bits: texture sheet location
        TEXTURE_Z       = 0x00000000000F8000, //    ...Z (5bits)
        TEXTURE_Y       = 0x0000000001F00000, //    ...Y (5bits)
        TEXTURE_X       = 0x000000003E000000, //    ...X (5bits)
        VISION_BLOCKING = 0x0000000040000000, //1bit:   visionBlocking = true/false
        SOLID           = 0x0000000080000000, //1bit:   solid          = true/false
    };

    bool null = true;
};
