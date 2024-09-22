#include "StructureMap.h"
#include <nch/cpp-utils/io/Log.h>
#include <nch/sdl-utils/Timer.h>
#include "CollectionUtils.h"
#include "DebugScreen.h"
#include "Noise.h"
#include "TileMap.h"

const int64_t StructureMap::msrs = 4;

void StructureMap::init(NoiseMap* nMap, Camera* cam, int64_t loadDist)
{
    StructureMap::nMap = nMap;
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
    ss << "regAvgZ=" << rhm->avgHeight << "\n";
    DebugScreen::newLine(ss);
}

std::vector<Structure*> StructureMap::getStructuresInRXYZ(int64_t rX, int64_t rY, int64_t rZ)
{
    std::vector<Structure*> res;
    for(Structure* stru : structures) {
        Box3X<int64_t> regBox;
        regBox.c1 = Point3X<int64_t>(rX*32, rY*32, rZ*32);
        regBox.c2 = Point3X<int64_t>(rX*32+31, rY*32+31, rZ*32+31);

        if(stru->getBounds().intersects(regBox)) {
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
                int64_t x = rX*32, y = rY*32, z = rZ*32;

                //Get terrain heightmap of this (rX, rY) location.
                NoiseMap::RegHeightMap* rhm = nMap->cachedBaseTerrainHeightMapAt(rX, rY);
                
                //Specific regions
                populateSpecificRegions(rX, rY, rZ, rhm);

                //Monolith (1 in 100)
                uint32_t miniseed = nMap->getNoise()->hash3ToUint32(rX, rY, rZ);
                srand(miniseed);
                if(rand()%100==0) {        
                    int64_t avgTerrainZ = rhm->avgHeight;
                    if(rZ==TileMap::getRegRXYZ(avgTerrainZ)) {
                        structures.push_back(new Structure(Structure::MONOLITH, Point3X<int64_t>(x, y, avgTerrainZ-9)));
                    }
                }
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
        nch::Log::log("Generated ship @ xyz(%d, %d, %d)", x, y, avgTerrainZ);
        Point3X<int64_t> shipOrigin(x, y, avgTerrainZ);
        structures.push_back(new Structure(Structure::CRASHED_SHIP, shipOrigin));
    }
}

void StructureMap::updateCamStRXYZ()
{
    camStRX = getStRXYZ(cam->getRX());
    camStRY = getStRXYZ(cam->getRY());
    camStRZ = getStRXYZ(cam->getRZ());
}

