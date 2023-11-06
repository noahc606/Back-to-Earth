#include "Camera.h"
#include <math.h>

Camera::Camera(){}
Camera::~Camera(){}

void Camera::init(Controls* ctrls)
{
    BTEObject::init(nullptr, nullptr, ctrls);
}

void Camera::tick()
{
    if( freecam ) {
        float flySpeed = 0.05;
        if( controls->isHeld("PLAYER_SPRINT") ) flySpeed = 0.5;
        if( controls->isHeld("PLAYER_CROUCH") ) flySpeed = 0.01;
        if( controls->isHeld("PLAYER_CROUCH") && controls->isHeld("PLAYER_SPRINT") ) {
            flySpeed = 10;
        }

        if( controls->isHeld("PLAYER_MOVE_WEST" ) ) x=x-flySpeed;
        if( controls->isHeld("PLAYER_MOVE_EAST" ) ) x=x+flySpeed;
        if( controls->isHeld("PLAYER_MOVE_SOUTH") ) y=y+flySpeed;
        if( controls->isHeld("PLAYER_MOVE_NORTH") ) y=y-flySpeed;
        if( controls->isHeld("PLAYER_MOVE_DOWN" ) ) z=z-flySpeed;
        if( controls->isHeld("PLAYER_MOVE_UP"   ) ) z=z+flySpeed;
    }

    if( !focused ) {
        //Control zoom
        if( controls->isHeld("MAP_USE_ZOOM") ) {
            int mw = controls->getMouseWheel();
            zoomIndex += mw;

            int cz = 10;    //Closest possible zoom
            int fz = -10;   //Furthest possible zoom

            if(zoomIndex>=cz) zoomIndex = cz;
            if(zoomIndex<=fz) zoomIndex = fz;

            if( mw!=0) controls->resetWheel(__PRETTY_FUNCTION__);
        }
        double original = 1.5;
        double extremes = 7;
        zoom = original*pow( pow(extremes, 0.1), zoomIndex );
        //Round zoom to the next hundredth
        zoom = ceil(zoom*100.0)/100.0;

    }
}

double Camera::getX() { return x; }
double Camera::getY() { return y; }
double Camera::getZ() { return z; }
long long Camera::getLayer() { return (long long)floor(z); }
double Camera::getZoom() { return zoom; }
int Camera::getZoomIndex() { return zoomIndex; }
bool Camera::isFocused() { return focused; }
bool Camera::isFreecam() { return freecam; }

void Camera::setXYZ(double p_x, double p_y, double p_z) { x = p_x; y = p_y; z = p_z;  }
void Camera::setFocused(bool p_focused) { focused = p_focused; }
