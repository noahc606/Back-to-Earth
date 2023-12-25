#include "TileMap.h"
#include <cmath>
#include <math.h>
#include <sstream>
#include "DebugScreen.h"
#include "Log.h"
#include "Terrain.h"
#include "TileIterator.h"
#include "TileType.h"

void TileMap::init(SDLHandler* sh, FileHandler* fh, Planet* pt)
{
	sdlHandler = sh;
	fileHandler = fh;
	planet = pt;
}

void TileMap::destroy()
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Deleting TileMap.");

    //Delete elements in regionMap
    for( t_regionMap::iterator itr = regionMap.begin(); itr!=regionMap.end(); itr = regionMap.begin() ) {
        regionMap.erase(itr);
    }

    stopAllUpdates();
}

void TileMap::putInfo(std::stringstream& ss, int& tabs)
{
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
}

TileMap::t_regionMap* TileMap::getRegionMap() { return &regionMap; }
TileMap::t_updatesMap* TileMap::getUpdatesMap() { return &updatesMap; }
Planet* TileMap::getPlanet() { return planet; }

/*
    This function will work fine in small loops but keep in mind getRegSubPos() is somewhat expensive. For large or even 3D loops (like in TileMapScreen::draw()) use a TileIterator.

*/
TileType TileMap::getTile(int64_t x, int64_t y, int64_t z)
{
    TileRegion* tr = getRegByXYZ(x, y, z);
    getRegSubPos(x, y, z);

    if( tr!=nullptr )
        return tr->getTile(x, y, z);

    //If tr==null
    return TileType();
}

TileRegion* TileMap::getRegByXYZ(int64_t x, int64_t y, int64_t z)
{
    getRegRXYZ(x, y, z);
    return getRegByRXYZ(x, y, z);
}

TileRegion* TileMap::getRegByRXYZ(int64_t rX, int64_t rY, int64_t rZ)
{
    t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
    if ( itr!=regionMap.end() )
        return &itr->second;
    return nullptr;
}

TileMap::t_updates* TileMap::getRTUsByRXYz(int64_t rX, int64_t rY, int64_t z)
{
    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, z) );
    if( itr!=updatesMap.end() )
        return &(itr->second);
    return nullptr;
}


int64_t TileMap::getRegSubPos(int64_t c) { c %= 32; if( c<0 ) c+=32; return c; }
void TileMap::getRegSubPos(int64_t& x, int64_t& y, int64_t& z) { x = getRegSubPos(x); y = getRegSubPos(y); z = getRegSubPos(z); }

int64_t TileMap::getRegRXYZ(int64_t c) { c = floor(c/32.0); return c; }
void TileMap::getRegRXYZ(int64_t& x, int64_t& y, int64_t& z) { x = getRegRXYZ(x); y = getRegRXYZ(y); z = getRegRXYZ(z); }

int64_t TileMap::convRxyToLSRxy(int64_t rxOrY) { rxOrY = floor(rxOrY/16.0); return rxOrY; }
int64_t TileMap::convRzToLSRz(int64_t rz) { rz = floor(rz/4.0); return rz; }
void TileMap::convRxyzToLSRxyz(int64_t& rx, int64_t& ry, int64_t& rz) { rx = convRxyToLSRxy(rx); ry = convRxyToLSRxy(ry); rz = convRzToLSRz(rz); }

bool TileMap::collides( Box3D& b )
{
    TileIterator ti(this);

    int64_t xf = std::floor(b.c1.x);
    int64_t yf = std::floor(b.c1.y);
    int64_t zf = std::floor(b.c1.z);
    int64_t xc = std::ceil(b.c2.x);
    int64_t yc = std::ceil(b.c2.y);
    int64_t zc = std::ceil(b.c2.z);

    ti.setBounds( xf, yf, zf, xc, yc, zc );
    ti.setTrackerMode( ti.SINGLE );

    for( ; !ti.invalidIndex(); ti.nextRegion() ) {
        TileRegion* tr = ti.peekRegion();
        for(long x = ti.getBegSub(0); x<=ti.getEndSub(0); x++ ) {
            for(long y = ti.getBegSub(1); y<=ti.getEndSub(1); y++ ) {
                for(long z = ti.getBegSub(2); y<=ti.getEndSub(2); z++ ) {

					TileType tt = tr->getTile(x, y, z);
					
					if ( tt.isSolid() ) {
						std::stringstream ss; int tabs = 0;
						tt.putInfo(ss, tabs);
						std::cout << "Intersecting TileType @ (" << x << ", " << y << ", " << z << "):" << ss.str();
					}
                }
            }
        }


    }

    return true;
}

int TileMap::setTile(int64_t x, int64_t y, int64_t z, TileType tt)
{
    TileRegion* tr = getRegByXYZ(x, y, z);
    getRegSubPos(x, y, z);
    if( tr!=nullptr ) {
        tr->setTile(x, y, z, tt);
        return 0;
    }
    return -1;
}

int TileMap::loadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ)
{
    t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
	//If no region was found, continue.
    if( itr==regionMap.end() ) {
        Terrain terra;
        TileRegion tr;

		//Generate terrain
		tr.setRegTexState(tr.GENERATING);
		terra.populateRegion(tr, rX, rY, rZ);
		tr.setRegTexState(tr.FINISHED_GENERATING);
		
		//Place artificial tiles
		tr.load(fileHandler, "world1", rX, rY, rZ);
		
		regionMap.insert( std::make_pair(std::make_tuple(rX, rY, rZ), tr) );
        return 0;
    }
    return -1;
}

/**
 * 	Used for debug purposes. Loads the region specified by rX, rY, and rZ, except it loads the artificial tiles (from file) no matter what.
 */
int TileMap::forceLoadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ)
{
	std::stringstream ss;
	ss << "Forceloading region (" << rX << ", " << rY << ", " << rZ << ")";
	Log::log(ss.str());
	
	loadRegion(fileHandler, rX, rY, rZ);
	TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
	if( tr!=nullptr ) {
		tr->load(fileHandler, "world1", rX, rY, rZ);
		return 1;
	}
	return -1;
}

int TileMap::saveRegion(FileHandler* fileHandler, std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ)
{
	TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
	
	if( tr==nullptr ) 					{ return -99; }
	if( !tr->beenModifiedSinceLoad() ) 	{ return -1; }
	
	std::stringstream ss;
	ss << "Saving region (" << rX << ", " << rY << ", " << rZ << ") in save '" << saveGameName << "'";
	Log::debug(ss.str());
	
	tr->save(fileHandler, saveGameName, rX, rY, rZ, false);
	return 0;
}

int TileMap::unloadRegion(FileHandler* fileHandler, std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ)
{
    t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
    if( itr!=regionMap.end() ) {
        regionMap.erase(itr);
        return 0;
    }
    return -1;
}

int TileMap::addTileUpdate(int64_t x, int64_t y, int64_t z)
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

int TileMap::addTileUpdates(int64_t x0, int64_t y0, int64_t z0, int64_t x1, int64_t y1, int64_t z1)
{
    int result = 0;
    for( int ix = x0; ix<=x1; ix++ ) {
        for( int iy = y0; iy<=y1; iy++ ) {
            for( int iz = z0; iz<=z1; iz++ ) {
                result += addTileUpdate(ix, iy, iz);
            }
        }
    }

    return result;
}

/**
    Create a 3x3 of tile updates at the specified position
*/
int TileMap::addTileUpdates(int64_t x, int64_t y, int64_t z)
{
    return addTileUpdates(x-1, y-1, z, x+1, y+1, z);
}

int TileMap::addRegionUpdate(int64_t rX, int64_t rY, int64_t layer)
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

int TileMap::stopRegionUpdate(int64_t rX, int64_t rY, int64_t layer)
{
    t_updatesMap::iterator itr = updatesMap.find( std::make_tuple(rX, rY, layer) );
    if( itr!=updatesMap.end() ) {
        updatesMap.erase(itr);
        return 0;
    }
    return -1;
}

int TileMap::stopAllUpdates()
{
    int count = 0;
    //Delete elements in updatesMap
    for( t_updatesMap::iterator itr = updatesMap.begin(); itr!=updatesMap.end(); itr = updatesMap.begin() ) {
        updatesMap.erase(itr);
        count++;
    }

    return count;
}
