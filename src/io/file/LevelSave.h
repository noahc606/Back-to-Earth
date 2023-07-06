#pragma once
#include "FileHandler.h"
#include "TileType.h"

class LevelSave
{
public:
    typedef std::vector<TileType> t_palette;

    LevelSave(FileHandler* parentFH);

    void trSave(FilePath path, long rX, long rY, long rZ, t_palette* palette, uint16_t tiles[32][32][32] );
protected:

private:
    FileHandler* fileHandler = nullptr;
    const static uint16_t ampSBits[];
    const static uint8_t pows2[];
};
