#include "LevelSave.h"
#include <math.h>
#include <iostream>
#include <set>
#include <sstream>
#include "TileMap.h"
#include "Log.h"

LevelSave::LevelSave(){}

void LevelSave::saveTileRegion(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	//If file exists, check prefix. If the file doesn't exist or it has a bad prefix, (re)build the prefix and header.
	savePrelims(fh, tr, rX, rY, rZ);
	
	//Create DataStream
	DataStream ds;

	//Clear old data that was being pointed to by the header - we will replace it with all 0's.
	//Also, we will make the old header entry as being unpopulated.
	clearHeaderEntryPlusSaveSection(fh, rX, rY, rZ);

	//Create Header Entry with 40 bits of data, which holds the bits of 'dataLocation', 'dataBitsPerTile', and 'dataPopulated', in that order
	//The first 32bits of this data (dataLocation) represents the location (in bytes) within the file for the save data.
	//The next 4 bits (dataBitsPerTile) represents a hex number. This hex number points to the size of the save data (bits per tile).
	//The last 4 bits (dataPopulated) represents if this data is a real pointer. If so, these bits will be 0b1010 = 0xA. If not, it will be something else (most likely 0b1111).
	uint8_t dataBitsPerTile = tr.getArtificialPaletteSizeBucket();
	uint32_t dataLocation = getNewAllocationPos(fh, dataBitsPerTile);
	uint8_t dataPopulated = 0b1010;
	
	//Put in the 40-bit header entry somewhere in the header, depending on rX, rY, rZ.
	buildHeaderEntry(fh, rX, rY, rZ, dataLocation, dataBitsPerTile, dataPopulated);
	
	//Save tile palette data and tile index data of this specific region. Will be written @ 'dataLocation' position in the file
	//Total of 32*32*32=1024 tiles. Each tile = 'dataBitsPerTile' bits. Two hex chars = 1 byte.
	fh->seekTo(dataLocation);
	
	//Dump tile palette and tile data, in that order, to datastream.
	tr.dumpPaletteData(ds, dataBitsPerTile);
	tr.dumpTileData(ds, dataBitsPerTile);
	
	//Dump bytes from datastream to file, @ 'dataLocation'.
	ds.dumpBytestream(fh, dataLocation);
	
	//Save and close file
	fh->saveCloseFile();
}

bool LevelSave::loadTileRegion(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	//TODO: finish this method
	return false;
	
	/* Preliminaries */
	//Get the file path
	std::string lsrFp = getLsrFilePathFromRxyz(rX, rY, rZ);
	//Method returns false if file doesn't exist.
	if( !fh->fileExists(lsrFp) ) {
		return false;
	}
	//If file does exist, open the file for reading and return true at the end.
	fh->openFile(lsrFp, FileHandler::READ);
	//Seek to header entry.
	fh->seekTo(8*2);
	fh->seekThru(getHeaderEntryDelta(rX, rY, rZ));
		
	/* Build basic information */
	//Create DataStream and read the next 5 bytes
	DataStream ds;
		for( int i = 0; i<5; i++ ) {
		ds.putByte(fh->readByte());
	}
		
	//Build dataLocation
	uint32_t dataLocation = getHeaderEntryData1(fh);
		
	//Build dataBitsPerTile
	uint8_t dataBitsPerTile = getHeaderEntryData2(fh);
	
	//Build dataPopulated
	uint8_t dataPopulated = getHeaderEntryData3(fh);
	ds.clear();
	
	/* Build TileRegion */
	//Seek to data location
	fh->seekTo(dataLocation);
	//Retrieve tile palette data - 64 bits per entry, 2^dataBitsPerTile entries.
	for(int i = 0; i<256; i++) {

	}
	//Retrieve tile index data ('dataBitsPerTile' bits per entry)
	for(int i = 0; i<32*32*32/8*dataBitsPerTile; i++) {
		//fh->readByte();
	}
	
	return true;
}

std::string LevelSave::getLsrFilePathFromRxyz(Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	//LSR = (32*16)x(32*16)x(32*4)=512x512x128 (LARGE save region). Determines which file the data is placed in.
	
	//Get SRxyz from Rxyz
	Defs::t_ll lsrX = TileMap::convRxyToLSRxy(rX);
	Defs::t_ll lsrY = TileMap::convRxyToLSRxy(rY);
	Defs::t_ll lsrZ = TileMap::convRzToLSRz(rZ);
	
	//Create file name
	std::stringstream ss;
	ss << "dump/";
	ss << lsrX << ",";
	ss << lsrY << ",";
	ss << lsrZ << ".bte_lsr";
	
	return ss.str();
}

uint32_t LevelSave::getHeaderEntryDelta(Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	/*	Total of 16*16*4=1024 header entries.
	 * 
	 * 	Based on a header entry's rX mod16 (0-15), rY mod16 (0-15), and rZ mod4 (0-3)...
	 * 	this function returns a number of bytes relative to where that data object should be.
	 */	
	return 	(5*16*4)*(rX%16)+
			(5*4)*(rY%16)+
			(5*1)*(rZ%4);
} 
uint32_t LevelSave::getHeaderEntryData1(FileHandler* fh)
{
	DataStream ds;
	ds.putByte( fh->readByte() );
	ds.putByte( fh->readByte() );
	ds.putByte( fh->readByte() );
	ds.putByte( fh->readByte() );
	fh->seekThru(-4);
	
	return ds.peekXBits(32);
}
uint8_t LevelSave::getHeaderEntryData2(FileHandler* fh)
{
	fh->seekThru(4);
	DataStream ds;
	ds.putByte( fh->readByteStay() );
	fh->seekThru(-4);
	
	return ds.peekXBits(4);
}
uint8_t LevelSave::getHeaderEntryData3(FileHandler* fh)
{
	fh->seekThru(4);
	DataStream ds;
	ds.putByte( fh->readByteStay() );
	fh->seekThru(-4);

	ds.seekBitDelta(4);
	return ds.peekXBits(4);
}

uint32_t LevelSave::getSaveSectionSizeBytes(uint8_t bitsize)
{
	uint32_t tiles = bitsize*32*32*32/8;
	uint32_t palette = 64*std::pow(2, bitsize)/8;
	
	return tiles+palette;
}
uint32_t LevelSave::getNewAllocationPos(FileHandler* fh, uint8_t bitsize)
{
	fh->seekTo(16);
	
    int nhe = 16*16*4;  // (n)umber of (h)eader (e)ntries
    int she = 5;        // (s)ize of 1 (h)eader (e)ntry (in bytes)

	std::map<uint32_t, uint32_t> usedDataAreas;
	std::map<uint32_t, uint32_t> freeDataAreas;

	/* Build map that holds a list of areas with USED data. */
	/* For example: An entry <1234, 5000> indicates there are 5000 USED bytes of data @ byte position 1234 in the file. */
	for(int i = 0; i<nhe; i++) {
		//DataStream ds;
		uint32_t loc = getHeaderEntryData1(fh);
		uint8_t bpt = getHeaderEntryData2(fh);
		uint8_t pop = getHeaderEntryData3(fh);
		
		if(pop==0b1010) {
			usedDataAreas.insert( std::make_pair(loc, getSaveSectionSizeBytes(bpt)) );
		}
		
		fh->seekThru(she);
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
	return fh->getFileLength();
}

void LevelSave::buildPrefixAndHeader(FileHandler* fh)
{
	//Create DataStream
	DataStream ds;
	
	/* Building DataStream */
	//Magic Number (Prefix)
	ds.put64Bits(magicNumberP1);
	ds.put64Bits(magicNumberP2);
	ds.dumpBytestream(fh, 0);
	
	//File Header (16*16*4=1024 entries, each having 40 bits of data.
	int numBits = 40;
	int numEntries = 16*16*4;
	for(int i = 0; i<numEntries; i++) {
		ds.putXBits((uint64_t)-1, numBits);
	}
	ds.dumpBytestream(fh, 8*2);
}
void LevelSave::buildHeaderEntry(FileHandler* fh, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ, uint32_t loc, uint8_t bpt, uint8_t pop)
{
	DataStream ds;
	fh->seekTo(8*2);
	fh->seekThru( getHeaderEntryDelta(rX, rY, rZ) );
	ds.putXBits(loc, 32);
	ds.putXBits(bpt, 4);
	ds.putXBits(pop, 4);
	ds.dumpBytestream(fh, fh->tellPos());
}
void LevelSave::savePrelims(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	std::string lsrFp = getLsrFilePathFromRxyz(rX, rY, rZ);
	
	bool fileExisted = fh->fileExists(lsrFp);
	
	//Create our file. Save and close it. Make sure to use APPEND instead of write, so we don't lose data that already exists
	fh->openFile(lsrFp, FileHandler::APPEND);
	fh->saveCloseFile();
	
	//Create a file again. This time, we are going do it in update+binary mode.
	fh->openFile(lsrFp, FileHandler::UPDATE, true);
	
	//If our file didn't exist or had a bad prefix at the time of file creation, build prefix and header.
	if(!fileExisted) {
		Log::log("Building new file...");
		buildPrefixAndHeader(fh);
	} else {
		fh->seekTo(0);
		if(!fh->checkMagicNumber(magicNumberP1, magicNumberP2)) {
			Log::log("Rebuilding prefix and header for bad file.");
			buildPrefixAndHeader(fh);
		}
	}
	
	fh->seekTo(0);
}

void LevelSave::clearHeaderEntryPlusSaveSection(FileHandler* fh, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ)
{
	//Get location of old data, if it exists. If it doesn't exist (pop!=0b1010), stop function after clearing header entry's 'pop' section.
	fh->seekTo(16+getHeaderEntryDelta(rX, rY, rZ) );
	uint32_t loc = getHeaderEntryData1(fh);
	uint32_t bpt = getHeaderEntryData2(fh);
	uint8_t pop = getHeaderEntryData3(fh);
		
	//Clear old header entry (put 40 1's in the header entry).
	//Then, add in everything except for the 'pop' data object (which will be 0x0).
	DataStream ds;
	ds.putXBits( (uint64_t)-1, 40 );
	ds.dumpBytestream(fh, 16+getHeaderEntryDelta(rX, rY, rZ));
	ds.putXBits(loc, 32);
	ds.putXBits(bpt, 4);
	ds.putXBits(0b0, 4);
	ds.dumpBytestream(fh, 16+getHeaderEntryDelta(rX, rY, rZ));
	
	//If the data doesn't exist, stop function here.
	if( pop!=0b1010 ) {
		return;
	}
	
	//Clear old data (put 0's throughout the entire save section).
	for(int i = 0; i<getSaveSectionSizeBytes(bpt); i++ ) {
		ds.putByte(0b0);
	}
	ds.dumpBytestream(fh, loc);
}