#pragma once
#include "Controls.h"
#include "GUIHandler.h"
#include "SDLHandler.h"
#include "World.h"

class ButtonAction
{
public:
    ButtonAction(SDLHandler* sh,  GUIHandler* gh, FileHandler* fh, Controls* ctrls);
    virtual ~ButtonAction();

protected:

private:
    void populateSettingUIInfo(GUIHandler* gh, int& currentWindowID, int& settingFileID, std::vector<std::pair<int, int>>& objIDs, std::vector<std::string>& objKeys);
    void saveSettingsBasedOnUIs(GUIHandler* gh, Settings* stgs, FileHandler* fh, Controls* ctrls);
};
