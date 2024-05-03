#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include "Box3D.h"
#include "BTEObject.h"
#include "Camera.h"
#include "FileHandler.h"
#include "Loggable.h"
#include "Planet.h"
#include "Player.h"
#include "Texture.h"
#include "TileRegion.h"

class TileMap : public BTEObject, public Loggable
{
public:
	/**/
	typedef int													t_regState;
	typedef std::tuple<char, char>								t_charP;
	typedef std::tuple<int64_t, int64_t>						t_v2d;
	typedef std::tuple<int64_t, int64_t, int64_t>				t_v3d;
	typedef std::map<t_v3d, TileRegion>							t_regionMap;
	/**/
	void init(SDLHandler* sh, FileHandler* fh, Planet* pt, std::string wdn);
	void destroy();
	/**/
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	t_regionMap* getRegionMap();
	Planet* getPlanet();
	std::string getWorldDirName();

	TileType getTile(int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByXYZ (int64_t x, int64_t y, int64_t z);
	TileRegion* getRegByRXYZ(int64_t rX, int64_t rY, int64_t rZ);
	TileRegion* getRegByScrRXYZ(Camera* cam, int64_t scrRX, int64_t scrRY, int64_t scrRZ);

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
	std::vector<Player>* players;
	std::string worldDirName = "world_unknown";
};
