#include "TileRegion.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include "DebugScreen.h"
#include "Log.h"
#include "TileMap.h"
#include "Timer.h"

const uint16_t TileRegion::ampSBits[] = {
        0b1000000000000000,
        0b0100000000000000,
        0b0010000000000000,
        0b0001000000000000,
        0b0000100000000000,
        0b0000010000000000,
        0b0000001000000000,
        0b0000000100000000,
        0b0000000010000000,
        0b0000000001000000,
        0b0000000000100000,
        0b0000000000010000,
        0b0000000000001000,
        0b0000000000000100,
        0b0000000000000010,
        0b0000000000000001 };

const uint8_t TileRegion::pows2[] = { 32, 16, 8, 4, 2, 1 };

/**
(X+, Y+, Z+) = (East, South, Up)
*/
TileRegion::TileRegion()
{
    //Create 'empty' tile
    TileType space;
    space.init();
    space.setVisionBlocking(false);
    int id = addToPaletteFast(space);

    //Fill region with empty tile
    for( int x = 0; x<32; x++ ) {
        for(int y = 0; y<32; y++ ) {
            for(int z = 0; z<32; z++ ) {
                tiles[x][y][z] = id;
            }
        }
    }
}

TileRegion::~TileRegion(){}

void TileRegion::info(std::stringstream& ss, int& tabs, int subX, int subY, int subZ)
{
    DebugScreen::indentLine(ss, tabs);
    ss << "palette.size()=" << (int)palette.size() << "; ";
    ss << "Selected Tile [" << subX << "][" << subY << "][" << subZ << "]";
    DebugScreen::newLine(ss);

    TileType tt = getTile(subX, subY, subZ);
    if( tt.isNull() ) {
        DebugScreen::newGroup(ss, tabs, "Selected TileType: null");
        tt.info(ss, tabs);
    } else {
        DebugScreen::newGroup(ss, tabs, "Selected TileType");
        tt.info(ss, tabs);
    }
    DebugScreen::endGroup(tabs);
}

TileType TileRegion::getTile( int x, int y, int z )
{
    return palette[tiles[x][y][z]];
}

TileType TileRegion::getTileSafe( int x, int y, int z )
{
    unsigned int index = tiles[x][y][z];

    if( index>palette.size() )
        return palette[index];

    return TileType();
}

int TileRegion::getRegTexState() { return regTexState; }

/**
    General-purpose setTile function.
    Performance:
        -This is expensive when used in multidimensional loops - use setTiles() to fill any rectangular-prism-area with a single type of tile.
        -Use setTile(int, int, int, int) whenever you want to place tiles directly from the palette (for example, during region generation).
*/
void TileRegion::setTile( int x, int y, int z, TileType tile )
{
    tiles[x][y][z] = addToPalette(tile);
}

/**
    A faster setTile function that copies a tile from the palette directly and places it somewhere.
    Cannot be used to generate a new tile (again, only tiles from the palette can be used).
*/
void TileRegion::setTile( int x, int y, int z, int16_t paletteIndex )
{
    tiles[x][y][z] = paletteIndex;
}

void TileRegion::setTiles( int x1, int y1, int z1, int x2, int y2, int z2, TileType tile )
{
    setTiles( addToPalette(tile), x1, y1, z1, x2, y2, z2 );
}

void TileRegion::setTiles( int x1, int y1, int z1, int x2, int y2, int z2, int16_t paletteIndex )
{
    for( int x = x1; x<x2; x++ ) {
        for( int y = y1; y<y2; y++ ) {
            for( int z = z1; z<z2; z++ ) {
                tiles[x][y][z] = paletteIndex;
            }
        }
    }
}

void TileRegion::setRegTexState(int p_rts)
{
    regTexState = p_rts;
}

void TileRegion::resetRegTexState()
{
    regTexState = 0;
}


int TileRegion::addToPalette( TileType tile, t_palette& pal)
{
    //Try to find the tile in the palette using the == operator (custom definition in Tile.h)
    int i;
    for( i = 0; i<(int)pal.size(); i++ ) {
        if( tile==pal[i] ) {
            break;
        }
    }

    //If tile is not found in the palette, add the tile to the palette.
    if( i==(int)pal.size() ) {
        pal.push_back( tile );
    }

    //Return the index of the tile found. If tile wasn't found, this will be pal.size()
    return i;
}

int TileRegion::addToPalette( TileType tile ) { return addToPalette(tile, palette); }


/**
    Ignores checking the entire palette for copies of a tiletype.
*/
int TileRegion::addToPaletteFast( TileType tile )
{
    int i = palette.size();
    palette.push_back( tile );
    return i;
}


/**
    Compresses the tile palette. Rarely, there could be unused or repeated tiles in a palette - this function takes care of that.
    This is a relatively expensive function since it needs to "rebuild" the entire palette from scratch - just don't use it *too* often.
    Automatically called when:
        -A 'hard save' occurs (when this region is unloaded - NOT when this region is auto-saved)
        -a TileRegion with palette size>60000 (theoretical max 65535) is detected - very rare circumstance.
*/
void TileRegion::compress()
{
    //Build new tile palette.
    t_palette paletteNew;
    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            for( int z = 0; z<32; z++ ) {
                addToPalette( getTile(x, y, z), paletteNew );
            }
        }
    }

    //Replace old palette with new palette
    palette = paletteNew;
}

/**
    Saves this region to disk.

    Automatically called whenever:
        -This region needs to be unloaded (compress==true). More expensive option
        -This region is auto-saved (compress==false). Less expensive option
*/
void TileRegion::save(SDLHandler* sh, FileHandler* fh, std::string path, long rX, long rY, long rZ, bool p_compress)
{
    std::stringstream filename;
    filename << "saved\\games\\" << path << "\\" << rX << "," << rY << "," << rZ;
    fh->cEditFile( FilePath(filename.str(), "bte_tr", sh->getFilesystemType() ) );

    if( p_compress ) {
        Timer t1;
        compress();
    }

    std::string line1;
    const std::string::size_type new_capacity{ 100000u };
    line1.reserve( new_capacity );

    line1 += "Region("; line1 += std::to_string(rX); line1 += ","; line1 += std::to_string(rY); line1 += ","; line1 += std::to_string(rZ); line1 += ").palette={";
    for(unsigned int i = 0; i<palette.size()-1; i++) {
        TileType tt = palette[i];
        line1 += tt.toString();
        line1 += ",";
    }
    line1 += palette[palette.size()-1].toString();
    line1 += "}, ";

    //8 characters = 3 tiles
    //1 tile = 16 bits, each character = 6bits
    int x = 0;
    int y = 0;
    int z = 0;
    uint8_t counter = 0;
    uint8_t bits = 0;
    char base64Char = 64;

    line1 += "Region("; line1 += rX; line1 += ","; line1 += rY; line1 += ","; line1 += rZ; line1+= ").data={";

    while(true) {
        if( (tiles[x][y][z]&ampSBits[bits])>0 ) {
            base64Char += pows2[counter];
        }

        counter++;
        if(counter==6) {
            line1 += base64Char;
            base64Char = 64;
            counter = 0;
        }

        bits++;
        if(bits==16) {
            bits = 0;
            x++;
            if(x>31) {
                x = 0;
                y++;
                if( y>31 ) {
                    line1 += base64Char;
                    base64Char = 64;
                    counter = 0;
                    y = 0;
                    z++;
                    if( z>31 ) {
                        z = 0; break;
                    } else {
                        line1 += ";";
                    }
                }
            }
        }
    }
    line1 += "}\n";

    int strSize = 100000;//line1.length();

    std::stringstream prefix;
    std::stringstream numSS; numSS << strSize; std::string num = numSS.str();
    //num must be less than 1000000!
    if(strSize>999999) num = "ERROR!";
    while( num.length()<6 ) {
        num = "0"+num;
    }
    prefix << "[" << num << "]! "; //Length of prefix will always be 10 (4 characters + 6 numerical chracters)

    fh->write(prefix.str());
    fh->write(line1);
    fh->saveAndCloseFile();
}
