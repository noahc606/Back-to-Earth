#pragma once
#include <map>
#include <tuple>
#include "Canvas.h"
#include "Defs.h"
#include "Loggable.h"
#include "NoiseMap.h"
#include "Structure.h"
#include "TileDict.h"

class StructureMap : Loggable {
public:
    StructureMap();
    ~StructureMap();

    void init(NoiseMap* nMap, TileDict* tDict, Camera* cam, int64_t loadDist);
    void tick();
    void drawDebugOverlay(Canvas* csInteractions);
    void putInfo(std::stringstream& ss, int& tabs);

	std::vector<Structure*> getStructuresInRXYZ(int64_t rX, int64_t rY, int64_t rZ);

    static int64_t getStRXYZ(int64_t rc);

    static const int64_t msrs; //Maximum structure region size
private:
    void populateRegionsNear(int64_t rX, int64_t rY, int64_t rZ, int64_t loadDist);
    void populateSpecificRegions(int64_t rX, int64_t rY, int64_t rZ, NoiseMap::RegHeightMap* rhm);
    void populateStructuresPerRegion(int64_t rX, int64_t rY, int64_t rZ, NoiseMap::RegHeightMap* rhm);
    void populateCommonPerRegion(int64_t rX, int64_t rY, int64_t rZ, NoiseMap::RegHeightMap* rhm);
    void updateCamStRXYZ();

    int r();

    bool initted = false;
    NoiseMap* nMap = nullptr;
    TileDict* tDict = nullptr;
    int64_t loadDist = 0;
    nch::ArrayList<Structure>           structures;
    std::map<Defs::t_tripleI64, bool>   populatedRegions;
    
    Camera* cam = nullptr;
    int64_t camStRX = 0, camStRY = 0, camStRZ = 0;
    double lastPopTime = NAN;

    bool populationDisabled = true;
};