#pragma once
#include <nch/cpp-utils/arraylist.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include "DataStream.h"
#include "TileMap.h"
#include "TileRegion.h"
#include <string>

class LevelSave
{
public:
    /**/
    LevelSave(std::string saveGameDir, TileDict* td);
	/**/
	std::string getNatLSRPathFromRxyz(int64_t rX, int64_t rY, int64_t rZ);
	std::string getNatPOIPathFromRxyz(int64_t rX, int64_t rY, int64_t rZ);
	static std::string getNatHeaderTriple(int64_t rX, int64_t rY, int64_t rZ);
	/**/
	//Save and load
	void saveRegionTiles(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	void saveRegionTileEntities(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	bool loadRegionTiles(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	bool loadRegionTileEntities(TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	/**/

protected:

private:
	void loadError(int16_t val, std::vector<int>& sxyz, std::vector<int64_t>& rXYZ, uint16_t aps, std::string lfp, TileRegion& tr);

	static nch::ArrayList<nch::NoahAllocTable> nats;
	std::string tilesDir = "";
	std::string poiDir = "";
	TileDict* td;
};
