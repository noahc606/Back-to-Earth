#pragma once
#include "TileMap.h"
#include "Canvas.h"

class ScreenUpdater : public Loggable
{
public:
	typedef std::set<std::pair<int64_t, int64_t>>				t_updates;
	typedef std::set<std::pair<int64_t, int64_t>>				t_regUpdates;	//Set of (rX, rY) regions
	typedef std::map<std::pair<int64_t, int64_t>, t_updates>	t_tileUpdates;	//Map of regions, each having a set of (x,y) points

    void init(SDLHandler* sh, TileMap* tm, Canvas* cs);

	void putInfo(std::stringstream& ss, int& tabs);

	TileMap* getTileMap();
	t_regUpdates* getRegUpdates();
	t_tileUpdates* getTileUpdates();
	t_updates* getTUsByRXY(int64_t csRX, int64_t csRY);

	//Add/Remove updates
	int addTileUpdate(int64_t csX, int64_t csY);
	int addTileUpdates(int64_t csX, int64_t csY);
	int addTileUpdates(int64_t csX0, int64_t csY0, int64_t csX1, int64_t csY1);
	int addRegionUpdate(int64_t csRX, int64_t csRY);
	int stopRegionUpdate(int64_t csRX, int64_t csRY);
	void stopAllUpdates();

	//Control updates on a map-wide scale
    void updateMapVisible(bool blackout, int loadDist);
	void updateMapVisible(int loadDist);
	void regPillarGenAttempt(FileHandler* fileHandler, int64_t csRX, int64_t csRY, int64_t csRZ, int loadDepth);
	void updateMapToFINISHED_GENERATING(FileHandler* fileHandler, int loadDist);
	void updateMapToSHOULD_UPDATE(int loadDist);
	void updateMapToUPDATED(int loadDist);
	void updateMapMoved(FileHandler* fileHandler, int loadDist);

private:
	t_tileUpdates tileUpdates;
	t_regUpdates regUpdates;

	SDLHandler* sdlHandler = nullptr;
    TileMap* tileMap = nullptr;
    Camera* cam = nullptr;
	Canvas* csTileMap = nullptr;

	int loadCount = 0;
    int loadCountMax = 10;
	double allowedLoadTimeMS = 1.0;


    // Info: region load counting
    int infoRegLoadDivisor = 20;
    int infoRegLoadCount = 0;
    // Info: region load time
    double infoRegLoadTime = 0.0;
    double infoRegLoadTimeLatest = 0.0;
    double infoRegLoadTimeAvg = 0.0;
};