#include "LevelSave.h"
#include <math.h>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include <nch/cpp-utils/string-utils.h>
#include <set>
#include <sstream>
#include "DebugScreen.h"
#include "TileMap.h"
#include "Tile.h"

using namespace nch;

LevelSave::LevelSave(std::string saveGameDir, TileDict* td)
{
	if(saveGameDir=="") {
		saveGameDir = "backtoearth/dump";
	}
	tilesDir = saveGameDir+"/tilemap/default";
	poiDir = saveGameDir+"/poi/default";
	LevelSave::td = td;
}


std::string LevelSave::getNatLSRPathFromRxyz(int64_t rX, int64_t rY, int64_t rZ)
{
	//LSR = (32*16)x(32*16)x(32*4)=512x512x128 (LARGE save region). Determines which file the data is placed in.
	
	//Get SRxyz from Rxyz
	int64_t lsrX = TileMap::convRxyToLSRxy(rX);
	int64_t lsrY = TileMap::convRxyToLSRxy(rY);
	int64_t lsrZ = TileMap::convRzToLSRz(rZ);
	
	//Create file name
	std::stringstream ss;
	ss << tilesDir << "/";
	ss << lsrX << ",";
	ss << lsrY << ",";
	ss << lsrZ;
	
	return ss.str();
}

std::string LevelSave::getNatPOIPathFromRxyz(int64_t rX, int64_t rY, int64_t rZ)
{
	std::stringstream ss;
	ss << poiDir << "/" << rX << "," << rY << "," << rZ;
	return ss.str();
}

std::string LevelSave::getNatHeaderTriple(int64_t rX, int64_t rY, int64_t rZ)
{
	std::stringstream res;
	res << "[" << rX << "," << rY << "," << rZ << "]";
	return res.str();
}

void LevelSave::saveRegionTiles(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path and header triple
	std::string sfp = getNatLSRPathFromRxyz(rX, rY, rZ);
	std::string headerTriple = getNatHeaderTriple(rX, rY, rZ);
	//Get region's palette size
	uint8_t dataBitsPerTile = tr.getPaletteSizeBucket( tr.getPaletteSizeArtificial()+1 );

	/* Execute the save operation */
	//If region has no artificial tiles (dataBitsPerTile==0), nothing to save.
	if(dataBitsPerTile==0) return;

	NoahAllocTable nat(sfp);
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

void LevelSave::saveRegionTileEntities(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path
	std::string sfp = getNatPOIPathFromRxyz(rX, rY, rZ);
	//Get region's tile entity locations
	std::vector<Vec3<int64_t>> tileEntityLocs = tr.getTileEntityLocs();

	/* Execute save operation */
	//If no tile entity locations, nothing to save.
	if(tileEntityLocs.size()==0) return;

	NoahAllocTable nat(sfp);
	for(int i = 0; i<tileEntityLocs.size(); i++) {
		Vec3<int64_t> teLoc = tileEntityLocs[i];
		TileEntity* te = tr.getTileEntity(teLoc);
		if(te==nullptr) continue;

		nlohmann::json j = te->jsonify();
		if(j.is_null()) continue;

		auto teData = nlohmann::json::to_bson(j);
		nat.save("poi"+teLoc.toArrayString()+".te", teData);
	}

	nat.close();
}

bool LevelSave::loadRegionTiles(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path and header triple
	std::string lfp = getNatLSRPathFromRxyz(rX, rY, rZ);
	std::string headerTriple = getNatHeaderTriple(rX, rY, rZ);
	//Method returns false if file(s) don't exist.
	nch::FsUtils fsu;
	if(!fsu.fileExists(lfp+".nat") || !fsu.fileExists(lfp+".nath")) {
		return false;
	}
	
	//Open file(s) for reading and return true at the end (since they exist).
	NoahAllocTable nat(lfp);
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
			//Add artificial tile with value 'val' to the TileRegion.
			Tile t = td->at(val);
			if(t.id=="null") {
				t = td->at("debug_tile");
			}
			tr.addToPaletteFast(t);
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
	uint16_t aps = tr.getPaletteSizeArtificial();	//Get # of newly-built palette's artificial elements.
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

bool LevelSave::loadRegionTileEntities(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get file path
	std::string lfp = getNatPOIPathFromRxyz(rX, rY, rZ);
	//Method returns false if file(s) don't exist.
	nch::FsUtils fsu;
	if(!fsu.fileExists(lfp+".nat") || !fsu.fileExists(lfp+".nath")) {
		return false;
	}

	NoahAllocTable nat(lfp);
	std::vector<std::string> labelList = nat.getLabelList();
	for(std::string lbl : labelList) {
		//Get TileEntity
		TileEntity* te = nullptr;
		auto teDataRaw = nat.load(lbl);
		nlohmann::json teJson = nlohmann::json::from_bson(teDataRaw);
		te = new TileEntity(teJson);

		//Get subpos within region
		std::vector<int64_t> list = StringUtils::parseI64Array( lbl.substr(3, lbl.size()-6) );
		Vec3<int64_t> subpos(list[0], list[1], list[2]);

		//Set within TileRegion the proper TileEntity and subposition
		tr.setTileEntity(subpos, te);
	}

	nat.close();

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
		Tile t = td->at("debug_tile");
		tr.setTile(sxyz[0], sxyz[1], sxyz[2], t);
	}

	if(!true) {
		nch::Log::throwException(__PRETTY_FUNCTION__, "Throwing exception to prevent re-saving of bad region...");
	}
}