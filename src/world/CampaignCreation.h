#pragma once
#include <string>
#include "GUIHandler.h"
#include "FileHandler.h"

class CampaignCreation {
public:
    static bool createWorldDir(GUIHandler* gh, FileHandler* fh);
    
private:
    static std::string validateWorldName(FileHandler* fh, std::string ln);
    static bool createWorldData(GUIHandler* gh, FileHandler* fh, std::string validatedWorldName, std::string worldDisplayName);
};