#pragma once
#include "TileMap.h"

class TileMapUpdater : public Loggable
{
public:
	typedef std::set<std::pair<int64_t, int64_t>>				t_updates;
	typedef std::set<std::pair<int64_t, int64_t>>				t_regUpdates;	//Set of (rX, rY) regions
	typedef std::map<std::pair<int64_t, int64_t>, t_updates>	t_tileUpdates;	//Map of regions, each having a set of (x,y) points

    void init(SDLHandler* sh, TileMap* tm, Camera* cam);

	void putInfo(std::stringstream& ss, int& tabs);

	TileMap* getTileMap();
	t_regUpdates* getRegUpdates();
	t_tileUpdates* getTileUpdates();
	t_updates* getTUsByRXY(int64_t rX, int64_t rY);

	//Add/Remove updates
	int addTileUpdate(int64_t x, int64_t y);
	int addTileUpdates(int64_t x, int64_t y);
	int addTileUpdates(int64_t x0, int64_t y0, int64_t x1, int64_t y1);
	int addRegionUpdate(int64_t rX, int64_t rY);
	int stopRegionUpdate(int64_t rX, int64_t rY);
	void stopAllUpdates();

	//Control updates on a map-wide scale
    void updateMapVisible(bool blackout, int loadDistH, int loadDistV);
	void updateMapVisible(int loadDistH, int loadDistV);
	void updateMapShouldUpdate(int loadDistH, int loadDistV);
	void updateRegTicked(FileHandler* fileHandler, int64_t rX, int64_t rY, int64_t rZ, int loadDistV);
	void updateMapTicked(FileHandler* fileHandler, int loadDistH, int loadDistV);
	void updateMapMoved(FileHandler* fileHandler, std::string currentDimPath, int loadDistH, int loadDistV);
	void updateMapIdle(int loadDistH, int loadDistV);
	void updateMapAutosave(FileHandler* fileHandler, std::string currentDimPath, int loadDistH, int loadDistV);

private:
	t_tileUpdates tileUpdates;
	t_regUpdates regUpdates;

	SDLHandler* sdlHandler = nullptr;
    TileMap* tileMap = nullptr;
    Camera* cam = nullptr;



    int loadCountMax = 1;


    // Info: region load counting
    int infoRegLoadDivisor = 20;
    int infoRegLoadCount = 0;
    // Info: region load time
    double infoRegLoadTime = 0.0;
    double infoRegLoadTimeLatest = 0.0;
    double infoRegLoadTimeAvg = 0.0;
};