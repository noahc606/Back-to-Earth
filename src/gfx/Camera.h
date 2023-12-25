#pragma once
#include "BTEObject.h"

class Camera : public BTEObject
{
public:
    /**/
    Camera();
    virtual ~Camera();
    void init(Controls* ctrls);
    /**/
    void tick();
    /**/
    bool getInfo();
    double getX(); double getY(); double getZ(); long long getLayer();
    int getZoomIndex(); double getZoom();
    bool isFocused();
    bool isFreecam();
    /**/
    void setXYZ(double x, double y, double z);
    void setFocused(bool focused);
    /**/

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
	int camDirection = 5;

    bool focused = false;
    bool freecam = false;
};
