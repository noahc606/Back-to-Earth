#include "MissionHolder.h"
#include <nch/cpp-utils/filepath.h>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <fstream>
#include "Box3X.h"

MissionHolder::MissionHolder(){}
MissionHolder::MissionHolder(std::string saveGameName) { init(saveGameName); }
void MissionHolder::init(std::string saveGameName)
{
    MissionHolder::saveGameName = saveGameName;
    rebuildMainMissions();
    loadMissions();
    mt.build(missions, "crash_landing");
}
MissionHolder::~MissionHolder(){}

void MissionHolder::draw(SDLHandler* sh, int x, int y)
{
    mt.draw(sh, x, y);
}

void MissionHolder::rebuildMainMissions()
{
    std::string jsonPathPrefix = "backtoearth/saved/games/"+saveGameName+"/missions/";

    std::vector<std::string> mainTabs = { "crash_landing", "sustenance", "material_processing", "exploration" };
    for(int i = 0; i<mainTabs.size(); i++) {
        nlohmann::json tabJSON = R"({ "missions": [] })"_json;
        nlohmann::json* tjm = &tabJSON["missions"];
        bool foundTab = false;
        if(mainTabs[i]=="crash_landing") {
            foundTab = true;
            tjm->push_back(Mission("ROOT", "escape_pod_supplies", "Escape Pod Supplies", "Collect what you can from the escape pod.", "", R"({ "items": [ ["water_canister", 12], ["food_a", 5] ] })"_json).toJSON());
            tjm->push_back(Mission("escape_pod_supplies", "enter_hab", "Enter the Hab", "Step into the pressurized Accrio Emergency Habitat.", "", R"({ "poi": "hab_entrance" })"_json).toJSON());
            tjm->push_back(Mission("enter_hab", "explore_hab", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("explore_hab", "find_seed_vault", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("explore_hab", "find_water_supply", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("explore_hab", "find_emergency_rations_supply", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("enter_hab", "#3", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#3", "#4", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#3", "#4b", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#3", "#4c", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#3", "#4d", "", "", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("#4c", "x", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4c", "y", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5b", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5c", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5d", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5e", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5f", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5g", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5h", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5i", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5j", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5k", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5l", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5m", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#4", "#5n", "", "", "", R"({})"_json).toJSON());
            
            tjm->push_back(Mission("#5e", "#6", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#6", "#7", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#7", "#8", "", "", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("#8", "#9", "", "", "", R"({})"_json).toJSON());
            
            
        } else
        if(mainTabs[i]=="sustenance") {
            foundTab = true;

        } else
        if(mainTabs[i]=="material_processing") {
            foundTab = true;
            tjm->push_back(Mission("ROOT", "material_processing", "Material processing", "Process minerals and other substances into more useful materials.", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_copper", "Native Copper", "Find and crush Malachite/Tetrahedrite into Native Copper", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_copper", "process_native_copper", "Processed Copper", "Process Native Copper into Copper Ingots", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("material_processing", "obtain_native_tin", "Native Tin", "Find and crush Cassiterite into Native Tin", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_tin", "process_native_tin", "Processed Tin", "Process Native Tin into Tin Ingots", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("material_processing", "obtain_native_iron", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron", "process_native_iron", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron0", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron0", "process_native_iron0", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron1", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron1", "process_native_iron1", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron2", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron2", "process_native_iron2", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron3", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron3", "process_native_iron3", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron4", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron4", "process_native_iron4", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());

            tjm->push_back(Mission("material_processing", "obtain_native_iron5", "Native Iron", "Find and crush Crimsite/Limonite into Native Iron", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_native_iron5", "process_native_iron5", "Processed Iron", "Process Native Iron into Iron Ingots", "", R"({})"_json).toJSON());


            tjm->push_back(Mission("material_processing", "obtain_impure_so2_powder", "Impure Silicon Dioxide Powder", "Use a Mineral Crusher to crush rocks into Impure Silicon Dioxide Powder.", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("obtain_impure_so2_powder", "process_impure_so2_powder", "SO2", "Process Impure Silicon Dioxide Powder into Pure Silicon Dioxide Powder", "", R"({})"_json).toJSON());
            tjm->push_back(Mission("process_impure_so2_powder", "process_pure_so2_powder", "SO2", "Use an Arc Furnace to process Pure Silicon Dioxide Powder into Silicon Ingots.", "", R"({})"_json).toJSON());


        } else
        if(mainTabs[i]=="exploration") {
            foundTab = true;
            tjm->push_back(Mission("ROOT", "missing_supply_pods", "Missing Supply Pods", "Find the 12 missing supply pods on Accrio.", "", R"({})"_json).toJSON());
        }

        //Save this JSON file
        if(foundTab) {
            std::ofstream file(jsonPathPrefix+mainTabs[i]+".json");
            file << tabJSON;
        }
    }
}

void MissionHolder::loadMissions()
{
    missions.clear();

    /* Get set of 'missionFiles' and 'tabIDs' */
    nch::FsUtils fsu;
    nch::FsUtils::ListSettings lise;
    lise.includeDirs = false;
    std::vector<std::string> missionFiles = fsu.getDirContents("backtoearth/saved/games/"+saveGameName+"/missions", lise);
    std::vector<std::string> tabIDs;
    for(int i = 0; i<missionFiles.size(); i++) {
        nch::FilePath fp(missionFiles[i]);
        tabIDs.push_back(fp.getObjectName(false));
    }

    /* Load each mission file */
    for(int i = 0; i<missionFiles.size(); i++) {
        //Get JSON from this specific file
        std::ifstream jsonstream(missionFiles[i]);
        nlohmann::json tabList = nlohmann::json::parse(jsonstream)["missions"];
        //Get this JSON's data, put it into an array
        std::vector<Mission> tabMissions;
        for(int j = 0; j<tabList.size(); j++) {
            Mission m(tabList[j]);
            tabMissions.push_back(m);
        }
        //Insert this data into 'missions'
        missions.insert(std::make_pair(tabIDs[i], tabMissions));
    }
}

