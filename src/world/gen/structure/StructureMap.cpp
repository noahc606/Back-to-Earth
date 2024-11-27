#include "StructureMap.h"
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/timer.h>
#include "CollectionUtils.h"
#include "DebugScreen.h"
#include "Noise.h"
#include "TileMap.h"

const int64_t StructureMap::msrs = 4;

StructureMap::StructureMap(){}
StructureMap::~StructureMap()
{

}

void StructureMap::init(NoiseMap* nMap, TileDict* tDict, Camera* cam, int64_t loadDist)
{
    StructureMap::nMap = nMap;
    StructureMap::tDict = tDict;
    StructureMap::cam = cam;
    StructureMap::loadDist = loadDist;
    initted = true;

    updateCamStRXYZ();
    populateRegionsNear(camStRX, camStRY, camStRZ, loadDist);
}

void StructureMap::tick()
{
    if(
        getStRXYZ(cam->getRX())!=camStRX ||
        getStRXYZ(cam->getRY())!=camStRY ||
        getStRXYZ(cam->getRZ())!=camStRZ
    ) {
        updateCamStRXYZ();
        populateRegionsNear(camStRX, camStRY, camStRZ, loadDist);
    }
}

void StructureMap::drawDebugOverlay(Canvas* csInteractions)
{
    
}

void StructureMap::putInfo(std::stringstream& ss, int& tabs)
{
    if(!true) {
        DebugScreen::indentLine(ss, tabs);
        ss << "camStRXYZ=(" << camStRX << ", " << camStRY << ", " << camStRZ << "); ";
        DebugScreen::newLine(ss);
    }

    DebugScreen::indentLine(ss, tabs);
    ss << "populatedRegions.size()=" << populatedRegions.size() << "; ";
    ss << "structures.size()=" << structures.size() << "; ";
    ss << "lastPopTime=" << lastPopTime << "ms; ";
    DebugScreen::newLine(ss);

    DebugScreen::indentLine(ss, tabs);
    NoiseMap::RegHeightMap* rhm = nMap->cachedBaseTerrainHeightMapAt(cam->getRX(), cam->getRY());
    ss << "regAvgZ=" << rhm->avgHeight;
    DebugScreen::newLine(ss);
}

std::vector<Structure*> StructureMap::getStructuresInRXYZ(int64_t rX, int64_t rY, int64_t rZ)
{
    std::vector<Structure*> res;
    for(int i = 0; i<structures.size(); i++) {
        Structure* stru = &structures.at(i);

        Box3X<int64_t> regBox;
        regBox.c1 = Vec3X<int64_t>(rX*32, rY*32, rZ*32);
        regBox.c2 = Vec3X<int64_t>(rX*32+31, rY*32+31, rZ*32+31);

        if(stru->getBoundingBox().intersects(regBox)) {
            res.push_back(stru);
        }
    }

    return res;
}

int64_t StructureMap::getStRXYZ(int64_t rc) { rc = floor(rc/((double)msrs)); return rc; }

void StructureMap::populateRegionsNear(int64_t stRX, int64_t stRY, int64_t stRZ, int64_t loadDist)
{
    if(!initted) {
        return;
    }

    nch::Timer t("poptime", false);

    CollectionUtils cu;
    int64_t stRegDist = loadDist/msrs+1;

    //Iterate thru all stRegions within the stRegion load distance
    for(int64_t iStRX = stRX-stRegDist; iStRX<=stRX+stRegDist; iStRX++)
    for(int64_t iStRY = stRY-stRegDist; iStRY<=stRY+stRegDist; iStRY++)
    for(int64_t iStRZ = stRZ-stRegDist; iStRZ<=stRZ+stRegDist; iStRZ++) {
        //If this stRegion is unpopulated with structures...
        if( !cu.findInMap(populatedRegions, std::make_tuple(iStRX, iStRY, iStRZ)) ) {
            //Iterate thru all normal regions within this structure region (4x4x4)
            for(int64_t iDRX = 0; iDRX<msrs; iDRX++)
            for(int64_t iDRY = 0; iDRY<msrs; iDRY++)
            for(int64_t iDRZ = 0; iDRZ<msrs; iDRZ++) {
                int64_t rX = iStRX*4+iDRX, rY = iStRY*4+iDRY, rZ = iStRZ*4+iDRZ;

                //Get terrain heightmap of this (rX, rY) location.
                NoiseMap::RegHeightMap* rhm = nMap->cachedBaseTerrainHeightMapAt(rX, rY);
                
                //Specific regions
                populateSpecificRegions(rX, rY, rZ, rhm);
                populateStructuresPerRegion(rX, rY, rZ, rhm);
            }

            cu.insertInMap(populatedRegions, std::make_pair(std::make_tuple(iStRX, iStRY, iStRZ), true));
        }
    }

    //Track time spent populating
    lastPopTime = t.getElapsedTimeMS();
}

void StructureMap::populateSpecificRegions(int64_t rX, int64_t rY, int64_t rZ, NoiseMap::RegHeightMap* rhm) {
    int64_t x = rX*32, y = rY*32, z = rZ*32;
    
    int64_t avgTerrainZ = rhm->avgHeight;

    if(rX==0 && rY==0 && rZ==TileMap::getRegRXYZ(avgTerrainZ)) {
        Vec3X<int64_t> o(x, y, avgTerrainZ);
        nch::Log::log("Generated hab @ xyz(%d, %d, %d)", o.x, o.y, o.z);
        structures.pushBack(new Structure(Structure::HAB, o, tDict));
    }
}

void StructureMap::populateStructuresPerRegion(int64_t rX, int64_t rY, int64_t rZ, NoiseMap::RegHeightMap* rhm)
{
    int64_t x = rX*32, y = rY*32, z = rZ*32;

    //Monolith (1 in 1000)
    uint32_t miniseed = nMap->getNoise()->hash3ToUint32(rX, rY, rZ);
    srand(miniseed);
    if(rand()%1000==0) {        
        int64_t avgTerrainZ = rhm->avgHeight;
        if(rZ==TileMap::getRegRXYZ(avgTerrainZ)) {
            Structure::suppressNextWarning();
            Structure* mono = new Structure(Structure::MONOLITH, Vec3X<int64_t>(x, y, avgTerrainZ-9), tDict);
            structures.pushBack(mono);
        }
    }

    srand(rand());

    //Large (max 100x100x100 area) cave starting point (1 in 50)
    if(rand()%50==0) {
        int64_t avgTerrainZ = rhm->avgHeight;
        if(rZ>=TileMap::getRegRXYZ(avgTerrainZ)) {
            Structure* caveL = new Structure(Structure::CAVE_LARGE, Vec3X<int64_t>(x-50, y-50, z-50), tDict);
            structures.pushBack(caveL);
        }
    }
}

void StructureMap::updateCamStRXYZ()
{
    camStRX = getStRXYZ(cam->getRX());
    camStRY = getStRXYZ(cam->getRY());
    camStRZ = getStRXYZ(cam->getRZ());
}

