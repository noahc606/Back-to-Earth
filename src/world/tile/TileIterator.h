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

    int64_t getItrIndex();
    int64_t getItrLength();

    int64_t getItrSub(int c); int64_t gis(int c);
    int64_t getItrPos(int c); int64_t gip(int c);
    int64_t getItrReg(int c); int64_t gir(int c);
    int64_t getBegSub(int c); int64_t gbs(int c);
    int64_t getBegPos(int c); int64_t gbp(int c);
    int64_t getBegReg(int c); int64_t gbr(int c);
    int64_t getEndSub(int c); int64_t ges(int c);
    int64_t getEndPos(int c); int64_t gep(int c);
    int64_t getEndReg(int c); int64_t ger(int c);
    int64_t getTrackerSub(int c); int64_t gts(int c);
    int64_t getTrackerPos(int c); int64_t gtp(int c);
    int64_t getTrackerReg(int c); int64_t gtr(int c);

    bool atEnd();
    bool atBeginning();
    bool invalidIndex();
    bool invalidBounds();

    int setBounds(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2);
    int setBoundsByRXYZ(int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2);
    int setBoundsByRXYZ(int64_t rX, int64_t rY, int64_t rZ);
    void setTrackerMode(int mode);
    void setTrackerSub(int64_t sx, int64_t sy, int64_t sz);

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
    int64_t itrIndex = 0;
    int64_t itrLength = 0;

    //Region/Tisg tracking
    int trackingMode = NONE;
    TileRegion* nearbyTileRegions[3][3][3];

    bool boundsInvalid = true;

    bool warnings = true;

    //Once itrSubPos.(x|y|z) goes above 31 or below 0, itrRegion gets updated.
    int64_t begSub[3], itrSub[3], endSub[3];
    int64_t begPos[3], itrPos[3], endPos[3];
    int64_t begReg[3], itrReg[3], endReg[3];

    int64_t trackerSub[3];
};
