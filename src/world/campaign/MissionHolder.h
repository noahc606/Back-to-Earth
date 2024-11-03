#pragma once
#include <nlohmann/json.hpp>
#include "Mission.h"
#include "MissionTree.h"
#include "SDLHandler.h"

class MissionHolder {
public:


    MissionHolder();
    MissionHolder(std::string saveGameName);
    void init(std::string saveGameName);
    
    ~MissionHolder();

    void draw(SDLHandler* sh, int x, int y);

private:
    std::string saveGameName = "???null???";

    std::map<std::string, std::vector<Mission>> missions;   //map<[ID for mission tab], [list of missions within that tab]>
    MissionTree mt;
    
    void rebuildMainMissions();
    void loadMissions();
    
};