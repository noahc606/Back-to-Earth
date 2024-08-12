#include "Camera.h"
#include <math.h>
#include "Canvas.h"
#include "TileMapScreen.h"

Camera::Camera(){}
Camera::~Camera(){}

void Camera::init(SDLHandler* sh, Controls* ctrls)
{
    BTEObject::init(sh, nullptr, ctrls);
}

void Camera::tick()
{
    if(!focused) {
		/* Control zoom */
		if(controls->isHeld("MAP_USE_ZOOM")) {
			int mw = controls->getMouseWheel();
			zoomIndex += mw;

			int cz = 10;    //Closest possible zoom
			int fz = -10;   //Furthest possible zoom

			if(zoomIndex>=cz) zoomIndex = cz;
			if(zoomIndex<=fz) zoomIndex = fz;

			if(mw!=0) controls->resetWheel(__PRETTY_FUNCTION__);
		}
		double original = 1.5;
		double extremes = 7;
		zoom = original*pow( pow(extremes, 0.1), zoomIndex );
		//Round zoom to the next hundredth
		zoom = ceil(zoom*100.0)/100.0;
		
		/* Control direction */
		if(controls->isPressed("MAP_CHANGE_DIR")) {
			
			direction++;
			if(direction>=6) {
				direction = 0;
			}
			
			controls->stopPress("MAP_CHANGE_DIR", __PRETTY_FUNCTION__);
		}
    }
}

double Camera::getX() { return x; }
double Camera::getY() { return y; }
double Camera::getZ() { return z; }
int64_t Camera::getCsRX() { return floor( getCsX()/32 ); }
int64_t Camera::getCsRY() { return floor( getCsY()/32 ); }
int64_t Camera::getCsRZ() { return floor( getCsZ()/32 ); }


int64_t Camera::getRX() { return floor( getX()/32 ); }
int64_t Camera::getRY() { return floor( getY()/32 ); }
int64_t Camera::getRZ() { return floor( getZ()/32 ); }
int64_t Camera::getLayer(int c)
{
	switch(c)
	{
		case X: return (int64_t)floor(x);
		case Y: return (int64_t)floor(y);
		default: return (int64_t)floor(z);
	}
}
int64_t Camera::getLayer() { return getLayer(getAxis()); }
double Camera::getCsX() { return getCsXFromPos(x, y, z); }
double Camera::getCsY() { return getCsYFromPos(x, y, z); }
double Camera::getCsZ() { return getCsZFromPos(x, y, z); }
double Camera::getCsXFromPos(double px, double py, double pz) { return (getAxis()==X ? py : px); }
double Camera::getCsYFromPos(double px, double py, double pz) { return (getAxis()==Z ? py : pz); }
double Camera::getCsZFromPos(double px, double py, double pz) { return (getAxis()==X ? px : (getAxis()==Y ? py : pz)); }


double Camera::getZoom() { return zoom; }
int Camera::getZoomIndex() { return zoomIndex; }

/*
 * Get the scale (in pixels) of a single tile on screen.
 * Thus, if a single tile appears to be 12x12 physical pixels because of the current zoom, return 12. If 100x100, return 100.
 */
int Camera::getTileScale() { return 32.0*zoom; }
/*
 * Get the scale (in pixels) that should be used to blit a single tile.
 * 
 * That is, >16x16 tileScale returns 32, >8x8 tileScale returns 16, etc.
 */
int Camera::getBlitScale() { return 32.0*Canvas::getTexLODBasedOnZoom(zoom); }


int Camera::getScreenWidthTiles(SDLHandler* sh, int tileScale)
{
	//Get screen width (in tiles). Is always at least 1
	int screenWT = sh->getWidth()/tileScale;
	if( screenWT==0 ) screenWT = 1;
	return screenWT;
}
int Camera::getScreenWidthTiles() { return getScreenWidthTiles(sdlHandler, getTileScale()); }
int Camera::getScreenHeightTiles(SDLHandler* sh, int tileScale)
{
	//Get screen height (in tiles). Is always at least 1
	int screenHT = sh->getHeight()/tileScale;
	if( screenHT==0 ) screenHT = 1;
	return screenHT;
}
int Camera::getScreenHeightTiles() { return getScreenHeightTiles(sdlHandler, getTileScale()); }
int Camera::getScreenWidthReg(SDLHandler* sh, int tileScale) { return getScreenWidthTiles(sh, tileScale)/TileMapScreen::regionSize+1; }
int Camera::getScreenWidthReg() { return getScreenWidthReg(sdlHandler, getTileScale()); }
int Camera::getScreenHeightReg(SDLHandler* sh, int tileScale) { return getScreenHeightTiles(sh, tileScale)/TileMapScreen::regionSize+1; }
int Camera::getScreenHeightReg() { return getScreenHeightReg(sdlHandler, getTileScale()); }
/*
 * Get the number of graphical tile regions from the center of the screen to the left/right edges.
 */
int Camera::getScreenSemiWidthReg(SDLHandler* sh, int tileScale) { return getScreenWidthReg(sh, tileScale)/2+1; }
int Camera::getScreenSemiWidthReg() { return getScreenSemiWidthReg(sdlHandler, getTileScale()); }
/*
 * Get the number of graphical tile regions from the center of the screen to the upper/lower edges.
 */
int Camera::getScreenSemiHeightReg(SDLHandler* sh, int tileScale) { return getScreenHeightReg(sh, tileScale)/2+1; }
int Camera::getScreenSemiHeightReg() { return getScreenSemiHeightReg(sdlHandler, getTileScale()); }


int Camera::getDirection() { return direction; }
int Camera::coordinateHasDepth(int coord)
{
	return coord==getAxis();
}
int Camera::chd(int coord) { return coordinateHasDepth(coord); }

std::string Camera::directionToString(int direction)
{
	switch(direction) {
		case WEST: return "west";
		case EAST: return "east";
		case NORTH: return "north";
		case SOUTH: return "south";
		case UP:	return "up";
		case DOWN:	return "down";
	}
	return "unknown";
}

int Camera::getAxisFromDirection(int direction)
{
	switch(direction) {
		case WEST: case EAST: 	return X;
		case NORTH: case SOUTH: return Y;
		case UP: case DOWN: 	return Z;
	}
	
	return -100;
}

std::string Camera::getDirectionString() { return directionToString(direction); }

int Camera::getAxis() { return getAxisFromDirection(direction); }

bool Camera::isFocused() { return focused; }
bool Camera::isFreecam() { return freecam; }

void Camera::setXYZ(double p_x, double p_y, double p_z) { x = p_x; y = p_y; z = p_z;  }
void Camera::setFocused(bool p_focused) { focused = p_focused; }
