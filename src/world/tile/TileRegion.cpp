#include "TileRegion.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <sstream>
#include "DebugScreen.h"
#include "Log.h"
#include "TileMap.h"

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

TileRegion::~TileRegion() {}

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
void TileRegion::setTile( int x, int y, int z, uint16_t paletteIndex )
{
    tiles[x][y][z] = paletteIndex;
}

void TileRegion::setTiles( int x1, int y1, int z1, int x2, int y2, int z2, TileType tile )
{
    setTiles( addToPalette(tile), x1, y1, z1, x2, y2, z2 );
}

void TileRegion::setTiles( int x1, int y1, int z1, int x2, int y2, int z2, uint16_t paletteIndex )
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


int TileRegion::addToPalette( TileType tile )
{
    //Try to find the tile in the vector using the == operator (custom definition in Tile.h)
    int i;
    for( i = 0; i<(int)palette.size(); i++ ) {
        if( tile==palette[i] ) {
            break;
        }
    }

    //If tile is not found in the vector, add the tile to the vector.
    if( i==(int)palette.size() ) {
        palette.push_back( tile );
    }

    //Return the index of the tile found. If tile wasn't found, this will be palette.size()
    return i;
}
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
    palette.clear();

    for( int x = 0; x<32; x++ ) {
        for( int y = 0; y<32; y++ ) {
            for( int z = 0; z<32; z++ ) {
                addToPalette( getTile(x, y, z) );
            }
        }
    }
}

/**
    Saves this region to disk.

    Automatically called whenever:
        -This region needs to be unloaded (compress==true)
        -This region is auto-saved (compress==false)
*/
void TileRegion::save(bool p_compress)
{
    if( p_compress ) {
        compress();
    }

    //Implement saving
}
