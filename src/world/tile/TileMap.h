#pragma once
#include <map>
#include <nch/math-utils/box3.h>
#include <set>
#include <string>
#include <tuple>
#include "BTEObject.h"
#include "Camera.h"
#include "Defs.h"
#include "Loggable.h"
#include "NoiseMap.h"
#include "Planet.h"
#include "StructureMap.h"
#include "Texture.h"
#include "TileDict.h"
#include "TileRegion.h"

class TileMap : public BTEObject, public Loggable
{
public:
	/**/
	typedef int										t_regState;
	typedef std::tuple<char, char>					t_charP;
	typedef std::tuple<int64_t, int64_t>			t_v2d;
	typedef std::map<Defs::t_tripleI64, TileRegion>	t_regionMap;

	/**/
	void init(SDLHandler* sh, Planet* pt, StructureMap* struMap, NoiseMap* nMap, TileDict* td, std::string worldDirName);
	void destroy();
	/**/
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	t_regionMap* getRegionMap();
	NoiseMap::t_baseTerrainMap* getBaseTerrainMap();
	Planet* getPlanet();
	TileDict* getTileDict();

	//Getting TileTypes given their position
	Tile getTile(int64_t x, int64_t y, int64_t z);
	Tile getTileByCsXYZ(Camera* cam, nch::Vec3<int64_t> cspos);
	//Getting TileRegions given their position
	static TileRegion* getRegByRXYZ(t_regionMap* regMap, int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByXYZ (int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByRXYZ(int64_t rX, int64_t rY, int64_t rZ);
	TileRegion* getRegByCsRXYZ(Camera* cam, int64_t csRX, int64_t csRY, int64_t csRZ);
	//Get position within region based on xyz
	static int64_t getRegSubPos(int64_t c);
	static void getRegSubPos(int64_t& x, int64_t& y, int64_t& z);
	//Get region coordinates based on xyz
	static int64_t getRegRXYZ  (int64_t c);
	static void getRegRXYZ  (int64_t& x, int64_t& y, int64_t& z);
	//Get save-region coordinates based on xyz.
	static int64_t convRxyToLSRxy (int64_t rxOrRy);
	static int64_t convRzToLSRz (int64_t rz);
	static void convRxyzToLSRxyz (int64_t& rx, int64_t& ry, int64_t& rz);
	
	//Collision
	bool collides(nch::Box3<double> b, int64_t& cx, int64_t& cy, int64_t& cz);
	bool collides(nch::Box3<double> b);

	/** TileMap manipulation */
	//Set tile
	int setTile(int64_t x, int64_t y, int64_t z, Tile t);
	int setTileByCsXYZ(Camera* cam, int64_t cx, int64_t cy, int64_t cz, Tile t);
	static bool setTiles(t_regionMap* regionMap, int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2, Tile tt, bool natural);
	bool setTiles(int64_t x1, int64_t y1, int64_t z1, int64_t x2, int64_t y2, int64_t z2, Tile tt, bool natural);
	void setStructureWithinReg(Structure* stru, TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	//Load regions
	int loadRegion(int64_t rX, int64_t rY, int64_t rZ);
	int loadRegionByCsRXYZ(int camAxis, int64_t csRX, int64_t csRY, int64_t csRZ);
	int loadRegions(int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2);
	int saveRegion(int64_t rX, int64_t rY, int64_t rZ);
	int unloadRegion(int64_t rX, int64_t rY, int64_t rZ);

protected:

private:
	//Maps
	t_regionMap regionMap;
	NoiseMap* nMap;
	

	//World objects
	Planet* planet = nullptr;
	StructureMap* struMap = nullptr;
	TileDict* tileDict = nullptr;
	std::string saveGameName = "world_unknown";
};
