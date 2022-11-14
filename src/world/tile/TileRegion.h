#pragma once
#include <vector>
#include <string>
#include "Camera.h"
#include "TileType.h"

class TileRegion
{
public:
    typedef std::vector<TileType> t_palette;

    /**/
    TileRegion( );
    virtual ~TileRegion();

    /**/
    void info(std::stringstream& ss, int& tabs, int subX, int subY, int subZ);
    int addToPalette( TileType tile, t_palette& pal );
    int addToPalette( TileType tile );
    int addToPaletteFast( TileType tile );

    /**/
    //Palette Length
    //Tiles
    TileType getTile     ( int x, int y, int z );
    TileType getTileSafe ( int x, int y, int z );
    //Region State
    int getRegTexState();

    /**/
    //Tiles
    void setTile         ( int x, int y, int z,                       TileType tile );
    void setTile         ( int x, int y, int z,                       int16_t paletteIndex );
    void setTiles        ( int x1,int y1,int z1,int x2,int y2,int z2, TileType tile );
    void setTiles        ( int x1,int y1,int z1,int x2,int y2,int z2, int16_t paletteIndex );
    //Region state
    void setRegTexState(int p_rts);
    void resetRegTexState();

    void compress();
    void queueUnload();
    void save( SDLHandler* sh, FileHandler* fh, bool compress );

    enum RegTexState {
        NONE = 0,
        UNGENERATED=100,
        GENERATING=101,
        FINISHED_GENERATING=102,
        SHOULD_UPDATE = 103,
        UPDATED = 104,
    };

protected:

private:
    t_palette palette;
    int16_t tiles[32][32][32];

    int regTexState = UNGENERATED;
};
