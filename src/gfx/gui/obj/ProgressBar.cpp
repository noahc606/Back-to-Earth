#include "ProgressBar.h"
#include <nch/cpp-utils/io/Log.h>

ProgressBar::ProgressBar(Window* parentWindow, int x, int y, int width, int id)
: WindowComponent::WindowComponent(parentWindow, x, y)
{
    setType(BTEObject::Type::GUI_progressbar);
    setID(id);

    ProgressBar::parentWindow = parentWindow;
    ProgressBar::tX = x/2*2;
    ProgressBar::tY = y/2*2;
    ProgressBar::width = width/2*2;
}

void ProgressBar::init(SDLHandler* sh)
{
    BTEObject::init(sh, nullptr, nullptr);

    /* Build pbrTex */
    pbrTex.init(sdlHandler, width/2, 12, 2);
    //Left (left ridge)
    pbrTex.lock(0, 0, 2, 12);
    pbrTex.blit(TextureLoader::GUI_misc, 0, 0);
    //Middle (upper ridge, bar, and lower ridge)
    for(int i = 0; i<width/2; i += 32) {
        pbrTex.lock(2+i, 0, 32, 12);
        pbrTex.blit(TextureLoader::GUI_misc, 3, 0);
    }
    //Right (right ridge)
    pbrTex.lock(width/2-2, 0, 2, 12);
    pbrTex.blit(TextureLoader::GUI_misc, 36, 0);

    /* Build pbrTexOverlay */
    pbrTexOverlay.init(sdlHandler, width/2-4, 8, 2);

    onWindowUpdate();
}

void ProgressBar::initWorkTypeA(CurlHandler* ch, std::vector<std::string>* assetList, bool forceDisableUpdateDLs)
{
    ProgressBar::ch = ch;
    ProgressBar::assetList = assetList;
    ProgressBar::forceDisableUpdateDLs = forceDisableUpdateDLs;

    workType = 'a';
    workToDo = true;

    currentTask = 0;
    totalTasks = (*assetList).size();
}

void ProgressBar::destroy()
{
    BTEObject::destroy();
    pbrTex.destroy();
    pbrTexOverlay.destroy();
}

void ProgressBar::rebuildTexOverlay()
{
/* Rebuild pbrTexOverlay */
    //Set new tex width based on current progress
    double prog = ((double)currentTask)/(double)totalTasks;
    int texWidth = (double)(width/2-4)*prog;
    pbrTexOverlay.setTexWidth(texWidth);
    //Build newly sized pbrTexOverlay
    for(int i = 0; i<pbrTexOverlay.getTexWidth(); i += 32) {
        pbrTexOverlay.lock(i, 0, 32, 8);
        pbrTexOverlay.blit(TextureLoader::GUI_misc, 3, 13);
    }
}

void ProgressBar::draw()
{
    pbrTex.draw();

    if(workToDo) rebuildTexOverlay();
    pbrTexOverlay.draw();

    int percentage = (100*currentTask)/totalTasks;
    int doneTasks = currentTask;
    if(!workToDo) {
        percentage = 100;
        doneTasks = totalTasks;
    }

    //Build work descriptions
    std::stringstream ss1;
    ss1 << percentage << "% ";
    ss1 << "(" << doneTasks << "/" << totalTasks <<  ")";
    TextOld::draw(sdlHandler, ss1.str(), sX+2, sY-48, 2);
    TextOld::draw(sdlHandler, currentTaskDesc, sX+2, sY-24, 2);
}

void ProgressBar::tick()
{
    if(workToDo && currentTask>=totalTasks) {
        NCH_Log::log("Finished work.");
        workFinished = true;
        currentTask = -1;
        workToDo = false;
    }

    if(!workToDo) { return; }

    switch (workType) {
    case 'a': {
        if(ch==nullptr) break;
        if(assetList==nullptr) break;
        
        std::string s = (*assetList)[currentTask];
    
        if(!forceDisableUpdateDLs) {
            ch->cURLIntoFile(s, "https://noahc606.github.io/nch/bte/assets/"+s);
        }
        currentTaskDesc = s;
        NCH_Log::log("Downloaded file: %s", s.c_str());
    } break;
    default: {} break;
    }

    currentTask++;
}

void ProgressBar::onWindowUpdate()
{
    tX += 2;
    tY += 2;
    translateSPos();
    pbrTex.setDrawPos(sX, sY);
    pbrTexOverlay.setDrawPos(sX+4, sY+4);
}

void ProgressBar::resetWorkFinished() { workFinished = false; }

bool ProgressBar::hasWorkToDo() { return workToDo; }

bool ProgressBar::isWorkFinished() { return workFinished; }