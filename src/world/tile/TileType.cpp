#include "TileType.h"
#include <sstream>
#include <string>
#include "DebugScreen.h"
#include "Log.h"

TileType::TileType(){};

void TileType::init()
{
    null = false;
}

std::string TileType::toString()
{
    std::stringstream ss;
    ss << "{";
    ss << "solid=" << isSolid() << ",";
    ss << "visionBlocking=" << isVisionBlocking() << ",";
    ss << "rgb=(" << getRGB(0) << ", " << getRGB(1) << ", " << getRGB(2) << "),";
    ss << "textureXYZ=(" << getTextureXYZ(0) << ", " << getTextureXYZ(1) << ", " << getTextureXYZ(2) << ")";
    ss << "} = ";
	ss << getVal();
	
    return ss.str();
}

void TileType::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << toString();
    DebugScreen::newLine(ss);
}
std::string TileType::getInfo()
{
	std::stringstream ss;
	int tabs;
	info(ss, tabs);
	return ss.str();
}

bool TileType::isNull() { return null; }
uint64_t TileType::getVal() { return value; }
std::tuple<int, int, int> TileType::getRGB()
{
    return std::make_tuple(
        ((value&HexMasks::COLOR_RED  )>>10)*8,
        ((value&HexMasks::COLOR_GREEN)>>5 )*8,
        ((value&HexMasks::COLOR_BLUE )>>0 )*8
    );
}
int TileType::getRGB(int component)
{
    switch( component%3 ) {
        case 0: return std::get<0>(getRGB());
        case 1: return std::get<1>(getRGB());
        case 2: return std::get<2>(getRGB());
    }
    return 0;
}
std::tuple<int, int, int> TileType::getTextureXYZ() {
    return std::make_tuple(
        (value&HexMasks::TEXTURE_X)>>25,
        (value&HexMasks::TEXTURE_Y)>>20,
        (value&HexMasks::TEXTURE_Z)>>15
    );
}
int TileType::getTextureXYZ(int component)
{
    switch( component%3 ) {
        case 0: return std::get<0>(getTextureXYZ());
        case 1: return std::get<1>(getTextureXYZ());
        case 2: return std::get<2>(getTextureXYZ());
    }
    return 0;
}
bool TileType::isVisionBlocking() { return value&HexMasks::VISION_BLOCKING; }
bool TileType::isSolid() { return value&HexMasks::SOLID; }

void TileType::setVal(uint64_t p_val)
{
	value = p_val;
}

void TileType::setRGB( uint8_t p_r, uint8_t p_g, uint8_t p_b )
{
    // Set red bits
    value = value&(~HexMasks::COLOR_RED);
    value = value|( ((uint64_t)p_r/8)<<10 );

    // Set green bits
    value = value&(~HexMasks::COLOR_GREEN);
    value = value|( ((uint64_t)p_g/8)<<5 );

    // Set blue bits
    value = value&(~HexMasks::COLOR_BLUE);
    value = value|( ((uint64_t)p_b/8)<<0 );
}
void TileType::setTextureXYZ( uint8_t p_x, uint8_t p_y, uint8_t p_z )
{
    // Set X location bits
    value = value&(~HexMasks::TEXTURE_X);
    value = value|( ((uint64_t)(p_x%32))<<25 );

    // Set Y location bits
    value = value&(~HexMasks::TEXTURE_Y);
    value = value|( ((uint64_t)(p_y%32))<<20 );

    // Set spriteID bits
    value = value&(~HexMasks::TEXTURE_Z);
    value = value|( ((uint64_t)(p_z%32))<<15 );
}
void TileType::setTextureXY(uint8_t p_x, uint8_t p_y)
{
    setTextureXYZ( p_x, p_y, getTextureXYZ(2) );
}
void TileType::setVisionBlocking( bool p_visionBlocking )
{
    value = value|( ((uint64_t)p_visionBlocking)<<30 );
}
void TileType::setSolid( bool p_solid )
{
    value = value|( ((uint64_t)p_solid)<<31 );
}



bool operator==( TileType &left, TileType &right )
{
    if( left.isNull() && right.isNull() ) return true;
    if( left.isNull() != right.isNull() ) return false;

    return left.value == right.value;
}
