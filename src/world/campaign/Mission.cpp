#include "Mission.h"

Mission::Mission(nlohmann::json def)
{
    try { parent = def["parent"]; } catch(...) {}
    try { id = def["id"]; } catch(...) {}
    try { title = def["title"]; } catch(...) {}
    try { desc = def["desc"]; } catch(...) {}
    try { longDesc = def["longDesc"]; } catch(...) {}
    
    try { ez = def["ez"]; } catch(...) {}
}

Mission::Mission(std::string parent, std::string id, std::string title, std::string desc, std::string longDesc, nlohmann::json req)
{
    Mission::parent = parent;
    Mission::id = id;
    Mission::title = title;
    Mission::desc = desc;
    Mission::longDesc = longDesc;

    reqs.clear();
    MissionRequirements mr;
    try { mr.items = req["items"]; } catch(...) {}
}

Mission::~Mission(){}

nlohmann::json Mission::toJSON()
{
    nlohmann::json res;
    res["parent"] = parent;
    res["id"] = id;
    res["title"] = title;
    res["desc"] = desc;
    res["longDesc"] = longDesc;


    res["ez"] = ez;
    res["global"] = global;
    return res;
}