#include "ProgressBar.h"
#include "Log.h"

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

void ProgressBar::destroy()
{
    BTEObject::destroy();

    pbrTex.destroy();
    pbrTexOverlay.destroy();
}

void ProgressBar::draw()
{
    if(numTasksDoneLast!=numTasksDone) {
        numTasksDoneLast = numTasksDone;



        
        //pbrTex.lock()
    }

    pbrTex.draw();
    pbrTexOverlay.draw();
}

void ProgressBar::tick()
{

}

void ProgressBar::onWindowUpdate()
{
    tX += 2;
    tY += 2;
    translateSPos();
    pbrTex.setDrawPos(sX, sY);
    pbrTexOverlay.setDrawPos(sX+4, sY+4);
}

void ProgressBar::setProgress(int numTasksDone)
{
    if(numTasksDone>numTasksTotal) {
        numTasksDone = numTasksTotal;
    }
    ProgressBar::numTasksDone = numTasksDone;
}
void ProgressBar::setProgressTotal(int numTasksTotal) { ProgressBar::numTasksTotal = numTasksTotal; }