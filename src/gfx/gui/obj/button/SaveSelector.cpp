#include "SaveSelector.h"
#include "GUIHandler.h"
#include "Log.h"
#include "TextOld.h"

SaveSelector::SaveSelector(Window* parentWindow, int index, std::string dir, std::string sizeDesc, int id)
: Button::Button(parentWindow, 0, 0, 0, "", id)
{
    setSubType(BTEObject::Type::GUI_saveselect);

    SaveSelector::index = index;
    SaveSelector::worldDirName = dir;
    SaveSelector::worldDirPath = "saved/games/"+worldDirName;
    SaveSelector::worldSizeDesc = sizeDesc;

    width = 340*2;
    height = 72;
}

void SaveSelector::init(SDLHandler* sh, Controls* ctrls)
{
	GUI::init(sh, ctrls);

    std::vector<Texture*> ttb;  // TTB = (T)exes (T)o (B)uild
    ttb.push_back(&texBtn);
    ttb.push_back(&texBtnHovering);
    ttb.push_back(&texBtnSelected);

    int gss = TextureLoader::GUI_save_selector;
    for(int i = 0; i<ttb.size(); i++) {
        ttb[i]->init(sdlHandler, 340, 36, 2);
        ttb[i]->lock(  0, 0,   5, 36);  ttb[i]->blit(gss,   0, i*37,   5, 36);  // Left side
        ttb[i]->lock(  5, 0, 110, 36);  ttb[i]->blit(gss,   6, i*37, 110, 36);  // Middle 1
        ttb[i]->lock(115, 0, 110, 36);  ttb[i]->blit(gss,   6, i*37, 110, 36);  // Middle 2
        ttb[i]->lock(225, 0, 110, 36);  ttb[i]->blit(gss,   6, i*37, 110, 36);  // Middle 3
        ttb[i]->lock(335, 0,   5, 36);  ttb[i]->blit(gss, 117, i*37,   5, 36);  // Right side
    }
}

void SaveSelector::tick()
{
    bool alreadySelected = selected;
    Button::tick();

    if(clicked) {
        if(alreadySelected) {
            actionID = GUIHandler::ssr_SELECT_CAMPAIGN_select;
            actionInfo = std::make_pair(index, worldDirName);
        }
        clicked = false;
    }
}

void SaveSelector::onWindowUpdate()
{
    Button::onWindowUpdate();



    sX = parentWindow->getSX()+16;
    sY = parentWindow->getSY()+84+index*40*2;

    texBtn.setDrawPos(sX, sY);
    texBtnHovering.setDrawPos(sX, sY);
    texBtnSelected.setDrawPos(sX, sY);
}

void SaveSelector::draw()
{
    // Button appearance
    bool extraInfo = false;
    if(selected) {
        texBtnSelected.draw();
        extraInfo = true;
    } else if(hovering) {
        texBtnHovering.draw();
        extraInfo = true;
    } else {
        texBtn.draw();
    }

    // Extra info text
    if(extraInfo) {
        TextOld::draw(sdlHandler, worldDirName, sX+24, sY+20, 3);
        TextOld::draw(sdlHandler, worldDirPath+": "+worldSizeDesc, sX+22, sY+54, 1);
    } else {
        TextOld::draw(sdlHandler, worldDirName, sX+24, sY+28, 3);
    }
}

int SaveSelector::getActionID() { return actionID; }
std::pair<int, std::string> SaveSelector::getActionInfo() { return actionInfo; }

void SaveSelector::resetActionID(std::string methodName)
{
    if(actionID!=-1) {
        actionID = -1;
        actionInfo = std::make_pair(-1, "");
    } else {
        Log::warn(methodName, "Tried to reset SaveSelector's action which was already reset");
    }
}