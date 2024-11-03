#pragma once
#include <string>
#include <nlohmann/json.hpp>

class Mission {
public:
    struct MissionRequirements {
        std::vector<std::pair<std::string, int>> items; //If .size()==0, this requirement doesn't count
        std::string poi = "null";                       //If null, this requirement doesn't count
    };

    Mission();
    Mission(nlohmann::json def);
    Mission(std::string parent, std::string id, std::string title, std::string desc, std::string longDesc, nlohmann::json reqs);
    ~Mission();

    nlohmann::json toJSON();

    /* Required properties */
    std::string parent = "null";
    std::string id = "null";
    std::string title = "null";
    std::string desc = "null";
    std::string longDesc = "null";
    std::vector<MissionRequirements> reqs;

    /* Optional properites */
    bool ez = false;
    bool global = false;
private:
};