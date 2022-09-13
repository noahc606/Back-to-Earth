#pragma once
#include "Color.h"
#include "Texture.h"
#include "TileIterator.h"

class RegTexBuilder
{
public:
    RegTexBuilder(Texture* tex, TileIterator& ti, int dstX, int dstY, TileType ttfc, int dZ);
    virtual ~RegTexBuilder();

    static void info(std::stringstream& ss, int& tabs, TileIterator& ti, int dZ);

    /*
        bl = blocking
        co = corners
        si = sides
        sh = shadows
    */
    static void infoBl(TileIterator& ti, bool bl[8], int dZ);
    static void infoBl(TileIterator& ti, bool bl[8]);
    static void infoCoSi(bool bl[4], bool co[4], bool si[4]);
    static void infoSh(bool co[4], bool si[4], bool sh[4]);
    static void infoCoSiSh(bool bl[8], bool co[4], bool si[4], bool sh[4]);

protected:

private:

    void detailDepth0Tiles(Texture* tex, TileIterator& ti, int dstX, int dstY); //Depth 0
    void detailDepthPTiles(Texture* tex, TileIterator& ti, int dstX, int dstY, int dZ); //Depth positive (>0)

    void drawTypeA(Texture* tex, int dstX, int dstY, int srcX, int srcY, Color c, int dZ);
    void drawTypeA(Texture* tex, int dstX, int dstY, TileType tt, int dZ);
    void drawTypeA(Texture* tex, int dstX, int dstY, int srcX, int srcY);
    void drawOverlay(Texture* tex, int dstX, int dstY, int srcX, int srcY);
};
