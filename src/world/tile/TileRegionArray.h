#pragma once
#include <cstdint>
#include <stddef.h>

/*
    Data structure to hold TileRegion's tile array data. In
    different cases, the size of the array must be different
    (structures: only a few bits per element; world: 16 bits
    per element).
*/

class TileRegionArray {
public:
    typedef struct { uint8_t arr[32][32][4]; } Tiles1Bit;   //Tile value range: 0 to 1 (each array element uses 0 to 255 for a group of 8 tiles)
    typedef struct { uint8_t arr[32][32][8]; } Tiles2Bit;   //Tile value range: 0 to 3 (each element is a group of 4 tiles)
    typedef struct { uint8_t arr[32][32][16]; } Tiles4Bit;  //Tile value range: 0 to 15 (each element is a group of 2 tiles)
    typedef struct { uint8_t arr[32][32][32]; } Tiles8Bit;  //Tile value range: 0 to 255
    typedef struct { int16_t arr[32][32][32]; } Tiles16Bit; //Tile value range: -32768 to 32767

    TileRegionArray(size_t bitsize);
    ~TileRegionArray();

    void printInfoIndices();

    int16_t at(int x, int y, int z);
    void set(int x, int y, int z, int16_t val);
    void reset();

private:
    size_t bitsize = 16;
    Tiles1Bit* t1 = nullptr;
    Tiles2Bit* t2 = nullptr;
    Tiles4Bit* t4 = nullptr;
    Tiles8Bit* t8 = nullptr;
    Tiles16Bit* tX = nullptr;
};