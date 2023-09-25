#pragma once
#include "DataStream.h"
#include "FileHandler.h"
#include "TileType.h"

class LevelSave
{
public:
    typedef std::vector<TileType> t_palette;
    typedef std::pair<t_palette, int16_t[1024]> t_saveArea;
    /**/
    LevelSave();
    void init(FileHandler* parentFH);
    /**/
    static int getPaletteSizeBucket(int size);
    /**/
    void trSave(FilePath path, long rX, long rY, long rZ, t_palette* palette, int16_t tiles[32][32][32] );
protected:

private:
    FileHandler* fileHandler = nullptr;



    const static uint16_t ampSBits[];
    const static uint8_t pows2[];
};
