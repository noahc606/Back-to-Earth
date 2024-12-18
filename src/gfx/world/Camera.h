#pragma once
#include <nch/math-utils/vec3.h>
#include <string>
#include "Controls.h"
#include "SDLHandler.h"


class Camera
{
public:
    /**/
    Camera();
    virtual ~Camera();
    void init(SDLHandler* sh, Controls* ctrls);
    /**/
    void tick();
	void draw();
	void drawSelectionFromSrc(SDL_Rect& src, int type);
    /**/
    bool getInfo();
    double getX(); double getY(); double getZ();
	int64_t getCsRX(); int64_t getCsRY(); int64_t getCsRZ();
	int64_t getRX(); int64_t getRY(); int64_t getRZ();
	int64_t getLayer(int c); int64_t getLayer();	
	nch::Vec3<double> getCsPos();
	template<typename T> nch::Vec3<T> getCsPosFromWorldPos(nch::Vec3<T> wpos) {
		return nch::Vec3<T>(getAxis()==X ? wpos.y : wpos.x, getAxis()==Z ? wpos.y : wpos.z, getAxis()==X ? wpos.x : (getAxis()==Y ? wpos.y : wpos.z));
	}
	template<typename T> nch::Vec3<T> getWorldPosFromCsPos(nch::Vec3<T> cspos) {
		switch(getAxis()) {
			case Camera::X: return nch::Vec3<T>(cspos.z, cspos.x, cspos.y);
			case Camera::Y: return nch::Vec3<T>(cspos.x, cspos.z, cspos.y);
		}
		return nch::Vec3<T>(cspos.x, cspos.y, cspos.z);
	}

	//Zoom/Scale/Screen stuff
	int getZoomIndex(); double getZoom();
	int getTileScale(); int getBlitScale();
    static int getScreenWidthTiles(SDLHandler* sh, int tileScale);      int getScreenWidthTiles();
    static int getScreenHeightTiles(SDLHandler* sh, int tileScale);     int getScreenHeightTiles();
    static int getScreenWidthReg(SDLHandler* sh, int tileScale);        int getScreenWidthReg();
    static int getScreenHeightReg(SDLHandler* sh, int tileScale);       int getScreenHeightReg();
    static int getScreenSemiWidthReg(SDLHandler* sh, int tileScale);    int getScreenSemiWidthReg();
    static int getScreenSemiHeightReg(SDLHandler* sh, int tileScale);   int getScreenSemiHeightReg();

	int getOppositeDirection();
	int getDirection();
	int coordinateHasDepth(int coord);
	int chd(int coord);
	static std::string directionToString(int direction);
	static int getAxisFromDirection(int direction);
	std::string getDirectionString();
	int getAxis();
    bool isFocused();
    bool isFreecam();
    /**/
    void setXYZ(double x, double y, double z);
    void setFocused(bool focused);
    /**/

	enum Axes {
		X = 0,
		Y = 1,
		Z = 2
	};

	enum Directions {
		WEST = 0,
		EAST,
		NORTH,
		SOUTH,
		UP,
		DOWN,
	};

protected:

private:
	SDLHandler* sdlHandler;
	Controls* controls;

	int getDirFromSelection();

    double x = 0; double y = 0; double z = 0;

    int zoomIndex = 0; double zoom = 1;
	
	/*
	 *	6 possible values for direction: [012345] = [WENSUD]
	 * 	West/East 	= -X/+X
	 * 	North/South = -Y/+Y
	 * 	Up/Down 	= -Z/+Z
	 */
	int direction = DOWN;

    bool focused = false;
    bool freecam = false;

	SDL_Rect selBoxSrc;
	SDL_Rect selBoxDst;
	bool holdingModifier = false;
	bool camModUp = false;
	bool camModRight = false;
	bool camModDown = false;
	bool camModLeft = false;
	bool camModBack = false;
};
