#include "TileMap.h"
#include <math.h>
#include <sstream>
#include "DebugScreen.h"
#include "Log.h"
#include "Terrain.h"
#include "TileIterator.h"
#include "TileType.h"

void TileMap::init(SDLHandler* sh, FileHandler* fh, Planet* pt, std::string wdn, int64_t worldSeed)
{
	sdlHandler = sh;
	fileHandler = fh;
	planet = pt;
	worldDirName = wdn;
	TileMap::worldSeed = worldSeed;
}

void TileMap::destroy()
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Deleting TileMap.");

    //Delete elements in regionMap
    for( t_regionMap::iterator itr = regionMap.begin(); itr!=regionMap.end(); itr = regionMap.begin() ) {
        regionMap.erase(itr);
    }
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
}

TileMap::t_regionMap* TileMap::getRegionMap() { return &regionMap; }
Planet* TileMap::getPlanet() { return planet; }
std::string TileMap::getWorldDirName() { return worldDirName; }

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
TileType TileMap::getTileByCsXYZ(Camera* cam, int64_t csX, int64_t csY, int64_t csZ)
{
	switch(cam->getAxis()) {
		case Camera::X: return getTile(csZ, csX, csY); break;
		case Camera::Y: return getTile(csX, csZ, csY); break;
	}
	return getTile(csX, csY, csZ);
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

/*
	Get region given a canvas region location.
	Canvas's plane is perpendicular to camera's depth axis (canvas plane matches up with screen).
	csRZ represents traveling forward and backward in 3D space relative to the 2D canvas which fills up the screen.
*/
TileRegion* TileMap::getRegByCsRXYZ(Camera* cam, int64_t csRX, int64_t csRY, int64_t csRZ)
{
	switch(cam->getAxis()) {
		case Camera::X: return getRegByRXYZ(csRZ, csRX, csRY); break;
		case Camera::Y: return getRegByRXYZ(csRX, csRZ, csRY); break;
	}
	return getRegByRXYZ(csRX, csRY, csRZ);
}

int64_t TileMap::getRegSubPos(int64_t c) { c %= 32; if( c<0 ) c+=32; return c; }
void TileMap::getRegSubPos(int64_t& x, int64_t& y, int64_t& z) { x = getRegSubPos(x); y = getRegSubPos(y); z = getRegSubPos(z); }

int64_t TileMap::getRegRXYZ(int64_t c) { c = floor(c/32.0); return c; }
void TileMap::getRegRXYZ(int64_t& x, int64_t& y, int64_t& z) { x = getRegRXYZ(x); y = getRegRXYZ(y); z = getRegRXYZ(z); }

int64_t TileMap::convRxyToLSRxy(int64_t rxOrY) { rxOrY = floor(rxOrY/16.0); return rxOrY; }
int64_t TileMap::convRzToLSRz(int64_t rz) { rz = floor(rz/4.0); return rz; }
void TileMap::convRxyzToLSRxyz(int64_t& rx, int64_t& ry, int64_t& rz) { rx = convRxyToLSRxy(rx); ry = convRxyToLSRxy(ry); rz = convRzToLSRz(rz); }

/*
	Do any tiles in the TileMap collide with Box 'b'?
*/
bool TileMap::collides(Box3D b, int64_t& cx, int64_t& cy, int64_t& cz)
{
    TileIterator ti(this);

    int64_t xf = std::floor(b.c1.x);
    int64_t yf = std::floor(b.c1.y);
    int64_t zf = std::floor(b.c1.z);
    int64_t xc = std::floor(b.c2.x);
    int64_t yc = std::floor(b.c2.y);
    int64_t zc = std::floor(b.c2.z);

    ti.setBounds( xf, yf, zf, xc, yc, zc );
    ti.setTrackerMode( ti.SINGLE );

	//Iterate through all relevant tile positions (maximum of 2*2*3=12)
    for( ; !ti.invalidIndex(); ti.nextRegion() ) {	
        TileRegion* tr = ti.peekRegion();
		if(tr!=nullptr) {
			for(int64_t sx = ti.getBegSub(0); sx<=ti.getEndSub(0); sx++ )
			for(int64_t sy = ti.getBegSub(1); sy<=ti.getEndSub(1); sy++ )
			for(int64_t sz = ti.getBegSub(2); sz<=ti.getEndSub(2); sz++ ) {
				//If we are looking at a solid tile...
				if(tr->getTile(sx, sy, sz).isSolid() ) {
					int64_t x = ti.getItrReg(0)*32+sx;
					int64_t y = ti.getItrReg(1)*32+sy;
					int64_t z = ti.getItrReg(2)*32+sz;
					//Build tileBox and check for collision against it
					Box3D tileBox(x, y, z, x+1, y+1, z+1);
					if(b.intersects(tileBox)) {
						cx = x; cy = y; cz = z;
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

bool TileMap::collides(Box3D b)
{
	int64_t x, y, z = 0;
	return collides(b, x, y, z);
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

int TileMap::setTileByCsXYZ(Camera* cam, int64_t csX, int64_t csY, int64_t csZ, TileType tt)
{
	switch(cam->getAxis()) {
		case Camera::X: return setTile(csZ, csX, csY, tt); break;
		case Camera::Y: return setTile(csX, csZ, csY, tt); break;
	}
	return setTile(csX, csY, csZ, tt);
}

/*
	Given a nonexistent region (rX, rY, rZ), create the region, generate its terrain, and load its artificial tiles from file.
	Returns: 0 if successful, -1 if region already existed.
*/
int TileMap::loadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ)
{
	//Try to find the region (rX, rY, rZ).
	t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
	//If no region was found, create the region and process it.
	if( itr==regionMap.end() ) {
		//Create Terrain and TileRegion objects
		Terrain terra(worldSeed);
		TileRegion tr;
		
		//Populate region's terrain
		tr.setRegTexState(tr.GENERATING);
		terra.populateRegion(tr, rX, rY, rZ);
		tr.setRegTexState(tr.FINISHED_GENERATING);
		
		//Place region's artificial tiles
		tr.load(fileHandler, worldDirName, rX, rY, rZ);
		
		//Insert the region into the regionMap.
		regionMap.insert( std::make_pair(std::make_tuple(rX, rY, rZ), tr) );
		return 0;
	}
	return -1;
}

int TileMap::loadRegionByCsRXYZ(FileHandler* fileHandler, int camAxis, int64_t csRX, int64_t csRY, int64_t csRZ)
{
	switch(camAxis) {
		case Camera::X: return loadRegion(fileHandler, csRZ, csRX, csRY); break;
		case Camera::Y: return loadRegion(fileHandler, csRX, csRZ, csRY); break;
	}
	return loadRegion(fileHandler, csRX, csRY, csRZ);
}

/*
	Load a rectangular prism of regions.
	Warning: will cause more lag at once compared to a single loadRegion().
	Returns: -x, where x is the number of regions that couldn't be loaded (because they already existed there)
*/
int TileMap::loadRegions(FileHandler* fileHandler, int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2)
{
	//Return result
	int res = 0;

	//Switch r[?]1 and r[?]2 if necessary (r[?]1 should be <= r[?]2)
	if(rX2<rX1) { int64_t temp = rX1; rX1 = rX2; rX2 = temp; }
	if(rY2<rY1) { int64_t temp = rY1; rY1 = rY2; rY2 = temp; }
	if(rZ2<rZ1) { int64_t temp = rZ1; rZ1 = rZ2; rZ2 = temp; }

	//Load all regions within the rectangular prism using 3D for loop
	for(int iRX = rX1; iRX<=rX2; iRX++) {
		for(int iRY = rY1; iRY<=rY2; iRY++) {
			for(int iRZ = rZ1; iRZ<=rZ2; iRZ++) {
				res += loadRegion(fileHandler, iRX, iRY, iRZ);
			}
		}
	}

	//Return result
	return res;
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
		tr->load(fileHandler, worldDirName, rX, rY, rZ);
		return 1;
	}
	return -1;
}

int TileMap::saveRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ)
{
	TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
	
	if( tr==nullptr ) 					{ return -99; }
	if( !tr->beenModifiedSinceLoad() ) 	{ return -1; }
	
	std::stringstream ss;
	ss << "Saving region (" << rX << ", " << rY << ", " << rZ << ") in save '" << worldDirName << "'";
	Log::debug(ss.str());
	
	tr->save(fileHandler, worldDirName, rX, rY, rZ, false);
	return 0;
}

int TileMap::unloadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ)
{
	t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
	if( itr!=regionMap.end() ) {
		regionMap.erase(itr);
		return 0;
	}
	return -1;
}