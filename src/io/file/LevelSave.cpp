#include "LevelSave.h"
#include <math.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include <set>
#include <sstream>
#include "DebugScreen.h"
#include "TileMap.h"

LevelSave::LevelSave(std::string dir)
{
	if(dir=="") dir = "dump";	
	directory = dir;
}

LevelSave::LevelSave():
LevelSave::LevelSave(""){}

std::string LevelSave::getNatFilePathFromRxyz(std::string parentDir, int64_t rX, int64_t rY, int64_t rZ)
{
	//LSR = (32*16)x(32*16)x(32*4)=512x512x128 (LARGE save region). Determines which file the data is placed in.
	
	//Get SRxyz from Rxyz
	int64_t lsrX = TileMap::convRxyToLSRxy(rX);
	int64_t lsrY = TileMap::convRxyToLSRxy(rY);
	int64_t lsrZ = TileMap::convRzToLSRz(rZ);
	
	//Create file name
	std::stringstream ss;
	ss << parentDir << "/";
	ss << lsrX << ",";
	ss << lsrY << ",";
	ss << lsrZ;
	
	return ss.str();
}

std::string LevelSave::getNatHeaderTriple(int64_t rX, int64_t rY, int64_t rZ)
{
	std::stringstream res;
	res << "[" << rX << "," << rY << "," << rZ << "]";
	return res.str();
}

void LevelSave::saveTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path and header triple
	std::string sfp = getNatFilePathFromRxyz(directory, rX, rY, rZ);
	std::string headerTriple = getNatHeaderTriple(rX, rY, rZ);
	//Get region's palette size
	uint8_t dataBitsPerTile = tr.getPaletteSizeBucket( tr.getArtificialPaletteSize()+1 );

	/* Execute the save operation */
	//If region has no artificial tiles (dataBitsPerTile==0), nothing to save.
	if(dataBitsPerTile==0) return;

	nch::NoahAllocTable nat(sfp);
	DataStream ds;

	//Save palette size data
	ds.putByte(dataBitsPerTile);
	std::vector<unsigned char> dbptData = ds.getByteStreamCopy();
	ds.clear();
	nat.save("lsr"+headerTriple+".dbpt", dbptData);

	//Save palettte data
	tr.dumpPaletteData(ds, dataBitsPerTile);
	std::vector<unsigned char> palData = ds.getByteStreamCopy();
	ds.clear();
	nat.save("lsr"+headerTriple+".palette", palData);

	//Save tile data
	tr.dumpTileData(ds, dataBitsPerTile);
	std::vector<unsigned char> tileData = ds.getByteStreamCopy();
	ds.clear();
	nat.save("lsr"+headerTriple+".tiles", tileData);
	
	nat.close();
}

void LevelSave::saveTileRegion(TileMap* tm, int64_t rX, int64_t rY, int64_t rZ)
{
	TileRegion* tr = tm->getRegByRXYZ(rX, rY, rZ);
	if(tr!=nullptr) {
		saveTileRegion( *tr, rX, rY, rZ );
	} else {
		std::stringstream ss;
		ss << "TileRegion at (" << rX << ", " << rY << ", " << rZ << ") doesn't exist";
		nch::Log::warn(__PRETTY_FUNCTION__, ss.str());
	}
}

bool LevelSave::loadTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path and header triple
	std::string lfp = getNatFilePathFromRxyz(directory, rX, rY, rZ);
	std::string headerTriple = getNatHeaderTriple(rX, rY, rZ);
	//Method returns false if file(s) don't exist.
	nch::FsUtils fsu;
	if(!fsu.fileExists(lfp+".nat") || !fsu.fileExists(lfp+".nath")) {
		return false;
	}
	//Get
	
	//Open file(s) for reading and return true at the end (since they exist).
	nch::NoahAllocTable nat(lfp);
	auto dbptBytes = nat.load("lsr"+headerTriple+".dbpt");
	auto palBytes = nat.load("lsr"+headerTriple+".palette");
	auto tileBytes = nat.load("lsr"+headerTriple+".tiles");
	nat.close();

	//If dbptBytes, palBytes, OR tileBytes are empty (indicates unpopulated data), return true here.
	if(dbptBytes.size()==0 || palBytes.size()==0 || tileBytes.size()==0) {
		return true;
	}

	//Reset and build palette from file
	tr.resetArtificialPalette();
	DataStream tempDS;
	for(int i = 0; i<palBytes.size(); i += 8) {
		//Read palBytes 8 bytes at a time and add to 'tempDS'
		for(int j = 0; j<8; j++) tempDS.putByte(palBytes[i+j]);

		//Each palette element is a uint64_t value (8 bytes).
		uint64_t val = tempDS.peekXBits(64);
		if(val!=0 && val!=0xffffffffffffffff) {
			//Add artificial TileType with value 'val' to the TileRegion.
			TileType tt;
			tt.init();
			tt.setVal(val);
			tr.addToPaletteFast(tt);
		}

		//Clear 'tempDS' for next itration
		tempDS.clear();
	}
		
	//Put tile data from file into datastream
	DataStream tileDS;
	for(int i = 0; i<tileBytes.size(); i++) {
		tileDS.putByte(tileBytes[i]);
	}

	//Build tiles from datastream, iterating through 'dataBitsPerTile' # of bits at a time.
	uint8_t dataBitsPerTile = dbptBytes[0];			//Get data bits per tile
	uint16_t aps = tr.getArtificialPaletteSize();	//Get # of newly-built palette's artificial elements.
	for(int sx = 0; sx<32; sx++) {
		for(int sy = 0; sy<32; sy++) {
			for(int sz = 0; sz<32; sz++) {
				//Get artificial tile value - must negate from saved data, since negative values represent artifical palette indices.
				int16_t val = -tileDS.peekXBits(dataBitsPerTile);
				
				//-aps<=val<0: 	artificial tiles (includes placing air created from destroying tiles)
				//val==0:		Spots where we should not do anything (not even placing air)
				if( val>=-aps && val<=0 ) {
					if(val!=0) {
						//Set an artificial tile
						tr.setTile(sx, sy, sz, val);
					}
				} else {
					std::vector<int> sxyz = { sx, sy, sz };
					std::vector<int64_t> rXYZ = { rX, rY, rZ };
					loadError(val, sxyz, rXYZ, aps, lfp, tr);
				}
				tileDS.seekBitDelta(dataBitsPerTile);
			}
		}
	}
	
	return true;
}

void LevelSave::loadError(int16_t val, std::vector<int>& sxyz, std::vector<int64_t>& rXYZ, uint16_t aps, std::string lfp, TileRegion& tr)
{
	std::stringstream ss;
	ss << "\n";
	ss << "Invalid palette index " << val << " @ sxyz=(" << sxyz[0] << ", " << sxyz[1] << ", " << sxyz[2] << ") within rXYZ(" << rXYZ[0] << ", " << rXYZ[1] << ", " << rXYZ[2] << ").\n";
	ss << "Palette index should be within [" << -aps << ", 0]\n";
	ss << "File=" << lfp << ";\n";

	if(true) {
		nch::Log::warnv(__PRETTY_FUNCTION__, "placing debug tile\n====================", ss.str());
		TileType tt;
		tt.init();
		tt.setSolid(true);
		tt.setVisionBlocking(true);
		tt.setTextureXY(4, 0);
		tt.setRGB(255, 0, 255);
		tr.setTile(sxyz[0], sxyz[1], sxyz[2], tt);
	}

	if(!true) {
		nch::Log::throwException(__PRETTY_FUNCTION__, "Throwing exception to prevent re-saving of bad region...");
	}
}