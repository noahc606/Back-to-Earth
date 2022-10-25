#include "GUIAlignable.h"

GUIAlignable::GUIAlignable()
{
    //ctor
}

GUIAlignable::~GUIAlignable()
{
    //dtor
}

/**/

void GUIAlignable::translateSPos() {}
void GUIAlignable::onWindowUpdate(bool preventInvalidTPos) {}

/**/

int GUIAlignable::getHorAlignment() { return horAlignment; }
int GUIAlignable::getVerAlignment() { return verAlignment; }
int GUIAlignable::getAlignment(bool dim) { if(dim==0) return horAlignment; return verAlignment; }

int GUIAlignable::getSX() { return sX; }
int GUIAlignable::getSY() { return sY; }
int GUIAlignable::getSPos(bool dim) { if(dim==0) return sX; return sY; }

int GUIAlignable::getWidth() { return width; }
int GUIAlignable::getHeight() { return height; }
int GUIAlignable::getDim(bool dim) { if(dim==0) return width; return height; }

/**/

void GUIAlignable::setSX(int sx) { sX = sx/scale*scale; }
void GUIAlignable::setSY(int sy) { sY = sy/scale*scale; }
void GUIAlignable::setSPos(int sx, int sy)
{
    setSX(sx);
    setSY(sy);
}

/**/
