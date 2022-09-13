#pragma once
#include <map>
#include <tuple>
#include <vector>
#include "Texture.h"
#include "TileMap.h"
#include "TileRegion.h"
#include "TileType.h"

class TileIterator
{
public:
    TileIterator(TileMap* tileMap);
    virtual ~TileIterator();


    TileType nextTile();
    TileType prevTile();
    TileRegion* nextRegion();
    TileRegion* prevRegion();
    TileType firstTile();
    TileType lastTile();


    TileType peekTile();
    TileType peekTile(int offsetX, int offsetY, int offsetZ);
    TileType peekTrackedTile();
    TileType peekTrackedTile(int offsetX, int offsetY, int offsetZ);
    TileRegion* peekRegion();

    long getItrIndex();
    long getItrLength();
    long getItrSub(int c);
    long getItrPos(int c);
    long getItrReg(int c);

    long getBegSub(int c);
    long getEndSub(int c);
    long getBegPos(int c);
    long getEndPos(int c);
    long getBegReg(int c);
    long getEndReg(int c);

    long getTrackerSub(int c);
    long getTrackerPos(int c);
    long getTrackerReg(int c);

    bool reachedEnd();
    bool reachedBegin();
    bool invalidIndex();
    bool invalidBounds();


    int setBounds(long x1, long y1, long z1, long x2, long y2, long z2);
    int setBoundsByRXYZ(long rX1, long rY1, long rZ1, long rX2, long rY2, long rZ2);
    int setBoundsByRXYZ(long rX, long rY, long rZ);
    void setTrackerMode(int mode);
    void setTrackerSub(int sx, int sy, int sz);

    void logWarnings(bool value);

    void scanBounds();


    enum RegionTrackingModes {
        NONE,
        SINGLE,
        DOWN_1,
        ADJACENT,
        FULL,
    };

protected:

private:
    typedef TileMap::t_ll t_ll;

    /**/
    /**/
    void updateSubInfo();
    void updateSubInfo(bool beginning);
    void resetRegionPtrs();
    void updateRegionPtrs();
    void updateItrPos(int c);
    int testSelectionLoaded();

    //TileMap data structure being used
    TileMap* tileMap = nullptr;

    //Iterator info
    long itrIndex = 0;
    long itrLength = 0;

    //Region/Tisg tracking
    int trackingMode = NONE;
    TileRegion* nearbyTileRegions[3][3][3];

    bool boundsInvalid = true;

    bool warnings = true;

    //Once itrSubPos.(x|y|z) goes above 31 or below 0, itrRegion gets updated.
    long begSub[3], itrSub[3], endSub[3];
    t_ll begPos[3], itrPos[3], endPos[3];
    long begReg[3], itrReg[3], endReg[3];

    long trackerSub[3];
};
