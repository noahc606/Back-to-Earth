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
	Tile getTile(nch::Vec3<int64_t> pos);
	Tile getTileByCsXYZ(Camera* cam, nch::Vec3<int64_t> cspos);
	TileEntity* getTileEntity(nch::Vec3<int64_t> pos);
	//Getting TileRegions given their position
	static TileRegion* getRegByRXYZ(t_regionMap* regMap, nch::Vec3<int64_t> rpos);
	TileRegion* getRegByXYZ(nch::Vec3<int64_t> pos);
	TileRegion* getRegByRXYZ(nch::Vec3<int64_t> rpos);
	TileRegion* getRegByCsRXYZ(Camera* cam, nch::Vec3<int64_t> csrpos);
	//Get position within region based on xyz
	static int64_t getRegSubPos(int64_t c);
	static void getRegSubPos(nch::Vec3<int64_t>& pos);
	//Get region coordinates based on xyz
	static int64_t getRegRXYZ(int64_t c);
	static void getRegRXYZ(nch::Vec3<int64_t>& rpos);
	//Get save-region coordinates based on xyz.
	static int64_t convRxyToLSRxy(int64_t rxOrRy);
	static int64_t convRzToLSRz(int64_t rz);
	static void convRxyzToLSRxyz(nch::Vec3<int64_t>& rpos);
	
	//Collision
	bool collides(nch::Box3<double> b, nch::Vec3<int64_t>& cspos);

	/** TileMap manipulation */
	//Set tile
	int setTile(nch::Vec3<int64_t> pos, Tile t);
	int setTileByCsXYZ(Camera* cam, nch::Vec3<int64_t> cspos, Tile t);
	static bool setTiles(t_regionMap* regionMap, nch::Vec3<int64_t> pos1, nch::Vec3<int64_t> pos2, Tile tt, bool natural);
	bool setTiles(nch::Vec3<int64_t> pos1, nch::Vec3<int64_t> pos2, Tile tt, bool natural);
	int setTileEntity(nch::Vec3<int64_t> pos, TileEntity* te);
	void setStructureWithinReg(Structure* stru, TileRegion& tr, nch::Vec3<int64_t> rpos);
	//Load regions
	int loadRegion(nch::Vec3<int64_t> rpos);
	int loadRegionByCsRXYZ(Camera* cam, nch::Vec3<int64_t> csrpos);
	int saveRegion(nch::Vec3<int64_t> rpos);
	int unloadRegion(nch::Vec3<int64_t> rpos);

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
