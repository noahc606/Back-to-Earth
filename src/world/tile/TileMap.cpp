#include "TileMap.h"
#include <math.h>
#include <sstream>
#include "DebugScreen.h"
#include "Log.h"
#include "TileIterator.h"
#include "TileType.h"


void TileMap::init()
{

}

void TileMap::destroy()
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Deleting TileMap.");

    //Delete elements in regionMap
    for( t_regionMap::iterator itr = regionMap.begin(); itr!=regionMap.end(); itr = regionMap.begin() ) {
        regionMap.erase(itr);
    }

    //Delete elements in updatesMap
    for( t_updatesMap::iterator itr = updatesMap.begin(); itr!=updatesMap.end(); itr = updatesMap.begin() ) {
        updatesMap.erase(itr);
    }
}

void TileMap::info(std::stringstream& ss, int& tabs, t_ll p_mouseX, t_ll p_mouseY, t_ll p_selZ)
{
    //Info
    t_ll selX = p_mouseX; t_ll selY = p_mouseY; t_ll selZ = p_selZ;

    long rX = selX; long rY = selY; long rZ = selZ;
    TileMap::getRegRXYZ(rX, rY, rZ);

    long sx = selX; long sy = selY; long sz = selZ;
    TileMap::getRegSubPos(sx, sy, sz);

    //regionMap size
    DebugScreen::indentLine(ss, tabs);
    ss << "regionMap size=" << regionMap.size() << "; ";
    int rmCount = 0;
    t_regionMap::iterator itrRM = regionMap.begin();
    for( ; itrRM!= regionMap.end(); itrRM++ ) {
        rmCount++;
    }
    DebugScreen::newLine(ss);

    //regTexMap size
    //DebugScreen::indentLine(ss, tabs);
    //ss << "# of regStates!=-1=" << regStateMap.size() << "; ";
    //DebugScreen::newLine(ss);

    DebugScreen::indentLine(ss, tabs);
    ss << "updatesMap size (regions)=" << updatesMap.size() << "; ";
    int updateCount = 0;
    for( t_updatesMap::iterator itrUM = updatesMap.begin(); itrUM!= updatesMap.end(); itrUM++ ) {
        updateCount += itrUM->second.size();
    }
    ss << "updatesMap size (tiles)=" << updateCount << "; ";
    DebugScreen::newLine(ss);

    //Selected TileRegion
    TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
    if(tr!=nullptr) {
        std::stringstream selTR;
        selTR << "Selected TileRegion: (" << rX << ", " << rY << ", " << rZ << ")";
        DebugScreen::newGroup(ss, tabs, "Selected TileRegion");
            tr->info(ss, tabs, sx, sy, sz);
        DebugScreen::endGroup(tabs);
    }
}

TileMap::t_regionMap* TileMap::getRegionMap() { return &regionMap; }
TileMap::t_updatesMap* TileMap::getUpdatesMap() { return &updatesMap; }

/*
    This function will work fine in small loops but keep in mind getRegSubPos() is somewhat expensive. For large or even 3D loops (like in TileMapScreen::draw()) use a TileIterator.

*/
TileType TileMap::getTile(t_ll x, t_ll y, t_ll z)
{
    TileRegion* tr = getRegByXYZ(x, y, z);
    getRegSubPos(x, y, z);

    if( tr!=nullptr )
        return tr->getTile(x, y, z);

    //If tr==null
    return TileType();
}

TileRegion* TileMap::getRegByXYZ(t_ll x, t_ll y, t_ll z)
{
    getRegRXYZ(x, y, z);
    return getRegByRXYZ(x, y, z);
}

TileRegion* TileMap::getRegByRXYZ(long rX, long rY, long rZ)
{
    t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
    if ( itr!=regionMap.end() )
        return &itr->second;
    return nullptr;
}

TileMap::t_updates* TileMap::getRTUsByRXYz(long rX, long rY, long z)
{
    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, z) );
    if( itr!=updatesMap.end() )
        return &(itr->second);
    return nullptr;
}

void TileMap::getRegSubPos(t_ll& x, t_ll& y, t_ll& z) { x = getRegSubPos(x); y = getRegSubPos(y); z = getRegSubPos(z); }
void TileMap::getRegSubPos(long& x, long& y, long& z) { x = getRegSubPos(x); y = getRegSubPos(y); z = getRegSubPos(z); }
long TileMap::getRegSubPos(t_ll c) { c %= 32; if( c<0 ) c+=32; return c; }
void TileMap::getRegRXYZ(t_ll& x, t_ll& y, t_ll& z) { x = getRegRXYZ(x); y = getRegRXYZ(y); z = getRegRXYZ(z); }
void TileMap::getRegRXYZ(long& x, long& y, long& z) { x = getRegRXYZ(x); y = getRegRXYZ(y); z = getRegRXYZ(z); }
long TileMap::getRegRXYZ(t_ll c) { c = floor(c/32.0); return c; }

bool TileMap::collides( Box3D& b )
{
    TileIterator ti(this);

    t_ll xf = b.c1.x.floor();
    t_ll yf = b.c1.y.floor();
    t_ll zf = b.c1.z.floor();
    t_ll xc = b.c2.x.ceil();
    t_ll yc = b.c2.y.ceil();
    t_ll zc = b.c2.z.ceil();

    ti.setBounds( xf, yf, zf, xc, yc, zc );
    ti.setTrackerMode( ti.SINGLE );

    for( ; !ti.invalidIndex(); ti.nextRegion() ) {
        TileRegion* tr = ti.peekRegion();
        for(long x = ti.getBegSub(0); x<=ti.getEndSub(0); x++ ) {
            for(long y = ti.getBegSub(1); y<=ti.getEndSub(1); y++ ) {
                for(long z = ti.getBegSub(2); y<=ti.getEndSub(2); z++ ) {

                    TileType tt = tr->getTile(x, y, z);
                    std::stringstream ss; int tabs = 0;
                    tt.info(ss, tabs);
                    std::cout << "Intersecting TileType @ (" << x << ", " << y << ", " << z << "):" << ss.str();

                }
            }
        }


    }

    return true;
}

int TileMap::setTile(t_ll x, t_ll y, t_ll z, TileType tt)
{
    TileRegion* tr = getRegByXYZ(x, y, z);
    getRegSubPos(x, y, z);
    if( tr!=nullptr ) {
        tr->setTile(x, y, z, tt);
        return 0;
    }
    return -1;
}


int TileMap::loadRegion(long x, long y, long z)
{
    t_regionMap::iterator itr = regionMap.find( std::make_tuple(x, y, z) );
    if( itr==regionMap.end() ) {
        //TileRegion* tr = new TileRegion();
        //TileRegion* tr = noise.getRegion(100, x, y, z);
        //TileRegion* tr = noise.getRegion(z);
        TileRegion tr = noise.getRegion(z);
        regionMap.insert( std::make_pair(std::make_tuple(x, y, z), tr) );
        return 0;
    }
    return -1;
}

int TileMap::addTileUpdate(t_ll x, t_ll y, t_ll z)
{
    int rX = getRegRXYZ(x);
    int rY = getRegRXYZ(y);

    int sx = getRegSubPos(x);
    int sy = getRegSubPos(y);

    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, z) );
    if( itr!=updatesMap.end() ) {
        //Add the update inside of an existing update region
        t_updates* upd = &(itr->second);
        upd->insert( std::make_pair(sx, sy) );
        return 0;
    } else {
        //Add the update region with the update.
        t_updates upd;
        upd.insert( std::make_pair(sx, sy) );
        updatesMap.insert( std::make_pair(std::make_tuple(rX, rY, z), upd ) );
        return 1;
    }
}

int TileMap::addTileUpdates(t_ll x, t_ll y, t_ll z)
{
    int result = 0;
    for( int ix = x-1; ix<=x+1; ix++ ) {
        for( int iy = y-1; iy<=y+1; iy++ ) {
            result += addTileUpdate(ix, iy, z);
        }
    }

    return result;
}

int TileMap::addRegionUpdate(long rX, long rY, t_ll layer)
{
    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, layer) );

    if( itr!=updatesMap.end() ) {
        //Add the updates inside of an existing map element of 'updatesMap'.
        t_updates* upd = &(itr->second);
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                upd->insert( std::make_pair(sx, sy) );
            }
        }
        return 0;
    } else {
        //Add a new map element with the updates.
        t_updates upd;
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                upd.insert( std::make_pair(sx, sy) );
            }
        }
        updatesMap.insert( std::make_pair(std::make_tuple(rX, rY, layer), upd ) );
        return 1;
    }
}

int TileMap::stopRegionUpdate(long rX, long rY, t_ll layer)
{
    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, layer) );
    if( itr!=updatesMap.end() ) {
        updatesMap.erase(itr);
        return 0;
    }
    return -1;
}
