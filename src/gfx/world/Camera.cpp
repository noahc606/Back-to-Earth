#include "Camera.h"
#include <math.h>
#include "Canvas.h"
#include "TileMapScreen.h"

Camera::Camera(){}
Camera::~Camera(){}

void Camera::init(SDLHandler* sh, Controls* ctrls)
{
	sdlHandler = sh;
	controls = ctrls;

	selBoxSrc.x = 138; selBoxSrc.y = 138;
	selBoxSrc.w = 254; selBoxSrc.h = 254;
}

void Camera::tick()
{
	if(focused) return;

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
	


	/* Control map direction */
	//Turn on hold modifier
	if(controls->isPressed("MAP_CHANGE_DIR")) {
		controls->stopPress("MAP_CHANGE_DIR", __PRETTY_FUNCTION__);
		holdingModifier = true;


		SDL_WarpMouseInWindow(sdlHandler->getWindow(), sdlHandler->getWidth()/4*2, sdlHandler->getHeight()/4*2);
	}
	//Turn off hold modifier, change direction
	if(controls->isReleased("MAP_CHANGE_DIR")) {
		controls->stopRelease("MAP_CHANGE_DIR", __PRETTY_FUNCTION__);
		direction = getDirFromSelection();
		holdingModifier = false;
	}

	camModUp = false;
	camModRight = false;
	camModDown = false;
	camModLeft = false;
	camModBack = false;
	if(holdingModifier) {
		int mx = controls->getMouseX()/2*2;
		int my = controls->getMouseY()/2*2;
		int scx = sdlHandler->getWidth()/4*2;
		int scy = sdlHandler->getHeight()/4*2;

		//Update direction selection box
		SDL_Rect sbs = selBoxSrc;
		selBoxDst.x = scx-sbs.w;
		selBoxDst.y = scy-sbs.h;
		selBoxDst.w = sbs.w*2;
		selBoxDst.h = sbs.h*2;
		SDL_Rect sbd = selBoxDst;

		//If the mouse is over the direction selection UI
		if(mx>sbd.x+2 && mx<=sbd.x+sbd.w-2 && my>sbd.y+2 && my<=sbd.y+sbd.h-2) {

			int dmx = mx-scx;
			int dmy = my-scy;


			//Backwards
			bool withinCenter = false;
			if(mx>scx-108 && mx<=scx+108 && my>scy-108 && my<=scy+108) {
				withinCenter = true;
				if(!(mx>scx-38 && mx<=scx+38 && my>scy-38 && my<=scy+38)) {
					camModBack = true;
				}
			}
					
			if(!withinCenter) {
				if(dmy>=dmx && dmy<=-dmx) { camModLeft = true;	}	//Left
				if(dmy<=dmx && dmy>=-dmx) { camModRight = true;	}	//Right
				if(dmy<dmx && dmy<-dmx)   { camModUp = true;	}	//Up
				if(dmy>dmx && dmy>-dmx)   { camModDown = true; 	}	//Down
			}
		}
	}
}

void Camera::draw()
{
	if(!holdingModifier) return;
	int texID = TextureLoader::GUI_player_camera;
	int scx = sdlHandler->getWidth()/4*2;
	int scy = sdlHandler->getHeight()/4*2;

	//Draw direction selection box
	sdlHandler->renderCopy(texID, &selBoxSrc, &selBoxDst);
	
	//Draw hovered element within selBox
	SDL_Rect src; SDL_Rect dst;

	if(camModBack)	{ src.x = 0; 	src.y = 0; 	 src.w = 108; src.h = 108; drawSelectionFromSrc(src, 0); }
	else 			{ src.x = 422; 	src.y = 0; 	 src.w = 108; src.h = 108; drawSelectionFromSrc(src, 0); }
	if(camModLeft)  { src.x = 67; 	src.y = 140; src.w = 71;  src.h = 250; drawSelectionFromSrc(src, 1); }
	else 			{ src.x = 0; 	src.y = 140; src.w = 67;  src.h = 250; drawSelectionFromSrc(src, 2); }
	if(camModRight) { src.x = 392; 	src.y = 140; src.w = 71;  src.h = 250; drawSelectionFromSrc(src, 3); }
	else 			{ src.x = 463; 	src.y = 140; src.w = 67;  src.h = 250; drawSelectionFromSrc(src, 4); }
	if(camModUp) 	{ src.x = 140; 	src.y = 67;  src.w = 250; src.h = 71;  drawSelectionFromSrc(src, 5); }
	else 			{ src.x = 140; 	src.y = 0;   src.w = 250; src.h = 67;  drawSelectionFromSrc(src, 6); }
	if(camModDown) 	{ src.x = 140; 	src.y = 392; src.w = 250; src.h = 71;  drawSelectionFromSrc(src, 7); }
	else 			{ src.x = 140; 	src.y = 463; src.w = 250; src.h = 67;  drawSelectionFromSrc(src, 8); }
}

void Camera::drawSelectionFromSrc(SDL_Rect& src, int type) {
	int scx = sdlHandler->getWidth()/4*2;
	int scy = sdlHandler->getHeight()/4*2;
	int texID = TextureLoader::GUI_player_camera;

	int dx = 0;
	int dy = 0;
	switch(type) {
		case 1: dx = -181; break;
		case 2: dx = -181; break;
		case 3: dx = 181; break;
		case 4: dx = 181; break;
		case 5: dy = -181; break;
		case 6: dy = -181; break;
		case 7: dy = 181; break;
		case 8: dy = 181; break;
	}
	
	SDL_Rect dst;
	dst.x = scx-src.w+dx;
	dst.y = scy-src.h+dy;
	dst.w = src.w*2;
	dst.h = src.h*2;

	sdlHandler->renderCopy(texID, &src, &dst);

	/* Text */
	int mx = controls->getMouseX()/2*2;
	int my = controls->getMouseY()/2*2;

	TextOld::drawBoxed(sdlHandler, "Facing: "+directionToString(getDirFromSelection()), mx, my-20, nch::Color(255, 255, 0));
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

int Camera::getOppositeDirection()
{
	switch(direction) {
		case NORTH: return SOUTH; break;
		case SOUTH: return NORTH; break;
		case EAST: 	return WEST; break;
		case WEST: 	return EAST; break;
		case UP: 	return DOWN; break;
		case DOWN: 	return UP; break;
	}
	return -1;
}
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

int Camera::getDirFromSelection()
{
	int res = direction;
	bool u = false;

	//Backwards
	if(camModBack) {
		return getOppositeDirection();
	}

	//Up, right, down, left
	switch(res) {
		case NORTH: case SOUTH: {
			if(camModUp) 	return UP;
			if(camModRight) return EAST;
			if(camModDown) 	return DOWN;
			if(camModLeft) 	return WEST;
		} break;
		case WEST: case EAST: {
			if(camModUp) 	return UP;
			if(camModRight) return NORTH;
			if(camModDown) 	return DOWN;
			if(camModLeft) 	return SOUTH;
		} break;
		case UP: case DOWN: {
			if(camModUp) 	return NORTH;
			if(camModRight) return EAST;
			if(camModDown) 	return SOUTH;
			if(camModLeft) 	return WEST;
		} break;
	}

	return res;
}