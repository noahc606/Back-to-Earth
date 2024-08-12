#include "StructureMap.h"
#include "CollectionUtils.h"
#include "DebugScreen.h"
#include "Log.h"
#include "Noise.h"
#include "Timer.h"

const int64_t StructureMap::msrs = 4;

void StructureMap::init(int64_t gSeed, Camera* cam, int64_t loadDist)
{
    StructureMap::gSeed = gSeed;
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
}

std::vector<Structure*> StructureMap::getStructuresInRXYZ(int64_t rX, int64_t rY, int64_t rZ)
{
    std::vector<Structure*> res;
    for(Structure* stru : structures) {
        Box3X<int64_t> regBox;
        regBox.c1 = Point3X(rX*32, rY*32, rZ*32);
        regBox.c2 = Point3X(rX*32+31, rY*32+31, rZ*32+31);

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

    Timer t("poptime", false);

    Noise n(gSeed);
    CollectionUtils cu;
    int64_t stRegDist = loadDist/msrs+1;
    for(int64_t iStRX = stRX-stRegDist; iStRX<=stRX+stRegDist; iStRX++)
    for(int64_t iStRY = stRY-stRegDist; iStRY<=stRY+stRegDist; iStRY++)
    for(int64_t iStRZ = stRZ-stRegDist; iStRZ<=stRZ+stRegDist; iStRZ++) {
        //If this region is unpopulated with structures
        if( cu.findInMap(populatedRegions, std::make_tuple(iStRX, iStRY, iStRZ))==false ) {
            for(int64_t iDRX = 0; iDRX<msrs; iDRX++)
            for(int64_t iDRY = 0; iDRY<msrs; iDRY++)
            for(int64_t iDRZ = 0; iDRZ<msrs; iDRZ++) {
                int64_t rX = iStRX*4+iDRX, rY = iStRY*4+iDRY, rZ = iStRZ*4+iDRZ;
                int64_t x = rX*32, y = rY*32, z = rZ*32;

                uint32_t miniseed = n.hash3ToUint32(rX, rY, rZ);
                srand(miniseed);
                if(std::abs(rand())%1==0) {
                    Log::log("Generated structure @ (%d, %d, %d)", rX, rY, rZ);
                    Point3X<int64_t> origin(x, y, z);
                    structures.push_back( new Structure(0, origin) );
                }
            }

            cu.insertInMap(populatedRegions, std::make_pair(std::make_tuple(iStRX, iStRY, iStRZ), true));
        }
    }

    lastPopTime = t.getElapsedTimeMS();
}

void StructureMap::updateCamStRXYZ()
{
    camStRX = getStRXYZ(cam->getRX());
    camStRY = getStRXYZ(cam->getRY());
    camStRZ = getStRXYZ(cam->getRZ());
}

