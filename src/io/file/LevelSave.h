#pragma once
#include "DataStream.h"
#include "FileHandler.h"
#include "TileMap.h"
#include "TileRegion.h"
#include "TileType.h"
#include <string>

class LevelSave : public Loggable
{
public:
    /**/
    LevelSave(FileHandler* fh, std::string dir);
    LevelSave(FileHandler* fh);
	/**/
	void putInfo(std::stringstream& ss, int& tabs, int64_t rX, int64_t rY, int64_t rZ);
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	//Save and load
	void saveTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	void saveTileRegion(TileMap* tm, int64_t rX, int64_t rY, int64_t rZ);
	bool loadTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	void buildLevelDebugTex(SDLHandler* sh, Texture& tex, int64_t rX, int64_t rY, int64_t rZ);
	/**/

protected:

private:
	/**/
	uint64_t magicNumberP1 = 0xaf0e66d7af0e66d7;
	uint64_t magicNumberP2 = 0xce529030ce529030;
	FileHandler* fileHandler = nullptr;
	std::string directory = "";
	
	static const uint8_t popMask;
	
    /**/
	//Get file path information
	std::string getLsrFilePathFromRxyz(int64_t rX, int64_t rY, int64_t rZ);
	//Get Header Entry information
	uint32_t getHeaderEntryDelta(int64_t rX, int64_t rY, int64_t rZ);
	uint32_t getHeaderStartPos();
	uint32_t getRegDataStartPos();
	
	uint32_t getHeaderEntryData1();
	uint8_t getHeaderEntryData2();
	uint8_t getHeaderEntryData3();
	//Get Save Section information
	uint32_t getSaveSectionSizeBytes(uint8_t bitsize);
	uint32_t getNewAllocationPos(uint8_t bitsize, int64_t rX, int64_t rY, int64_t rZ);
	/**/
	//Build base components
	void buildPrefixAndHeader();
	void buildHeaderEntry(int64_t rX, int64_t rY, int64_t rZ, uint32_t loc, uint8_t bpt, uint8_t pop);
	void savePrelims(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	//Clear un-needed data
	bool clearHeaderEntryPlusSaveSection(int64_t rX, int64_t rY, int64_t rZ);
	
	int xFromIndex(int index, int w);
	int yFromIndex(int index, int w);
};
