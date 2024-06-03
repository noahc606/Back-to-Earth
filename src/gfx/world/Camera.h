#pragma once
#include "BTEObject.h"

class Camera : public BTEObject
{
public:
    /**/
    Camera();
    virtual ~Camera();
    void init(SDLHandler* sh, Controls* ctrls);
    /**/
    void tick();
    /**/
    bool getInfo();
    double getX(); double getY(); double getZ();
	int64_t getCsRX(); int64_t getCsRY(); int64_t getCsRZ();
	int64_t getRX(); int64_t getRY(); int64_t getRZ();
	int64_t getLayer(int c); int64_t getLayer();	
	double getCsX(); double getCsY(); double getCsZ();
	double getCsXFromPos(double px, double py, double pz);
	double getCsYFromPos(double px, double py, double pz);
	double getCsZFromPos(double px, double py, double pz);


	//Zoom/Scale/Screen stuff
	int getZoomIndex(); double getZoom();
	int getTileScale(); int getBlitScale();
    static int getScreenWidthTiles(SDLHandler* sh, int tileScale);      int getScreenWidthTiles();
    static int getScreenHeightTiles(SDLHandler* sh, int tileScale);     int getScreenHeightTiles();
    static int getScreenWidthReg(SDLHandler* sh, int tileScale);        int getScreenWidthReg();
    static int getScreenHeightReg(SDLHandler* sh, int tileScale);       int getScreenHeightReg();
    static int getScreenSemiWidthReg(SDLHandler* sh, int tileScale);    int getScreenSemiWidthReg();
    static int getScreenSemiHeightReg(SDLHandler* sh, int tileScale);   int getScreenSemiHeightReg();

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
};
