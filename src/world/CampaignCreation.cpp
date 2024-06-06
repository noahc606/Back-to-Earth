#include "CampaignCreation.h"
#include <iostream>
#include <set>
#include <sstream>
#include "Log.h"
#include "TextBox.h"

/*
    Takes the string 'ln' and returns a suitable name for the directory representing it.
    - Non-alphanumeric characters are turned into '_'s.
    - If a directory with the name exists, create a copy in the format <previous name>_<# of copy>.

    Returns on failure: an empty string.
    Returns on success: the final level name string.
*/
std::string CampaignCreation::validateLevelName(FileHandler* fh, std::string ln)
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

/*
    Creates a directory within "backtoearth/saved/games" which holds data for an individual save game (campaign).
    The campaign's settings are determined by the values of the GUIs within the current window.

    Returns true on success, false on failure.
*/
bool CampaignCreation::createCampaignDir(GUIHandler* gh, FileHandler* fh)
{
    //Check for campaign-create-new window. If not found, stop.
    if(gh->getGUI(BTEObject::GUI_window, GUIHandler::win_SELECT_CAMPAIGN_CN)==nullptr) {
        Log::warn(__PRETTY_FUNCTION__, "Couldn't find the window for creating campaigns");
        return false;
    }

    //Get level name textbox
    GUI* ptbx = gh->getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_SELECT_CAMPAIGN_CN_levelName);
    if(ptbx!=nullptr) {
        //Cast GUI to textbox
        TextBox* tbxLevelName = (TextBox*)ptbx;


        /* LAST STEP: check level name valid -> create directory for it */
        //Get level name entered, then try to validate it. If 'levelName' could be validated, create the directory.
        std::string levelName = validateLevelName(fh, tbxLevelName->getString());
        if(levelName!="") {
            Log::log("Creating new campaign with name \"%s\" within the directory: \"%s\".", tbxLevelName->getString().c_str(), levelName.c_str());
            fh->createBTEDir("saved/games/"+levelName);
        } else {
            Log::warn(__PRETTY_FUNCTION__, "Invalid level name entered: \"%s\"", tbxLevelName->getString().c_str());
            return false;
        }
    }

    return true;
}