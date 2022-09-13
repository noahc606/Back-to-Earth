#pragma once
#include <tuple>
#include "BTEObject.h"
#include "Canvas.h"
#include "Camera.h"
#include "CollidingVolume.h"
#include "ColorPalette.h"
#include "SpriteSheet.h"
#include "Texture.h"


class Player : public BTEObject
{
public:
    /**/
    Player();
    void init(SDLHandler* sh, Controls* ctrls);
    virtual ~Player();
    void destroy();
    /**/
    void draw(Canvas* csEntities);
    void tick();
    /**/
    void info(std::stringstream& ss, int& tabs);
    int getAction();
    Box3D getBounds();
    std::tuple<double, double, double> getPos();
    std::tuple<double, double, double> getVelComponents();
    double getVel();
    Camera* getCamera();
    /**/

    enum Action {
        NONE,
        GM_Place_Tile,
        GM_Destroy_Tile,
    };

    enum Components {
        HAIR,
        HEAD_BASE, HEAD_EYES, HEAD_PUPILS, HEAD_MOUTH,
        ARMS,
        MIDDLE_BODY,
        LOWER_BODY,
        FEET,

        LAST_INDEX
    };

protected:

private:
    enum Directions {
        NORTH,
        EAST,
        SOUTH,
        WEST,
    };

    ColorPalette playerPal;
    Texture playerTex;
    SpriteSheet spsh;
    int anTimer = 0;
    int anWalkFrame = 0; int anWalkState = 0;
    int anWalkShirtFrame = 0; int anWalkShirtState = 0;
    char facing = NORTH;

    bool godMode = true;
    bool ghost = true;

    Camera camera;

    int action = NONE;

    double age = 0;                                 //Age in ticks (60ticks = 1 second)
    double x  = 0; double y  = 0; double z  = 0;    //Position in world (meters)
    double vx = 0; double vy = 0; double vz = 0;    //Velocity in world (meters per tick)
    double ay = -9.81/3600.0;                       //Vertical acceleration in world (meters per tick^2)
    Box3D bounds;

    int timerBlink = 0;

};
