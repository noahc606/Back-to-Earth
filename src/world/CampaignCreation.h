#pragma once
#include <string>
#include "GUIHandler.h"
#include "FileHandler.h"

class CampaignCreation {
public:
    static std::string validateLevelName(FileHandler* fh, std::string ln);
    static bool createCampaignDir(GUIHandler* gh, FileHandler* fh);
    
private:
};