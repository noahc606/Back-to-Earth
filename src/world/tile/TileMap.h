#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include "Box3X.h"
#include "BTEObject.h"
#include "Camera.h"
#include "Defs.h"
#include "FileHandler.h"
#include "Loggable.h"
#include "Planet.h"
#include "StructureMap.h"
#include "Texture.h"
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
	void init(SDLHandler* sh, FileHandler* fh, Planet* pt, StructureMap* struMap, std::string wdn, int64_t worldSeed);
	void destroy();
	/**/
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	t_regionMap* getRegionMap();
	Planet* getPlanet();
	std::string getWorldDirName();
	//Getting TileTypes given their position
	TileType getTile(int64_t x, int64_t y, int64_t z);
	TileType getTileByCsXYZ(Camera* cam, int64_t csX, int64_t csY, int64_t csZ);
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
	bool collides( Box3X<double> b, int64_t& cx, int64_t& cy, int64_t& cz );
	bool collides( Box3X<double> b );

	/** TileMap manipulation */
	//Set tile
	//int setTile(t_regionMap& regMap, int64_t x, int64_t y, int64_t z, TileType tt);
	int setTile(int64_t x, int64_t y, int64_t z, TileType tt);
	int setTileByCsXYZ(Camera* cam, int64_t cx, int64_t cy, int64_t cz, TileType tt);
	void setStructureWithinReg(Structure* stru, TileRegion& tr, int64_t rX, int64_t rY, int64_t rZ);
	//Load regions
	int loadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);
	int loadRegionByCsRXYZ(FileHandler* fileHandler, int camAxis, int64_t csRX, int64_t csRY, int64_t csRZ);
	int loadRegions(FileHandler* fileHandler, int64_t rX1, int64_t rY1, int64_t rZ1, int64_t rX2, int64_t rY2, int64_t rZ2);
	int forceLoadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);
	int saveRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);
	int unloadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);

protected:

private:
	//Maps
	t_regionMap regionMap;

	//World objects
	Planet* planet = nullptr;
	StructureMap* struMap = nullptr;
	std::string worldDirName = "world_unknown";
	int64_t worldSeed = 0;
};
