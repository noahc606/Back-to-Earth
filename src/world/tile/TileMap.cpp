#include "TileMap.h"
#include <math.h>
#include <nch/cpp-utils/log.h>
#include <sstream>
#include "DebugScreen.h"
#include "Terrain.h"
#include "TileIterator.h"
#include "Tile.h"
#include <nch/sdl-utils/timer.h>

using namespace nch;

void TileMap::init(SDLHandler* sh, Planet* pt, StructureMap* struMap, NoiseMap* nMap, TileDict* td, std::string saveGameName)
{
	sdlHandler = sh;
	planet = pt;

	TileMap::tileDict = td;
	TileMap::struMap = struMap;
	TileMap::nMap = nMap;

	TileMap::saveGameName = saveGameName;
}

void TileMap::destroy()
{
    nch::Log::log("Destroying TileMap.");

	//Build a list of all tile region locations
	std::vector<std::tuple<uint64_t, uint64_t, uint64_t>> regs;
	TileMap::t_regionMap::iterator itrRM;
	
	if( this==nullptr || getRegionMap()==nullptr ) {
		return;
	}
	for( itrRM = getRegionMap()->begin(); itrRM!=getRegionMap()->end(); itrRM++ ) {
		int rX = std::get<0>(itrRM->first);
		int rY = std::get<1>(itrRM->first);
		int rZ = std::get<2>(itrRM->first);
		regs.push_back( std::make_tuple(rX, rY, rZ) );
	}
	
	//Remove and unload all known tile regions
	for( int i = 0; i<regs.size(); i++ ) {
		auto thisReg = regs.at(i);
		uint64_t rX = std::get<0>(thisReg);
		uint64_t rY = std::get<1>(thisReg);
		uint64_t rZ = std::get<2>(thisReg);
		
		TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
		saveRegion(rX, rY, rZ);
		unloadRegion(rX, rY, rZ);
	}

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
    ss << "baseTerrainMap size=" << nMap->getBaseTerrainMap()->size() << "; ";
    int rmCount = 0;
    t_regionMap::iterator itrRM = regionMap.begin();
    for( ; itrRM!= regionMap.end(); itrRM++ ) {
        rmCount++;
    }
    DebugScreen::newLine(ss);
}

TileMap::t_regionMap* TileMap::getRegionMap() { return &regionMap; }
Planet* TileMap::getPlanet() { return planet; }
TileDict* TileMap::getTileDict() { return tileDict; }

/*
    This function will work fine in small loops but keep in mind getRegSubPos() is somewhat expensive. For large or even 3D loops (like in TileMapScreen::draw()) use a TileIterator.

*/
Tile TileMap::getTile(int64_t x, int64_t y, int64_t z)
{
    TileRegion* tr = getRegByXYZ(x, y, z);
    getRegSubPos(x, y, z);

    if( tr!=nullptr )
        return tr->getTile(x, y, z);

    //If tr==null
    return Tile();
}
Tile TileMap::getTileByCsXYZ(Camera* cam, Vec3<int64_t> cspos)
{
	Vec3<int64_t> wpos = cam->getWorldPosFromCsPos(cspos);
	return getTile(wpos.x, wpos.y, wpos.z);
}

TileRegion* TileMap::getRegByXYZ(int64_t x, int64_t y, int64_t z)
{
    getRegRXYZ(x, y, z);
    return getRegByRXYZ(x, y, z);
}

TileRegion* TileMap::getRegByRXYZ(t_regionMap* regMap, int64_t rX, int64_t rY, int64_t rZ)
{
    t_regionMap::iterator itr = regMap->find( std::make_tuple(rX, rY, rZ) );
    if ( itr!=regMap->end() )
        return &itr->second;
    return nullptr;
}

TileRegion* TileMap::getRegByRXYZ(int64_t rX, int64_t rY, int64_t rZ) { return getRegByRXYZ(&regionMap, rX, rY, rZ); }

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
bool TileMap::collides(Box3<double> b, int64_t& cx, int64_t& cy, int64_t& cz)
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
				if(tr->getTile(sx, sy, sz).solid ) {
					int64_t x = ti.getItrReg(0)*32+sx;
					int64_t y = ti.getItrReg(1)*32+sy;
					int64_t z = ti.getItrReg(2)*32+sz;
					//Build tileBox and check for collision against it
					Box3<double> tileBox(x, y, z, x+1, y+1, z+1);
					if(b.collides(tileBox)) {
						cx = x; cy = y; cz = z;
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

bool TileMap::collides(Box3<double> b)
{
	int64_t x, y, z = 0;
	return collides(b, x, y, z);
}

int TileMap::setTile(int64_t x, int64_t y, int64_t z, Tile t)
{
	TileRegion* tr = getRegByXYZ(x, y, z);
	getRegSubPos(x, y, z);
	if( tr!=nullptr ) {
		tr->setTile(x, y, z, t);
		return 0;
	}
	return -1;
}

int TileMap::setTileByCsXYZ(Camera* cam, int64_t csX, int64_t csY, int64_t csZ, Tile t)
{
	switch(cam->getAxis()) {
		case Camera::X: return setTile(csZ, csX, csY, t); break;
		case Camera::Y: return setTile(csX, csZ, csY, t); break;
	}
	return setTile(csX, csY, csZ, t);
}

bool TileMap::setTiles(t_regionMap* rm, int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2, Tile t, bool natural)
{
	//Validate selected volume specified by (x1, y1, z1) -> (x2, y2, z2).
	//If any regions within that volume are unloaded, operation should fail.
	TileIterator ti(rm);
	if(ti.setBounds(x1, y1, z1, x2, y2, z2)!=0) {
		nch::Log::warnv(__PRETTY_FUNCTION__, "doing nothing", "Could not place tiles within unloaded area (%d, %d, %d) <-> (%d, %d, %d)", x1, y1, z1, x2, y2, z2);
		return false;
	}
	
	//Fill the appropriate volumes within all contained regions
	for(TileRegion* tr = ti.peekRegion(); !ti.atEnd(); tr = ti.nextRegion()) {
		int16_t id = tr->addToPalette(t, natural);
		tr->setTiles(ti.gbs(0), ti.gbs(1), ti.gbs(2), ti.ges(0), ti.ges(1), ti.ges(2), id);
	}

	//Successful operation
	return true;
}

bool TileMap::setTiles(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2, Tile tt, bool natural)
{
	return setTiles(&regionMap, x1, y1, z1, x2, y2, z2, tt, natural);
}

void TileMap::setStructureWithinReg(Structure* stru, TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	//Get relevant structure bounds and world bounds
	Vec3<int64_t> so = stru->getOrigin();																	//[S]tructure [O]rigin (min xyz location)
	Vec3<int64_t> sb = stru->getBounds();																	//[S]tructure [B]ounds
	Box3<int64_t> wb(rX*32, rY*32, rZ*32, rX*32+31, rY*32+31, rZ*32+31);									//[W]orld [B]ounds			(32*32*32)
	Box3<int64_t> ucb(wb.c1.x-so.x, wb.c1.y-so.y, wb.c1.z-so.z, wb.c2.x-so.x, wb.c2.y-so.y, wb.c2.z-so.z);	//[U]n[C]lipped [B]ounds	(32*32*32)
	Box3<int64_t> cb = ucb.intersection(Box3<int64_t>(0, 0, 0, sb.x-1, sb.y-1, sb.z-1));					//[C]lipped [B]ounds		(a*b*c)
	if(cb==Box3<int64_t>(0, 0, 0, 0, 0, 0)) {
		Box3<int64_t> sbb = stru->getBoundingBox();
		nch::Log::errorv(
			__PRETTY_FUNCTION__,
			"empty intersection",
			"Failed to work with structure bounds: (%d, %d, %d) <-> (%d, %d, %d)",
			sbb.c1.x, sbb.c1.y, sbb.c1.z, sbb.c2.x, sbb.c2.y, sbb.c2.z
		);
	}
	Box3<int64_t> wfb(																						//[W]orld [F]inal [B]ounds	(a*b*c)
		getRegSubPos(cb.c1.x+so.x), getRegSubPos(cb.c1.y+so.y), getRegSubPos(cb.c1.z+so.z),
		getRegSubPos(cb.c2.x+so.x), getRegSubPos(cb.c2.y+so.y), getRegSubPos(cb.c2.z+so.z)
	);
	
	//TileIterator thru Structure (tiS).
	//Set bounds within tiS to be the part of the structure to be generated (32x32x32 regardless of whether near an edge or not)
	TileIterator tiS(stru->getRegionMap());
	tiS.setTrackerMode(tiS.SINGLE);
	if(tiS.setBounds(cb)==-1) {
		nch::Log::errorv(__PRETTY_FUNCTION__, "Structure bounds contains unloaded regions", "Failed to set TileIterator bounds");
	}

	for(TileRegion* tir = tiS.peekRegion(); !tiS.atEnd(); tir = tiS.nextRegion()) {		
		//Build palette lookup map (from tir's palIdx -> tr's palIdx)
		std::map<int16_t, int16_t> paletteMatches;
		for(int16_t i = 0; i<(int16_t)tir->getPaletteSizeNatural(); i++) {
			int16_t tirIdx = i;
			int16_t trIdx = tr.addToPalette(tir->getPaletteElement(i), true);
			paletteMatches.insert(std::make_pair(tirIdx, trIdx));
		}

		//Place the relevant tile volume from this structure into the current world region.
		int64_t wtDX = tiS.gip(0)-cb.c1.x, wtDY = tiS.gip(1)-cb.c1.y, wtDZ = tiS.gip(2)-cb.c1.z;	//[W]orld [T]ile [D]elta[XYZ] (0-31)
		for(int64_t idsx = 0; idsx<=tiS.ges(0)-tiS.gbs(0); idsx++)
		for(int64_t idsy = 0; idsy<=tiS.ges(1)-tiS.gbs(1); idsy++)
		for(int64_t idsz = 0; idsz<=tiS.ges(2)-tiS.gbs(2); idsz++) {
			//Get the current tile index from the structure corresponding to this position
			int16_t tirIdx = tir->getTileKey(tiS.gbs(0)+idsx, tiS.gbs(1)+idsy, tiS.gbs(2)+idsz);
			Vec3<int64_t> dst(wfb.c1.x+idsx+wtDX, wfb.c1.y+idsy+wtDY, wfb.c1.z+idsz+wtDZ);
			
			//If this is a non-null tile, place something in the world.
			if(tirIdx!=0) {
				try {
					int16_t idx = paletteMatches.at(tirIdx);
					tr.setTile(dst.x, dst.y, dst.z, idx);
				} catch(std::out_of_range) {
					nch::Log::errorv(__PRETTY_FUNCTION__, "Failed to place structure tile within world", "Palette index out of range");
				}
			}
		}
	}
}

/*
	Given a nonexistent region (rX, rY, rZ), create the region, generate its terrain, and load its artificial tiles from file.
	Returns: 0 if successful, -1 if region already existed.
*/
int TileMap::loadRegion(int64_t rX, int64_t rY, int64_t rZ)
{
	//Try to find the region (rX, rY, rZ).
	t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
	//If no region was found, create the region and process it.
	if( itr==regionMap.end() ) {
		//Create Terrain and TileRegion objects
		Terrain terra(nMap, tileDict);
		TileRegion tr(tileDict);
		
		//Populate region's terrain
		tr.setRegTexState(tr.GENERATING);
		terra.populateRegion(tr, rX, rY, rZ);
		tr.setRegTexState(tr.FINISHED_GENERATING);

		//Place tiles that are part of structures
		std::vector<Structure*> regStructures = struMap->getStructuresInRXYZ(rX, rY, rZ);
		for(Structure* stru : regStructures) {
			setStructureWithinReg(stru, tr, rX, rY, rZ);
		}

		//Place region's artificial tiles
		tr.load(saveGameName, rX, rY, rZ);
		
		//Insert the region into the regionMap.
		regionMap.insert( std::make_pair(std::make_tuple(rX, rY, rZ), tr) );
		return 0;
	}
	return -1;
}

int TileMap::loadRegionByCsRXYZ(int camAxis, int64_t csRX, int64_t csRY, int64_t csRZ)
{
	switch(camAxis) {
		case Camera::X: return loadRegion(csRZ, csRX, csRY); break;
		case Camera::Y: return loadRegion(csRX, csRZ, csRY); break;
	}
	return loadRegion(csRX, csRY, csRZ);
}

/*
	Load a rectangular prism of regions.
	Warning: will cause more lag at once compared to a single loadRegion().
	Returns: -x, where x is the number of regions that couldn't be loaded (because they already existed there)
*/
int TileMap::loadRegions(int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2)
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
				res += loadRegion(iRX, iRY, iRZ);
			}
		}
	}

	//Return result
	return res;
}

int TileMap::saveRegion(int64_t rX, int64_t rY, int64_t rZ)
{
	TileRegion* tr = getRegByRXYZ(rX, rY, rZ);
	
	if( tr==nullptr ) 					{ return -99; }
	if( !tr->beenModifiedSinceLoad() ) 	{ return -1; }
	
	nch::Log::debug("Saving region (%d, %d, %d) in save \"%s\"", rX, rY, rZ, saveGameName.c_str());	
	tr->save(saveGameName, rX, rY, rZ, false);
	return 0;
}

int TileMap::unloadRegion(int64_t rX, int64_t rY, int64_t rZ)
{
	t_regionMap::iterator itr = regionMap.find( std::make_tuple(rX, rY, rZ) );
	if( itr!=regionMap.end() ) {
		regionMap.erase(itr);
		return 0;
	}
	return -1;
}