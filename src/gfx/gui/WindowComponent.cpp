#include "WindowComponent.h"
#include "Log.h"
/**/

WindowComponent::WindowComponent(){}
WindowComponent::WindowComponent(Window* p_parentWindow, int p_x, int p_y, int p_width)
{
    //Take in p_parentWindow
    parentWindow = p_parentWindow;

    //Set alignments
    if( p_x<-1000 ) { horAlignment = p_x; }
    if( p_y<-1000 ) { verAlignment = p_y; }

    //Take in p_x, p_w
    tX = p_x/2*2;
    tY = p_y/2*2;

    //Take in p_w
    width = (p_width/64*64);
    if(p_width<8) width = 8;

    height = 16;
}
WindowComponent::WindowComponent(Window* p_parentWindow, int p_x, int p_y)
: WindowComponent::WindowComponent(p_parentWindow, p_x, p_y, 0)
{}

WindowComponent::~WindowComponent(){}

/**/

void WindowComponent::draw(){}
void WindowComponent::tick(){}

/**/

int WindowComponent::getTX() { return tX; }
int WindowComponent::getTY() { return tY; }
int WindowComponent::getTPos(bool dim) { if(dim==0) return tX; return tY; }

Window* WindowComponent::getParentWindow() { return parentWindow; };

void WindowComponent::setPos(int p_x, int p_y)
{
    tX = p_x/scale*scale;
    tY = p_y/scale*scale;

    updateProperties(true);
}

void WindowComponent::updateProperties(bool p_preventInvalidTPos) {}
