#include "TileIterator.h"
#include <iostream>
#include <nch/cpp-utils/io/Log.h>
#include <sstream>
#include <string.h>
#include <SDL2/SDL.h>

TileIterator::TileIterator(std::map<Defs::t_tripleI64, TileRegion>* regMap)
{
    TileIterator::regMap = regMap;

    logWarnings(false);
    resetRegionPtrs();
    setTrackerSub(0, 0, 0);
    setTrackerMode(FULL);
}

TileIterator::TileIterator(TileMap* tileMap): TileIterator(tileMap->getRegionMap()){}

TileIterator::~TileIterator(){}

/**
    Advances iterator by 1 tile.
    The iterator will go through all selected tiles in a region, then move on to the next region, and so on.
    If the end has been passed the iterator will return a null tile.
*/
TileType TileIterator::nextTile()
{
    itrIndex++;
    if( invalidIndex() ) {
        itrIndex = itrLength;
        return TileType();
    }

    itrSub[0]++;

    if( itrSub[0]>endSub[0] ) {
        itrSub[0] = begSub[0];
        itrSub[1]++;
        updateItrPos(0);

        if( itrSub[1]>endSub[1] ) {
            itrSub[1] = begSub[1];
            itrSub[2]++;
            updateItrPos(1);

            if( itrSub[2]>endSub[2] ) {
                itrSub[2] = begSub[2];
                nextRegion();
                updateItrPos(2);
            }
        }
    }

    return peekTile();
}

TileType TileIterator::prevTile()
{
    itrIndex--;
    if( invalidIndex() ) {
        itrIndex = -1;
        return TileType();
    }

    itrSub[0]--;

    if( itrSub[0]<begSub[0] ) {
        itrSub[0] = endSub[0];
        itrSub[1]--;
        updateItrPos(0);

        if( itrSub[1]<begSub[1] ) {
            itrSub[1] = endSub[1];
            itrSub[2]--;
            updateItrPos(1);

            if( itrSub[2]<begSub[2] ) {
                itrSub[2] = endSub[2];
                nextRegion();
                updateItrPos(2);
            }
        }
    }

    return peekTile();
}

TileRegion* TileIterator::nextRegion()
{
    if( itrReg[0]!=endReg[0] || itrReg[1]!=endReg[1] || itrReg[2]!=endReg[2] ) {
        itrReg[0]++;

        if( itrReg[0]>endReg[0] ) {
            itrReg[0] = begReg[0];
            itrReg[1]++;

            if( itrReg[1]>endReg[1] ) {
                itrReg[1] = begReg[1];
                itrReg[2]++;

                if( itrReg[2]>endReg[2] ) {
                    itrReg[0] = endReg[0];
                    itrReg[1] = endReg[1];
                    itrReg[2] = endReg[2];

                    itrIndex = itrLength;
                    return nullptr;
                }
            }
        }
    } else {
        itrIndex = itrLength;
        return nullptr;
    }

    updateSubInfo(true);
    updateRegionPtrs();
    return nearbyTileRegions[1][1][1];
}

TileRegion* TileIterator::prevRegion()
{
    if( itrReg[0]!=begReg[0] || itrReg[1]!=begReg[1] || itrReg[2]!=begReg[2] ) {
        itrReg[0]--;

        if( itrReg[0]<begReg[0] ) {
            itrReg[0] = endReg[0];
            itrReg[1]--;

            if( itrReg[1]<begReg[1] ) {
                itrReg[1] = endReg[1];
                itrReg[2]--;

                if( itrReg[2]<begReg[2] ) {
                    itrReg[0] = begReg[0];
                    itrReg[1] = begReg[1];
                    itrReg[2] = begReg[2];

                    itrIndex = -1;
                    return nullptr;
                }
            }
        }
    } else {
        itrIndex = -1;
        return nullptr;
    }

    updateSubInfo(true);
    updateRegionPtrs();
    return nearbyTileRegions[1][1][1];
}

TileType TileIterator::firstTile()
{
    //Set itrReg to begReg, set itrSubPos to begPos, and truncate itrSubPos
    memcpy( itrReg, begReg, sizeof itrReg );
    itrSub[0] = begPos[0]; itrSub[1] = begPos[1]; itrSub[2] = begPos[2];
    TileMap::getRegSubPos( itrSub[0], itrSub[1], itrSub[2] );

    //updateSubInfo and region pointers
    itrIndex = 0;
    updateSubInfo(true);
    updateItrPos(0);
    updateItrPos(1);
    updateItrPos(2);
    updateRegionPtrs();

    //Return current element
    return peekTile();
}

TileType TileIterator::lastTile()
{
    //Set itrReg to endReg, set itrSubPos to endPos, and truncate itrSubPos
    memcpy( itrReg, endReg, sizeof itrReg );
    itrSub[0] = endPos[0]; itrSub[1] = endPos[1]; itrSub[2] = endPos[2];
    TileMap::getRegSubPos( itrSub[0], itrSub[1], itrSub[2] );

    //updateSubInfo and region pointers
    itrIndex = itrLength-1;
    updateSubInfo(false);
    updateItrPos(0);
    updateItrPos(1);
    updateItrPos(2);
    updateRegionPtrs();

    //Return current element
    return peekTile();
}

TileType TileIterator::peekTile()
{
    if( nearbyTileRegions[1][1][1]!=nullptr )
        return nearbyTileRegions[1][1][1]->getTile(itrSub[0], itrSub[1], itrSub[2]);
    return TileType();
}

TileType TileIterator::peekTile(int offsetX, int offsetY, int offsetZ)
{
    int posInReg[3] = { (int)itrSub[0]+offsetX, (int)itrSub[1]+offsetY, (int)itrSub[2]+offsetZ };
    int regOffset[3] = {0, 0, 0};

    for( int i = 0; i<3; i++ ) {
        if( posInReg[i]<0 ) {
            posInReg[i] += 32;
            regOffset[i]--;
        } else
        if( posInReg[i]>31 ) {
            posInReg[i] -= 32;
            regOffset[i]++;
        }
    }

    TileRegion* tr = nearbyTileRegions[1+regOffset[0]][1+regOffset[1]][1+regOffset[2]];

    if(tr!=nullptr) {
        return tr->getTile(posInReg[0], posInReg[1], posInReg[2]);
    } else {
        return TileType();
    }
}

TileType TileIterator::peekTrackedTile()
{
    if( nearbyTileRegions[1][1][1]!=nullptr )
        return nearbyTileRegions[1][1][1]->getTile(trackerSub[0], trackerSub[1], trackerSub[2]);
    return TileType();
}

TileType TileIterator::peekTrackedTile(int offsetX, int offsetY, int offsetZ)
{
    int posInReg[3] = { (int)trackerSub[0]+offsetX, (int)trackerSub[1]+offsetY, (int)trackerSub[2]+offsetZ };

    int regOffset[3] = {0, 0, 0};

    for( int i = 0; i<3; i++ ) {
        if( posInReg[i]<0 ) {
            posInReg[i] += 32;
            regOffset[i]--;
        } else
        if( posInReg[i]>31 ) {
            posInReg[i] -= 32;
            regOffset[i]++;
        }
    }

    TileRegion* tr = nearbyTileRegions[1+regOffset[0]][1+regOffset[1]][1+regOffset[2]];

    if(tr!=nullptr) {

        return tr->getTile(posInReg[0], posInReg[1], posInReg[2]);
    } else {
        return TileType();
    }
}

TileRegion* TileIterator::peekRegion() { return nearbyTileRegions[1][1][1]; }

int64_t TileIterator::getItrIndex() { return itrIndex; }
int64_t TileIterator::getItrLength() { return itrLength; }

//Get interator (Pos)ition, (Sub)position, or (Reg)ion coordinate.
int64_t TileIterator::getItrPos(int c) { return itrReg[c]*32+itrSub[c]; }
int64_t TileIterator::gip(int c) { return getItrPos(c); }
int64_t TileIterator::getItrSub(int c) { return itrSub[c]; }
int64_t TileIterator::gis(int c) { return getItrSub(c); }
int64_t TileIterator::getItrReg(int c) { return itrReg[c]; }
int64_t TileIterator::gir(int c) { return getItrReg(c); }

//Get beginning (Pos)ition, (Sub)position, or (Reg)ion coordinate.
int64_t TileIterator::getBegSub(int c) { return begSub[c]; }
int64_t TileIterator::gbs(int c) { return getBegSub(c); }
int64_t TileIterator::getBegPos(int c) { return begPos[c]; }
int64_t TileIterator::gbp(int c) { return getBegPos(c); }
int64_t TileIterator::getBegReg(int c) { return begReg[c]; }
int64_t TileIterator::gbr(int c) { return getBegReg(c); }

//Get ending (Pos)ition, (Sub)position, or (Reg)ion coordinate.
int64_t TileIterator::getEndSub(int c) { return endSub[c]; }
int64_t TileIterator::ges(int c) { return getEndSub(c); }
int64_t TileIterator::getEndPos(int c) { return endPos[c]; }
int64_t TileIterator::gep(int c) { return getEndPos(c); }
int64_t TileIterator::getEndReg(int c) { return endReg[c]; }
int64_t TileIterator::ger(int c) { return getEndReg(c); }

//Get tracker (Pos)ition, (Sub)position, or (Reg)ion coordinate.
int64_t TileIterator::getTrackerSub(int c) { return trackerSub[c]; }
int64_t TileIterator::gts(int c) { return getTrackerSub(c); }
int64_t TileIterator::getTrackerPos(int c) { return itrReg[c]*32+trackerSub[c]; }
int64_t TileIterator::gtp(int c) { return getTrackerPos(c); }
int64_t TileIterator::getTrackerReg(int c) { return itrReg[c]; }
int64_t TileIterator::gtr(int c) { return getTrackerReg(c); }

bool TileIterator::atEnd() { return itrIndex>=itrLength-1; }
bool TileIterator::atBeginning() { return itrIndex<=0; }
bool TileIterator::invalidIndex() { return itrIndex>=itrLength || itrIndex<=-1; }
bool TileIterator::invalidBounds() { return boundsInvalid; }

/**
    Returns 0 if all regions selected are loaded. If not, return -1.
*/
int TileIterator::setBounds( int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2 )
{
    //Get begPos and endPos
    //Make sure x1,y1,&z1 are less than x2,y2,&z2 respectively
    begPos[0] = x1; endPos[0] = x2;
    if( endPos[0]<begPos[0] ) { begPos[0] = x2; endPos[0] = x1; }
    begPos[1] = y1; endPos[1] = y2;
    if( endPos[1]<begPos[1] ) { begPos[1] = y2; endPos[1] = y1; }
    begPos[2] = z1; endPos[2] = z2;
    if( endPos[2]<begPos[2] ) { begPos[2] = z2; endPos[2] = z1; }

    //get begReg and endReg
    begReg[0] = begPos[0]; begReg[1] = begPos[1]; begReg[2] = begPos[2];
    TileMap::getRegRXYZ(begReg[0], begReg[1], begReg[2]);

    endReg[0] = endPos[0]; endReg[1] = endPos[1]; endReg[2] = endPos[2];
    TileMap::getRegRXYZ(endReg[0], endReg[1], endReg[2]);

    //Update itrLength
    itrLength = (endPos[0]-begPos[0]+1)*(endPos[1]-begPos[1]+1)*(endPos[2]-begPos[2]+1);

    //Go to firstTile
    firstTile();

    //Test if all regions from begReg to endReg are loaded or not.
    return testSelectionLoaded();
}

int TileIterator::setBoundsByRXYZ( int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2 )
{
    //Get begReg and endReg
    begReg[0] = rX1; endReg[0] = rX2;
    if( endReg[0]<begReg[0] ) { begReg[0] = rX2; endReg[0] = rX1; }
    begReg[1] = rY1; endReg[1] = rY2;
    if( endReg[1]<begReg[1] ) { begReg[1] = rY2; endReg[1] = rY1; }
    begReg[2] = rZ1; endReg[2] = rZ2;
    if( endReg[2]<begReg[2] ) { begReg[2] = rZ2; endReg[2] = rZ1; }

    //Get begPos and endPos
    for( int i = 0; i<3; i++ ) {
        begPos[i] = begReg[i]*32;
        endPos[i] = (endReg[i]+1)*32-1;
    }

    //Update itrLength
    itrLength = (endPos[0]-begPos[0]+1)*(endPos[1]-begPos[1]+1)*(endPos[2]-begPos[2]+1);

    //Go to firstTile
    firstTile();

    //Test if all regions from begReg to endReg are loaded or not.
    return testSelectionLoaded();
}

int TileIterator::setBoundsByRXYZ(int64_t rX, int64_t rY, int64_t rZ)
{
    return setBoundsByRXYZ(rX, rY, rZ, rX, rY, rZ);
}

void TileIterator::setTrackerMode(int mode)
{
    trackingMode = RegionTrackingModes::NONE;

    switch(mode)
    {
        case RegionTrackingModes::NONE:
        case RegionTrackingModes::SINGLE:
        case RegionTrackingModes::DOWN_1:
        case RegionTrackingModes::ADJACENT:
        case RegionTrackingModes::FULL:
            trackingMode = mode;
            updateRegionPtrs();
        break;
    }
}

void TileIterator::setTrackerSub(int64_t sx, int64_t sy, int64_t sz)
{
    trackerSub[0] = sx;
    trackerSub[1] = sy;
    trackerSub[2] = sz;
}

void TileIterator::logWarnings(bool value) { warnings = value; }

void TileIterator::scanBounds()
{
    //Variables
    int numScans = 1;
    int start = 0; int end = 0;
    int num = 0; int numNull = 0; int total = 0;
    std::tuple<int, int, int> textureXYZ = std::make_tuple(2, 0, 0);

    //Message
    nch::Log::debug("Scanning bounds...");

    //Start time
    start = SDL_GetTicks();

    for(int i = 0; i<numScans; i++) {

        while( !invalidIndex() ) {
            TileType tt = peekTile();
            if( tt.getTextureXYZ()==textureXYZ ) num++;
            if(tt.isNull()) numNull++;

            total++;
            nextTile();
        }
    }
    end = SDL_GetTicks();
    std::cout << "#: " << num << " out of " << total << ". Found " << numNull << " null tiles.\n";
    std::cout << "Time Elapsed: " << (end-start) << "ms\n";
}

void TileIterator::updateSubInfo() { updateSubInfo(true); }

void TileIterator::updateSubInfo(bool beginning)
{
    for(int i = 0; i<3; i++ ) {
        //Update begSub
        begSub[i] = 0;
        if( itrReg[i]==begReg[i] ) {
            begSub[i] = TileMap::getRegSubPos(begPos[i]);
        }

        //Update endSub
        endSub[i] = 31;
        if( itrReg[i]==endReg[i] ) {
            endSub[i] = TileMap::getRegSubPos(endPos[i]);
        }

        //Set itr sub position to beginning or ending sub position depending on parameter
        if(beginning) {
            itrSub[i] = begSub[i];
        } else {
            itrSub[i] = endSub[i];
        }
    }
}

void TileIterator::resetRegionPtrs()
{
    for( int x = 0; x<3; x++ )
        for( int y = 0; y<3; y++ )
            for( int z = 0; z<3; z++ )
                nearbyTileRegions[x][y][z] = nullptr;
}

void TileIterator::updateRegionPtrs()
{
    resetRegionPtrs();

    switch(trackingMode)
    {
        case RegionTrackingModes::SINGLE: {
            nearbyTileRegions[1][1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]);
        } break;
        case RegionTrackingModes::DOWN_1: {
            nearbyTileRegions[1][1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]);
            nearbyTileRegions[1][1][0] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]-1);
        } break;
        case RegionTrackingModes::ADJACENT: {
            nearbyTileRegions[1][1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]);
            for( int x = -1; x<=1; x += 2 )
                nearbyTileRegions[x+1][1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0]+x, itrReg[1], itrReg[2]);
            for( int y = -1; y<=1; y += 2 )
                nearbyTileRegions[1][y+1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1]+y, itrReg[2]);
            for( int z = -1; z<=1; z += 2 )
                nearbyTileRegions[1][1][z+1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]+z);
        } break;
        case RegionTrackingModes::FULL: {
            for( int x = -1; x<2; x++ ) {
                for( int y = -1; y<2; y++ ) {
                    for( int z = -1; z<2; z++ ) {
                        nearbyTileRegions[x+1][y+1][z+1] = TileMap::getRegByRXYZ(regMap, itrReg[0]+x, itrReg[1]+y, itrReg[2]+z);
                    }
                }
            }
        } break;
        default: {
            nearbyTileRegions[1][1][1] = TileMap::getRegByRXYZ(regMap, itrReg[0], itrReg[1], itrReg[2]);
        } break;
    }
}

void TileIterator::updateItrPos(int c)
{
    itrPos[c] = itrReg[c]*32+itrSub[c];
}

int TileIterator::testSelectionLoaded()
{
    boundsInvalid = false;

    for( int x = begReg[0]; x<=endReg[0]; x++ ) {
        for( int y = begReg[1]; y<=endReg[1]; y++ ) {
            for( int z = begReg[2]; z<=endReg[2]; z++ ) {
                if( TileMap::getRegByRXYZ(regMap, x, y, z)==nullptr ) {

                    boundsInvalid = true;

                    if(warnings) {
                        nch::Log::warn(__PRETTY_FUNCTION__, "TileIterator selected area is partially unloaded");

                        std::stringstream ss2;
                        ss2 << "Bounds are from RXYZ(" << begReg[0] << ", " << begReg[1] << ", " << begReg[2] << ") to RXYZ(" << endReg[0] << ", " << endReg[1] << ", " << endReg[2] << ")";
                        nch::Log::warn(__PRETTY_FUNCTION__, ss2.str());
                    }

                    return -1;
                }
            }
        }
    }
    return 0;
}
