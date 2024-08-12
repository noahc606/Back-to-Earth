#pragma once
#include <map>
#include <tuple>
#include "Canvas.h"
#include "Defs.h"
#include "Loggable.h"
#include "Structure.h"


class StructureMap : Loggable {
public:
    void init(int64_t gSeed, Camera* cam, int64_t loadDist);
    void tick();
    void drawDebugOverlay(Canvas* csInteractions);
    void putInfo(std::stringstream& ss, int& tabs);

	std::vector<Structure*> getStructuresInRXYZ(int64_t rX, int64_t rY, int64_t rZ);

    static int64_t getStRXYZ(int64_t rc);

    static const int64_t msrs; //Maximum structure region size
private:
    void populateRegionsNear(int64_t rX, int64_t rY, int64_t rZ, int64_t loadDist);
    void updateCamStRXYZ();

    bool initted = false;
    int64_t gSeed = 0;
    int64_t loadDist = 0;
    std::vector<Structure*>             structures;
    std::map<Defs::t_tripleI64, bool>   populatedRegions;
    
    Camera* cam = nullptr;
    int64_t camStRX = 0, camStRY = 0, camStRZ = 0;
    double lastPopTime = NAN;
};