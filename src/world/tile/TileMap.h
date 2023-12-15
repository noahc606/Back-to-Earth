#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include "Box3D.h"
#include "BTEObject.h"
#include "FileHandler.h"
#include "Loggable.h"
#include "Player.h"
#include "Texture.h"
#include "TileRegion.h"

class TileMap : public BTEObject, public Loggable
{
public:
	/**/
	typedef int										t_regState;
	typedef std::tuple<char, char>					t_charP;
	typedef std::tuple<int64_t, int64_t>			t_v2d;
	typedef std::tuple<int64_t, int64_t, int64_t>	t_v3d;
	typedef std::set<std::pair<int, int>>			t_updates;
	typedef std::map<t_v3d, TileRegion>				t_regionMap;
	typedef std::map<t_v3d, t_updates>				t_updatesMap;
	/**/
	void destroy();
	/**/
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	t_regionMap* getRegionMap();
	t_updatesMap* getUpdatesMap();

	TileType getTile(int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByXYZ (int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByRXYZ(int64_t rX, int64_t rY, int64_t rZ);
	t_updates* getRTUsByRXYz(int64_t rX, int64_t rY, int64_t z);

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
	
	bool collides( Box3D &b );

	/** TileMap manipulation */
	//Set tile
	int setTile(int64_t x, int64_t y, int64_t z, TileType tt);
	//Load regions
	int loadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);
	int forceLoadRegion(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ);
	int saveRegion(FileHandler* fileHandler, std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ);
	int unloadRegion(FileHandler* fileHandler, std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ);
	//Add/Remove updates
	int addTileUpdate(int64_t x, int64_t y, int64_t layer);
	int addTileUpdates(int64_t x, int64_t y, int64_t layer);
	int addTileUpdates(int64_t x0, int64_t y0, int64_t layer0, int64_t x1, int64_t y1, int64_t layer1);
	int addRegionUpdate(int64_t rX, int64_t rY, int64_t layer);
	int stopRegionUpdate(int64_t rX, int64_t rY, int64_t layer);
	int stopAllUpdates();

protected:

private:
	//Maps
	t_regionMap regionMap;
	
	t_updatesMap updatesMap;

	std::vector<Player>* players;
};
