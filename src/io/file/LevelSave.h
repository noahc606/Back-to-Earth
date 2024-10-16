#pragma once
#include <nch/cpp-utils/arraylist.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include "DataStream.h"
#include "TileMap.h"
#include "TileRegion.h"
#include "TileType.h"
#include <string>

class LevelSave
{
public:
    /**/
    LevelSave(std::string dir);
    LevelSave();
	/**/
	static std::string getNatFilePathFromRxyz(std::string parentDir, int64_t rX, int64_t rY, int64_t rZ);
	static std::string getNatHeaderTriple(int64_t rX, int64_t rY, int64_t rZ);
	/**/
	//Save and load
	void saveTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	void saveTileRegion(TileMap* tm, int64_t rX, int64_t rY, int64_t rZ);
	bool loadTileRegion(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	/**/

protected:

private:
	void loadError(int16_t val, std::vector<int>& sxyz, std::vector<int64_t>& rXYZ, uint16_t aps, std::string lfp, TileRegion& tr);

	static nch::ArrayList<nch::NoahAllocTable> nats;
	std::string directory = "";
};
