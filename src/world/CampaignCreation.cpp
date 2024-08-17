#include "CampaignCreation.h"
#include <iostream>
#include <nch/cpp-utils/io/Log.h>
#include <set>
#include <sstream>
#include "Noise.h"
#include "RadioButton.h"
#include "TextBox.h"

/*
    Creates a directory within "backtoearth/saved/games" which holds data for an individual world (campaign).
    The campaign's settings are determined by the values of the GUIs within the current window.

    Returns true on success, false on failure.
*/
bool CampaignCreation::createWorldDir(GUIHandler* gh, FileHandler* fh)
{
    //Check for campaign-create-new window. If not found, stop.
    if(gh->getGUI(BTEObject::GUI_window, GUIHandler::win_SELECT_CAMPAIGN_CN)==nullptr) {
        NCH_Log::warn(__PRETTY_FUNCTION__, "Couldn't find the window for creating campaigns");
        return false;
    }

    //Get world name textbox
    std::string worldName = "";
    TextBox* tbxLevelName = nullptr;
    GUI* ptbx = gh->getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_SELECT_CAMPAIGN_CN_levelName);
    if(ptbx!=nullptr) {
        //Cast GUI to textbox
        tbxLevelName = (TextBox*)ptbx;

        /* LAST STEP: check world name valid -> create directory for it */
        //Get world name entered, then try to validate it. If 'world' could be validated, create the directory.
        worldName = validateWorldName(fh, tbxLevelName->getString());
        if(worldName=="") {
            NCH_Log::warn(__PRETTY_FUNCTION__, "Invalid level name entered: \"%s\"", tbxLevelName->getString().c_str());
            tbxLevelName->errorFlash();
            return false;
        }
    } else {
        return false;
    }

    //Get gamemode mode
    std::string gamemode = "";
    { RadioButton* rb = gh->getRadioButton(gh->rbtn_SELECT_CAMPAIGN_CN_gameMode0); if(rb!=nullptr && rb->isSelected()) gamemode = "sandbox"; }
    { RadioButton* rb = gh->getRadioButton(gh->rbtn_SELECT_CAMPAIGN_CN_gameMode1); if(rb!=nullptr && rb->isSelected()) gamemode = "survival"; }
    { RadioButton* rb = gh->getRadioButton(gh->rbtn_SELECT_CAMPAIGN_CN_gameMode2); if(rb!=nullptr && rb->isSelected()) gamemode = "hardcore"; }
    if(gamemode=="") {
        NCH_Log::warn(__PRETTY_FUNCTION__, "Failed to get gamemode");
        return false;
    }

    //Get world seed
    int64_t worldSeed = -1;
    TextBox* tbxWorldSeed = nullptr;
    GUI* ptbx2 = gh->getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_SELECT_CAMPAIGN_CN_worldSeed);
    if(ptbx2!=nullptr) {
        //Cast GUI to textbox
        tbxWorldSeed = (TextBox*)ptbx2;
        try {
            worldSeed = std::stoll(tbxWorldSeed->getString());
        } catch(...) {
            worldSeed = Noise::stringSeedToI64Seed(tbxWorldSeed->getString());
        }
    } else {
        return false;
    }

    NCH_Log::log("Creating new campaign with name \"%s\", seed %d within the directory: \"%s\".", tbxLevelName->getString().c_str(), worldSeed, worldName.c_str());
    createWorldData(gh, fh, worldName, tbxLevelName->getString(), worldSeed, gamemode);
    return true;
}

/*
    Takes the string 'ln' and returns a suitable name for the directory representing it.
    - Non-alphanumeric characters are turned into '_'s.
    - If a directory with the name exists, create a copy in this format: "<previous name>_<# of copy>".

    Returns on failure: an empty string.
    Returns on success: the final world name string which may or may not be a transformed version of 'ln'.
*/
std::string CampaignCreation::validateWorldName(FileHandler* fh, std::string ln)
{
    //List of characters to not turn into an underscore.
    std::string plainChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-";
    std::string digs = "0123456789";

    //Find 'validLevelName', which is 'ln' but with all non-plainChars turned into _.
    std::stringstream validLNSS;
    for(int i = 0; i<ln.size(); i++) {
        if(plainChars.find(ln[i])!=std::string::npos) {
            validLNSS << ln[i];
        } else {
            validLNSS << "_";
        }
    }
    std::string validLevelName = validLNSS.str();

    //If valid level name is empty, fail
    if(validLevelName=="") {
        return "";
    }

    //Try to find the finalLevelName (if needed, increment the numbers in " - ####" until a new copy name is found).
    std::string finalLevelName = validLevelName;
    while(fh->dirExists("saved/games/"+finalLevelName)) {
        bool flnHasNum = false;
        std::string numstr = "";
        int copyNum = 0;
        bool foundDigs = false;
        for(int i = finalLevelName.size()-1; i>0; i--) {
            //See whether finalLevelName is itself a copy already
            if( digs.find(finalLevelName[i])!=std::string::npos ) {
                foundDigs = true;
            } else {
                //If we have find a "_" behind a set of digits
                if(foundDigs && finalLevelName.substr(i, 1)=="_") {
                    flnHasNum =true;
                    numstr = finalLevelName.substr(i+1);
                    finalLevelName = finalLevelName.substr(0, i);
                    break;
                } else {
                    flnHasNum = false;
                    break;
                }
            }
        }

        //If finalLevelName is itself a copy...
        if(flnHasNum) {
            try {
                copyNum = std::stoi(numstr);
                std::stringstream ss;
                ss << finalLevelName << "_" << (copyNum+1);
                finalLevelName = ss.str();
            } catch(...) {}
        //If not ...
        } else {
            finalLevelName = finalLevelName+"_2";
        }
    }

    //Return finalLevelName
    return finalLevelName;
}

bool CampaignCreation::createWorldData(GUIHandler* gh, FileHandler* fh, std::string validatedWorldDirName, std::string worldDisplayName, int64_t worldSeed, std::string gamemode)
{
    //Create directory
    std::string path = "saved/games/"+validatedWorldDirName+"/";
    fh->createBTEDir(path);

    //Create data file
    fh->cEditFile(path+"data.txt");


    std::stringstream ssws; ssws << worldSeed;
    fh->write("worldSeed=");        fh->write(ssws.str());          fh->writeln();
    fh->write("worldName=");        fh->write(worldDisplayName);    fh->writeln();
    fh->write("playerGameMode=");   fh->write(gamemode);            fh->writeln();
    fh->saveCloseFile();

    return true;
}