#include "LevelSave.h"
#include <math.h>
#include <cmath>
#include <iostream>
#include <set>
#include <sstream>
#include "TileMap.h"
#include "Log.h"

LevelSave::LevelSave(FileHandler* fh, std::string dir)
{
	if(dir=="") {
		dir = "dump";
	}
	
	fileHandler = fh;
	directory = dir;
}

LevelSave::LevelSave(FileHandler* fh):
LevelSave::LevelSave(fh, ""){}

void LevelSave::saveTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	//If file exists, check prefix. If the file doesn't exist or it has a bad prefix, (re)build the prefix and header.
	savePrelims(tr, rX, rY, rZ);
	
	
	//Create DataStream
	DataStream ds;
	
	//Clear old data that was being pointed to by the header - we will replace it with all 0's.
	//Also, we will make the old header entry as being unpopulated.
	clearHeaderEntryPlusSaveSection(rX, rY, rZ);

	//Create Header Entry with 40 bits of data, which holds the bits of 'dataLocation', 'dataBitsPerTile', and 'dataPopulated', in that order
	//The first 32bits of this data (dataLocation) represents the location (in bytes) within the file for the save data.
	//The next 4 bits (dataBitsPerTile) represents a hex number. This hex number points to the size of the save data (bits per tile).
	//The last 4 bits (dataPopulated) represents if this data is a real pointer. If so, these bits will be 0b1010 = 0xA. If not, it will be something else (most likely 0b1111).
	uint8_t dataBitsPerTile = tr.getPaletteSizeBucket( tr.getArtificialPaletteSize()+1 );
	uint32_t dataLocation = getNewAllocationPos(dataBitsPerTile);
	uint8_t dataPopulated = 0b1010;
	switch(dataBitsPerTile) {
		//If region is all empty space + naturally generated tiles (dataBitsPerTile is 0, meaning no artificial tiles)
		case 0: {
			//Build empty header, save and close file.
			buildHeaderEntry(rX, rY, rZ, 0xffffffff, 0b1111, 0b1111);
			fileHandler->saveCloseFile();
			return;
		} break;
	}
	
	//Put in the 40-bit header entry somewhere in the header, depending on rX, rY, rZ.
	buildHeaderEntry(rX, rY, rZ, dataLocation, dataBitsPerTile, dataPopulated);

	//Save tile palette data and tile index data of this specific region. Will be written @ 'dataLocation' position in the file
	//Total of 32*32*32=1024 tiles. Each tile = 'dataBitsPerTile' bits. Two hex chars = 1 byte.
	fileHandler->seekTo(dataLocation);
	
	//Dump tile palette and tile data, in that order, to datastream.
	//Separator 1
	ds.put64Bits(0xdddddddddddddddd);
	ds.put64Bits(0xdddddddddddddddd);
	//Palette data
	tr.dumpPaletteData(ds, dataBitsPerTile);
	//Separator 2
	ds.put64Bits(0xeeeeeeeeeeeeeeee);
	ds.put64Bits(0xeeeeeeeeeeeeeeee);
	//Tile data
	tr.dumpTileData(ds, dataBitsPerTile);
	
	//Dump bytes from datastream to file, @ 'dataLocation'.
	ds.dumpBytestream(fileHandler, dataLocation);


	//Save and close file
	fileHandler->saveCloseFile();
}

void LevelSave::saveTileRegion(TileMap* tm, int64_t rX, int64_t rY, int64_t rZ)
{
	TileRegion* tr = tm->getRegByRXYZ(rX, rY, rZ);
	if(tr!=nullptr) {
		saveTileRegion( *tr, rX, rY, rZ );
	} else {
		std::stringstream ss;
		ss << "TileRegion at (" << rX << ", " << rY << ", " << rZ << ") doesn't exist";
		Log::warn(__PRETTY_FUNCTION__, ss.str());
	}
}

bool LevelSave::loadTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	/* Preliminaries */
	//Get the file path
	std::string lsrFp = getLsrFilePathFromRxyz(rX, rY, rZ);
	//Method returns false if file doesn't exist.
	if( !fileHandler->fileExists(lsrFp) ) {
		return false;
	}
	//If file does exist, open the file for reading and return true at the end.
	fileHandler->openFile(lsrFp, FileHandler::READ);
	//Seek to header entry.
	fileHandler->seekTo(8*2);
	fileHandler->seekThru(getHeaderEntryDelta(rX, rY, rZ));
	
	/* Build basic information and seek to data location */
	//Build dataLocation
	uint32_t dataLocation = getHeaderEntryData1();
	//Build dataBitsPerTile
	uint8_t dataBitsPerTile = getHeaderEntryData2();
	//Build dataPopulated
	uint8_t dataPopulated = getHeaderEntryData3();
	
	/* Do not continue under certain conditions */
	if( dataPopulated!=0b1010 ) {
		fileHandler->saveCloseFile();
		return false;
	}
	
	/** Build tile palette data - 64 bits per entry, 2^dataBitsPerTile entries. */
	//Seek to palette data
	fileHandler->seekTo(dataLocation+16);
	//Reset and build palette from file
	tr.resetArtificialPalette();
	TileRegion::t_palette tpal;
	for( int i = 0; i<std::pow(2, dataBitsPerTile); i++ ) {
		//Read 8 bytes of the 64 bit tile
		DataStream tempDS;
		for( int j = 0; j<8; j++ ) {
			tempDS.putByte(fileHandler->readByte());
		}
		
		uint64_t val = tempDS.peekXBits(64);
		if( val!=0&&val!=0xffffffffffffffff ) {
			TileType tt;
			tt.setVal(val);
			tr.addToPaletteFast(tt);
		}
		tempDS.clear();
	}
	int aps = tr.getArtificialPaletteSize();
	
	/** Retrieve tile index data ('dataBitsPerTile' bits per entry) */
	//Seek to tile data
	fileHandler->seekTo(dataLocation+16+8*std::pow(2, dataBitsPerTile)+16);
	//Put tile data from file into datastream
	DataStream tileDS;
	for(int i = 0; i<32*32*32/8*dataBitsPerTile; i++) {
		tileDS.putByte(fileHandler->readByte());
	}
	//Build tiles from datastream, iterating through 'dataBitsPerTile' # of bits at a time.
	for(int sx = 0; sx<32; sx++) {
		for(int sy = 0; sy<32; sy++) {
			for(int sz = 0; sz<32; sz++) {
				int16_t val = -tileDS.peekXBits(dataBitsPerTile);
				if( val>=-aps && val<=0 ) {
					if(val!=0) {
						tr.setTile(sx, sy, sz, val);
					}
				} else {
					std::stringstream ss;
					ss << "Invalid tile value " << val;
					ss << " @ (sx, sy, sz)=(" << sx << ", " << sy << ", " << sz;
					ss << "), bytepos=" << tileDS.getSeekBytePos();
					ss << ". Should be within [" << -aps << ", 0]";
					Log::warn(__PRETTY_FUNCTION__, ss.str(), "stopping load");
					fileHandler->saveCloseFile();
					return false;
				}
				tileDS.seekBitDelta(dataBitsPerTile);
			}
		}
	}
	
	fileHandler->saveCloseFile();
	return true;
}

std::string LevelSave::getLsrFilePathFromRxyz(int64_t rX, int64_t rY, int64_t rZ)
{
	//LSR = (32*16)x(32*16)x(32*4)=512x512x128 (LARGE save region). Determines which file the data is placed in.
	
	//Get SRxyz from Rxyz
	int64_t lsrX = TileMap::convRxyToLSRxy(rX);
	int64_t lsrY = TileMap::convRxyToLSRxy(rY);
	int64_t lsrZ = TileMap::convRzToLSRz(rZ);
	
	//Create file name
	std::stringstream ss;
	ss << directory << "/";
	ss << lsrX << ",";
	ss << lsrY << ",";
	ss << lsrZ << ".bte_lsr";
	
	return ss.str();
}

uint32_t LevelSave::getHeaderEntryDelta(int64_t rX, int64_t rY, int64_t rZ)
{
	/*	Total of 16*16*4=1024 header entries.
	 * 
	 * 	Based on a header entry's rX mod16 (0-15), rY mod16 (0-15), and rZ mod4 (0-3)...
	 * 	this function returns a number of bytes relative to where that data object should be.
	 */
	
	int modRX = rX%16; if( modRX<0 ) modRX+=16;
	int modRY = rY%16; if( modRY<0 ) modRY+=16;
	int modRZ = rZ%4;  if( modRZ<0 ) modRZ+=4;
	
	return 	(5*16*4)*modRX+
			(5*4)*modRY+
			(5*1)*modRZ;
} 
uint32_t LevelSave::getHeaderEntryData1()
{
	DataStream ds;
	ds.putByte( fileHandler->readByte() );
	ds.putByte( fileHandler->readByte() );
	ds.putByte( fileHandler->readByte() );
	ds.putByte( fileHandler->readByte() );
	fileHandler->seekThru(-4);
	
	return ds.peekXBits(32);
}
uint8_t LevelSave::getHeaderEntryData2()
{
	fileHandler->seekThru(4);
	DataStream ds;
	ds.putByte( fileHandler->readByteStay() );
	fileHandler->seekThru(-4);
	
	return ds.peekXBits(4);
}
uint8_t LevelSave::getHeaderEntryData3()
{
	fileHandler->seekThru(4);
	DataStream ds;
	ds.putByte( fileHandler->readByteStay() );
	fileHandler->seekThru(-4);

	ds.seekBitDelta(4);
	return ds.peekXBits(4);
}

uint32_t LevelSave::getSaveSectionSizeBytes(uint8_t bitsize)
{
	uint32_t tiles = bitsize*32*32*32/8;
	uint32_t separator1 = 16;
	uint32_t palette = 64*std::pow(2, bitsize)/8;
	uint32_t separator2 = 16;
	
	return tiles+separator1+palette+separator2;
}
uint32_t LevelSave::getNewAllocationPos(uint8_t bitsize)
{
	fileHandler->seekTo(16);
	
	int nhe = 16*16*4;  // (n)umber of (h)eader (e)ntries
	int she = 5;        // (s)ize of 1 (h)eader (e)ntry (in bytes)
	
	std::map<uint32_t, uint32_t> usedDataAreas;
	std::map<uint32_t, uint32_t> freeDataAreas;

	/* Build map that holds a list of areas with USED data. */
	/* For example: An entry <1234, 5000> indicates there are 5000 USED bytes of data @ byte position 1234 in the file. */
	for(int i = 0; i<nhe; i++) {
		//DataStream ds;
		uint32_t loc = getHeaderEntryData1();
		uint8_t bpt = getHeaderEntryData2();
		uint8_t pop = getHeaderEntryData3();
		
		if(pop==0b1010) {
			usedDataAreas.insert( std::make_pair(loc, getSaveSectionSizeBytes(bpt)) );
		}
		
		fileHandler->seekThru(she);
	}
	
	/* Build map that holds a list of areas with FREE data. */
	/* For example: An entry <1234, 5000> indicates there are 5000 FREE (unused) bytes of data @ byte position 1234 in the file. */
	auto udaEntry = usedDataAreas.begin();
	auto udaEntryLast = usedDataAreas.begin();
	while( udaEntry!=usedDataAreas.end() ) {
		if( udaEntry!=usedDataAreas.begin() ) {
			uint32_t pos = (udaEntryLast->first+udaEntryLast->second);
			uint32_t size = udaEntry->first-pos;
			if( size>0 ) {
				freeDataAreas.insert( std::make_pair(pos, size) );
			}
		}

		udaEntryLast = udaEntry;
		udaEntry++;
	}

	/* Find the first available location where there is a chunk of free data with the appropriate size, based on getSaveSectionSizeBytes(bitsize). */
	//Special case: If usedDataAreas is empty, return the first available section right after the header.
	if( usedDataAreas.size()==0 ) {
		return (16+16*16*4*5);
	}
	//Look through free data areas
	auto fdaEntry = freeDataAreas.begin();
	while( fdaEntry!=freeDataAreas.end() ) {
		if( fdaEntry->second<=getSaveSectionSizeBytes(bitsize) ) {
			return fdaEntry->first;
		}
	}
	
	//If no available location was found, get the end of the file.
	return fileHandler->getFileLength();
}

void LevelSave::buildPrefixAndHeader()
{
	//Create DataStream
	DataStream ds;
	
	/* Building DataStream */
	//Magic Number (Prefix)
	ds.put64Bits(magicNumberP1);
	ds.put64Bits(magicNumberP2);
	ds.dumpBytestream(fileHandler, 0);
	
	//File Header (16*16*4=1024 entries, each having 40 bits of data.
	int numBits = 40;
	int numEntries = 16*16*4;
	for(int i = 0; i<numEntries; i++) {
		ds.putXBits((uint64_t)-1, numBits);
	}
	ds.dumpBytestream(fileHandler, 8*2);
}
void LevelSave::buildHeaderEntry(int64_t rX, int64_t rY, int64_t rZ, uint32_t loc, uint8_t bpt, uint8_t pop)
{
	DataStream ds;
	fileHandler->seekTo(8*2);
	fileHandler->seekThru( getHeaderEntryDelta(rX, rY, rZ) );
	ds.putXBits(loc, 32);
	ds.putXBits(bpt, 4);
	ds.putXBits(pop, 4);

	ds.dumpBytestream(fileHandler, fileHandler->tellPos());
}
void LevelSave::savePrelims(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ)
{
	std::string lsrFp = getLsrFilePathFromRxyz(rX, rY, rZ);
	
	bool fileExisted = fileHandler->fileExists(lsrFp);
	
	//Create our file. Save and close it. Make sure to use APPEND instead of write, so we don't lose data that already exists
	fileHandler->openFile(lsrFp, FileHandler::APPEND, true);
	fileHandler->saveCloseFile();
	
	//Create a file again. This time, we are going do it in update+binary mode.
	fileHandler->openFile(lsrFp, FileHandler::UPDATE, true);
	
	//If our file didn't exist or had a bad prefix at the time of file creation, build prefix and header.
	if(!fileExisted) {
		Log::log("Building new file...");
		buildPrefixAndHeader();
	} else {
		fileHandler->seekTo(0);
		if(!fileHandler->checkMagicNumber(magicNumberP1, magicNumberP2)) {
			Log::log("Rebuilding prefix and header for bad file.");
			buildPrefixAndHeader();
		}
	}
	
	fileHandler->seekTo(0);
}

void LevelSave::clearHeaderEntryPlusSaveSection(int64_t rX, int64_t rY, int64_t rZ)
{
	//Get location of old data, if it exists. If it doesn't exist (pop!=0b1010), stop function after clearing header entry's 'pop' section.
	fileHandler->seekTo(16+getHeaderEntryDelta(rX, rY, rZ));
	
	uint32_t loc = getHeaderEntryData1();
	uint32_t bpt = getHeaderEntryData2();
	uint8_t pop = getHeaderEntryData3();
	
	//Clear old header entry (put 40 1's in the header entry).
	//Then, add in everything except for the 'pop' data object (which will be 0x0).
	DataStream ds;
	ds.putXBits( (uint64_t)-1, 40 );
	ds.dumpBytestream(fileHandler, 16+getHeaderEntryDelta(rX, rY, rZ));
	ds.putXBits(loc, 32);
	ds.putXBits(bpt, 4);
	ds.putXBits(0b0, 4);
	ds.dumpBytestream(fileHandler, 16+getHeaderEntryDelta(rX, rY, rZ));
	
	//If the data doesn't exist, stop function here.
	if( pop!=0b1010 ) {
		return;
	}
	
	//Clear old data (put 0's throughout the entire save section).
	for(int i = 0; i<getSaveSectionSizeBytes(bpt); i++ ) {
		ds.putByte(0b0);
	}
	ds.dumpBytestream(fileHandler, loc);
}