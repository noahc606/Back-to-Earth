#include "TileRegion.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <nch/cpp-utils/log.h>
#include <stdlib.h>
#include <sstream>
#include "DebugScreen.h"
#include "LevelSave.h"
#include "TileMap.h"
#include "Timer.h"

using namespace nch;

/**
(X+, Y+, Z+) = (East, South, Down)
*/
TileRegion::TileRegion(TileDict* tileDict, size_t bitsize)
{
	TileRegion::tileDict = tileDict;
	TileRegion::tiles = new TileRegionArray(bitsize);

	resetPalette();
	resetTiles();
}
TileRegion::TileRegion(TileDict* tileDict): TileRegion(tileDict, 16) {}

TileRegion::~TileRegion() { if(tiles==nullptr) delete tiles; }

void TileRegion::putPaletteInfo(std::stringstream& ss, int& tabs, bool natural)
{
	t_palette::iterator pitr = palette.begin();
	for( ; pitr!=palette.end(); pitr++ ) {
		ss << "(" << pitr->first << ", ";
		ss << std::setfill('0') << std::setw(16) << std::hex << tileDict->getNumericID(pitr->second.id) << std::dec << ")";
		ss << " ";
	}
}

void TileRegion::putInfo(std::stringstream& ss, int& tabs, int subX, int subY, int subZ)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "RegTexState=" << regTexState << "; ";
	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "palette.size()=" << (int)palette.size() << ", " << "natural=" << getPaletteSizeNatural() << ", artificial=" << getPaletteSizeArtificial() << "; ";
	ss << "paletteSizeBucket=" << getArtificialPaletteSizeBucket() << "; ";
	
	if(!true) {
		ss << "palette={ ";
		putPaletteInfo(ss, tabs, true);
		ss << "}; ";
	}
	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "modifiedSinceLoad=" << modifiedSinceLoad << ";";
	DebugScreen::newLine(ss);

	Tile t = getTile(subX, subY, subZ);
	if( (subX<=-1||subY<=-1||subZ<=-1) ) {
		DebugScreen::newGroup(ss, tabs, "Selected Tile: [DNE]");
	} else {
		std::stringstream st; st << "Selected Tile: " << t.id;
		DebugScreen::newGroup(ss, tabs, st.str());
			DebugScreen::indentLine(ss, tabs);
			ss << "material=" << t.material;
			DebugScreen::newIndentedLine(ss, tabs);
			ss << "skipRendering=" << t.skipRendering;
			DebugScreen::newIndentedLine(ss, tabs);
			ss << "solid=" << t.solid;
			DebugScreen::newIndentedLine(ss, tabs);
			ss << "mapColor=(" << (int)t.mapColor.r << "," << (int)t.mapColor.g << "," << (int)t.mapColor.b << ")";
			DebugScreen::newIndentedLine(ss, tabs);
			ss << "textureHolder=" << t.textureHolder;
			DebugScreen::newLine(ss);
		DebugScreen::endGroup(tabs);
	}
	DebugScreen::endGroup(tabs);
}
void TileRegion::putInfo(std::stringstream& ss, int& tabs) { putInfo(ss, tabs, -1, -1, -1); }

std::string TileRegion::getInfo(int subX, int subY, int subZ)
{
	std::stringstream ss;
	int tabs = 0;
	putInfo(ss, tabs, subX, subY, subZ);
	return ss.str();
}

uint16_t TileRegion::getPaletteSize() { return palette.size(); }
/**
 *	Return the # of map elements whose keys >=0 (includes 0!).
 */
uint16_t TileRegion::getPaletteSizeNatural()
{
	if( palette.size()!=0 ) {
		return palette.rbegin()->first+1;
	} else {
		return 0;
	}
}
/**
 *	Return the # of map elements whose keys<0 (doesn't include 0!).
 */
uint16_t TileRegion::getPaletteSizeArtificial()
{
	if(palette.size()!=0) { return -palette.begin()->first; }
	else 				  { return 0; }
}

/**
 * size=0 -> return -1
 * size=1 -> return 0
 * size=2 -> return 1
 * size=3-4 -> return 2
 * size=5-8 -> return 3
 * ...
 */
int TileRegion::getPaletteSizeBucket(int size)
{
	if(size==0) return -1;
	
	int res = 0;
	while(size!=1) {
		size = std::ceil( ((double)size)/2.0 );
		res++;
	}
	return res;
}

int TileRegion::getArtificialPaletteSizeBucket() { return getPaletteSizeBucket( getPaletteSizeArtificial() ); }

Tile TileRegion::getPaletteElement(int16_t key)
{
	t_palette::iterator pitr = palette.find(key);
	if( pitr!=palette.end() ) {
		return pitr->second;
	} else {
		return Tile();
	}
}

int16_t TileRegion::getTileKey(int x, int y, int z)
{
	//Negative key indicates artificial tile. Positive index indicates natural tile. 0 always = null.
	return tiles->at(x, y, z);
}

Tile TileRegion::getTile(int x, int y, int z)
{
	int16_t key = getTileKey(x, y, z);
	t_palette::iterator pitr = palette.find(key);
	if(pitr!=palette.end()) {
		return pitr->second;
	} else {
		std::stringstream ss;
		ss << "Tile key '" << key << "' @ (" << x << ", " << y << ", " << z << ") does not exist in palette ";
		ss << "(min=" << -getPaletteSizeArtificial() << ", max=" << getPaletteSizeNatural()-1 << ")";
		nch::Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning default tile");
		return Tile();
	}
	
	return palette[key];
}

std::vector<Vec3<int64_t>> TileRegion::getTileEntityLocs()
{
	std::vector<Vec3<int64_t>> res;
	for(auto elem : tileEntities) {
		res.push_back(Vec3<int64_t>(elem.first));
	}
	return res;
}

/**
 * 	Returns whether this region has been modified since being loaded.
 * 	The placement of "natural" tiles (key>=0: aren't saved to file) do not count as modification.
 * 	The placement of artificial tiles sets modifiedSinceLoad to be true.
 */
bool TileRegion::beenModifiedSinceLoad() { return modifiedSinceLoad; }
int TileRegion::getRegTexState() { return regTexState; }

bool TileRegion::assertDefaultTileExists(t_palette& pal)
{
	if( pal.find(0)==pal.end() ) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Could not find default palette element", "stopping");
		return false;
	}
	
	return true;
}

int16_t TileRegion::addToPalette(Tile tile, t_palette& pal, bool natural)
{
	/* Preliminaries */
	assertDefaultTileExists(pal);				//Preliminary checking
	if(tile.id=="null") { return 0; }			//Already created default tile
	if(!natural) {  modifiedSinceLoad = true; }	//Mark this region as being modified IF natural==false.
	
	/* Iterate through elements */
	//Start at 0 (natural -> [0, +#] & !natural -> [0, -#]
	int palSize = 0;
	if(natural) {
		t_palette::iterator pitr = pal.find(0);
		palSize = getPaletteSizeNatural();
		for( ; pitr!=pal.end(); pitr++ ) {
			if( tile.id==pitr->second.id ) {
				//Return location of already existing element
				return pitr->first;
			}
		}
	} else {
		palSize = -getPaletteSizeArtificial()-1;
		t_palette::iterator pitr = pal.find(0);
		for( ; pitr!=pal.begin(); pitr-- ) {
			if( tile.id==pitr->second.id ) {
				//Return location of already existing element
				return pitr->first;
			}
		}
	}
	
	/* Insert, return */
	//Insert new element and return location of insertion
	pal.insert( std::make_pair(palSize, tile) );
	return palSize;
}
int16_t TileRegion::addToPalette(Tile tile, bool natural) { return addToPalette(tile, palette, natural); }
int16_t TileRegion::addToPalette(Tile tile, t_palette& pal) { return addToPalette(tile, pal, false); }
int16_t TileRegion::addToPalette(Tile tile) { return addToPalette(tile, palette); }


/**
	Ignores checking the entire palette for copies of a tile type.
*/
int16_t TileRegion::addToPaletteFast(Tile tile, bool natural)
{
	if(natural) {
		palette.insert( std::make_pair(getPaletteSizeNatural(), tile) );
		return getPaletteSizeNatural();
	} else {
		palette.insert( std::make_pair(-getPaletteSizeArtificial()-1, tile) );
		return -getPaletteSizeArtificial()-1;
	}
}

int16_t TileRegion::addToPaletteFast(Tile tile) { return addToPaletteFast(tile, false); }

/**
	General-purpose setTile function.
	Performance:
		-This is expensive when used in multidimensional loops - use setTiles() to fill any rectangular-prism-area with a single type of tile.
		-Use setTile(int, int, int, int) whenever you want to place tiles directly from the palette (for example, during region generation).
*/
void TileRegion::setTile( int x, int y, int z, Tile tile ) { tiles->set(x, y, z, addToPalette(tile)); }

/**
	A faster setTile function that copies a tile from the palette directly and places it somewhere.
	Cannot be used to generate a new tile (again, only tiles from the palette can be used).
*/
void TileRegion::setTile(int x, int y, int z, int16_t paletteIndex) { tiles->set(x, y, z, paletteIndex); }

void TileRegion::setTiles(int x1, int y1, int z1, int x2, int y2, int z2, Tile tile) { setTiles(addToPalette(tile), x1, y1, z1, x2, y2, z2); }

void TileRegion::setTiles(int x1, int y1, int z1, int x2, int y2, int z2, int16_t paletteIndex)
{
	for( int x = x1; x<=x2; x++ )
	for( int y = y1; y<=y2; y++ )
	for( int z = z1; z<=z2; z++ ) {
		tiles->set(x, y, z, paletteIndex);
	}
}

void TileRegion::resetPalette()
{
	//Clear palette.
	palette.clear();
	
	//Create 'empty' tile (0). Every tile palette should have this element as key '0'.
	Tile t = tileDict->at("null");
	
	//Insert into palette map with key 0
	palette.insert( std::make_pair(0, t) );
}

void TileRegion::resetArtificialPalette()
{
	if( !assertDefaultTileExists(palette) ) { return; }
	
	t_palette::iterator pitr = palette.find(0);
	while(palette.begin()!=pitr) {
		palette.erase( palette.begin() );
	}
}

void TileRegion::resetTiles() { tiles->reset(); }

void TileRegion::setRegTexState(int p_rts) { regTexState = p_rts; }
void TileRegion::resetRegTexState() { regTexState = 0; }

/**
	Compresses the tile palette. Rarely, there could be unused or repeated tiles in a palette - this function takes care of that.
	This is a relatively expensive function since it needs to "rebuild" the entire palette from scratch - just don't use it *too* often.
	Automatically called when:
		-A 'hard save' occurs (when this region is unloaded - NOT when this region is auto-saved)
		-a TileRegion with palette size>60000 (theoretical max 65535) is detected - very rare circumstance.
*/
void TileRegion::compress()
{
	//Build new tile palette.
	t_palette paletteNew;
	for( int x = 0; x<32; x++ ) {
		for( int y = 0; y<32; y++ ) {
			for( int z = 0; z<32; z++ ) {
				addToPalette( getTile(x, y, z), paletteNew );
			}
		}
	}

	//Replace old palette with new palette
	palette = paletteNew;
}

void TileRegion::dumpPaletteData(DataStream& ds, uint8_t dataBitsPerTile)
{
	//Preliminary checking
	assertDefaultTileExists(palette);
	
	if(dataBitsPerTile>=0b1111) {
		return;
	}
	
	int chunksLeft = std::pow(2, dataBitsPerTile)-1;
	t_palette::iterator pitr = palette.find(0);
	ds.put64Bits(0);
	
	while( pitr!=palette.begin() ) {
		pitr--;
		chunksLeft--;
		ds.put64Bits( tileDict->getNumericID(pitr->second.id) );
	}
	
	while(chunksLeft) {
		chunksLeft--;
		ds.put64Bits(0xffffffffffffffff);
	}
}

void TileRegion::dumpTileData(DataStream& ds, uint8_t dataBitsPerTile)
{
	for( uint8_t sx = 0; sx<32; sx++ )
	for( uint8_t sy = 0; sy<32; sy++ )
	for( uint8_t sz = 0; sz<32; sz++ ) {
		int16_t key = -getTileKey(sx, sy, sz);
		if(key>0) { ds.putXBits( key, dataBitsPerTile ); 
		} else { ds.putXBits( 0, dataBitsPerTile ); }
	}
}

/**
	Saves this region to disk.

	Automatically called whenever:
		-This region needs to be unloaded (compress==true). More expensive option
		-This region is auto-saved (compress==false). Less expensive option
*/
void TileRegion::save(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ, bool p_compress)
{
	//Make sure folder which contains the region files themselves exists
	std::string saveGameDir = "backtoearth/saved/games/"+saveGameName;

	//Compression
	if(p_compress) {
		nch::Timer t1;
		compress();
	}

	//Level saving logic
	LevelSave ls(saveGameDir, tileDict);
	ls.saveRegionTiles(*this, rX, rY, rZ);
	ls.saveRegionTileEntities(*this, rX, rY, rZ);
}

void TileRegion::save(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ)
{
	save(saveGameName, rX, rY, rZ, false);
}

void TileRegion::load(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ)
{
	std::string saveGameDir = "backtoearth/saved/games/"+saveGameName;
	LevelSave ls(saveGameDir, tileDict);
	ls.loadRegionTiles(*this, rX, rY, rZ);
	ls.loadRegionTileEntities(*this, rX, rY, rZ);
}

TileEntity* TileRegion::getTileEntity(Vec3<int64_t> subpos)
{
	auto itr = tileEntities.find(subpos.tuple());
	if(itr!=tileEntities.end()) {
		return itr->second;
	}
	return nullptr;
}

void TileRegion::setTileEntity(Vec3<int64_t> subpos, TileEntity* te)
{
	auto itr = tileEntities.find(subpos.tuple());
	if(itr==tileEntities.end()) {
		tileEntities.insert(std::make_pair(subpos.tuple(), te));
	} else {
		delete itr->second;
		itr->second = te;
	}
}