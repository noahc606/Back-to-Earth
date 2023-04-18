#include "TileType.h"
#include <sstream>
#include "DebugScreen.h"
#include <string>

TileType::TileType(){};

void TileType::init()
{
    null = false;
}

std::string TileType::toString()
{
    std::stringstream ss;
    ss << "{";
    ss << "solid=" << solid << ",";
    ss << "visionBlocking=" << visionBlocking << ",";
    ss << "rgb=(" << (int)std::get<0>(rgb) << ", " << (int)std::get<1>(rgb) << ", " << (int)std::get<2>(rgb) << "),";
    ss << "textureXY=(" << (int)std::get<0>(textureXY) << ", " << (int)std::get<1>(textureXY) << ")";
    ss << "}";
    return ss.str();
}

void TileType::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << toString();
    DebugScreen::newLine(ss);
}

bool TileType::isSolid() { return solid; }
bool TileType::isVisionBlocking() { return visionBlocking; }
std::tuple<uint8_t, uint8_t, uint8_t> TileType::getRGB() { return rgb; }
std::tuple<int, int> TileType::getTextureXY() { return textureXY; }
bool TileType::isNull() { return null; }

void TileType::setSolid( bool p_isSolid )
{
    solid = p_isSolid;
}

void TileType::setVisionBlocking( bool p_visionBlocking )
{
    visionBlocking = p_visionBlocking;
}

void TileType::setRGB( uint8_t p_r, uint8_t p_g, uint8_t p_b )
{
    rgb = std::make_tuple( p_r, p_g, p_b );
}

void TileType::setTextureXY( int p_x, int p_y )
{
    textureXY = std::make_tuple( p_x, p_y );
}

bool operator==( TileType &left, TileType &right )
{
    if( left.null==true && right.null==true                ) return true;
    if( left.isNull()           !=right.isNull()           ) return false;

    if( left.isSolid()          !=right.isSolid()          ) return false;
    if( left.isVisionBlocking() !=right.isVisionBlocking() ) return false;
    if( left.getTextureXY()     !=right.getTextureXY()     ) return false;
    if( left.getRGB()           !=right.getRGB()           ) return false;

    return true;
}
