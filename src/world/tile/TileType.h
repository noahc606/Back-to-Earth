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

    bool isSolid();
    bool isVisionBlocking();
    std::tuple<uint8_t, uint8_t, uint8_t> getRGB();
    std::tuple<int, int> getTextureXY();

    void setSolid(bool solid);
    void setVisionBlocking(bool visible);
    void setRGB(uint8_t r, uint8_t g, uint8_t b);
    void setTextureXY(int x, int y);

    bool isNull();

    friend bool operator==( TileType& left, TileType& right );

protected:

private:
    bool solid = false;
    bool visionBlocking = true;
    std::tuple<uint8_t, uint8_t, uint8_t> rgb = std::make_tuple(0, 0, 0);
    std::tuple<int, int> textureXY = std::make_tuple(0, 0);

    bool null = true;
};
