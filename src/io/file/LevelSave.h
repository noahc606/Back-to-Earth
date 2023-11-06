#pragma once
#include "DataStream.h"
#include "Defs.h"
#include "FileHandler.h"
#include "TileRegion.h"
#include "TileType.h"
#include <string>

class LevelSave
{
public:
    /**/
    LevelSave();
	/**/
	/**/
	//Save and load
	void saveTileRegion(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
	bool loadTileRegion(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
	/**/

protected:

private:
	/**/
	uint64_t magicNumberP1 = 0xaf0e66d7af0e66d7;
	uint64_t magicNumberP2 = 0xce529030ce529030;
    /**/
	//Get file path information
	std::string getLsrFilePathFromRxyz(Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
	//Get Header Entry information
	uint32_t getHeaderEntryDelta(Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
	uint32_t getHeaderEntryData1(FileHandler* fh);
	uint8_t getHeaderEntryData2(FileHandler* fh);
	uint8_t getHeaderEntryData3(FileHandler* fh);
	//Get Save Section information
	uint32_t getSaveSectionSizeBytes(uint8_t bitsize);
	uint32_t getNewAllocationPos(FileHandler* fh, uint8_t bitsize);
	/**/
	//Build base components
	void buildPrefixAndHeader(FileHandler* fh);
	void buildHeaderEntry(FileHandler* fh, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ, uint32_t loc, uint8_t bpt, uint8_t pop);
	void savePrelims(FileHandler* fh, TileRegion& tr, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
	//Clear un-needed data
	void clearHeaderEntryPlusSaveSection(FileHandler* fh, Defs::t_ll rX, Defs::t_ll rY, Defs::t_ll rZ);
};
