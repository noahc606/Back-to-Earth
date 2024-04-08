#pragma once
#include <tuple>
#include "BTEObject.h"
#include "Canvas.h"
#include "Camera.h"
#include "CollidingVolume.h"
#include "ColorPalette.h"
#include "GUIHandler.h"
#include "Loggable.h"
#include "SpriteSheet.h"
#include "Texture.h"


class Player : public BTEObject, public Loggable
{
public:
	/**/
	Player();
	void init(SDLHandler* sh, GUIHandler* guih, Controls* ctrls);
	virtual ~Player();
	void destroy();
	/**/
	void draw(Canvas* csEntities);
	void drawCharInMenu();
	void drawHUD();
	void tick();
	void tickControls();
	void putInfo(std::stringstream& ss, int& tabs);
	/**/
	int getAction();
	Box3D* getBounds();
	std::tuple<double, double, double> getPos();
	std::tuple<double, double, double> getVelComponents();
	double getVel();
	Camera* getCamera();
	/**/
	void setPos(double, double, double);
	void setMenuState(int newMenuState);

	enum Action {
		NONE,
		GM_Place_Tile,
		GM_Destroy_Tile,
	};

	enum Components {
		TOP_HAIR,
		TOP_ARMS,
		TOP_MIDDLE_BODY,
		TOP_LOWER_BODY,
		
		SIDE_HAIR = TOP_LOWER_BODY+2,
		SIDE_HEAD_BASE, SIDE_HEAD_EYES, SIDE_HEAD_PUPILS, SIDE_HEAD_MOUTH,
		SIDE_ARMS,
		SIDE_MIDDLE_BODY,
		SIDE_LOWER_BODY,
		SIDE_FEET = SIDE_LOWER_BODY+3,

		LAST_INDEX
	};

protected:

private:
	void updateFacingDirection();
	void rebuildPlayerTex(bool alt);

	//Player palettes, textures, spritesheets
	ColorPalette playerPal;
	Texture playerTex;		//Render this in world
	Texture playerTexAlt;	//Render this in character menu
	Texture hud;			//Main screen HUD.
	SpriteSheet spsh;
	
	//Player animation
	int anTimer = 0;
	int anStandFrame = 0;
	int anWalkFrameX = 0; int anWalkState = 0;
	int anWalkShirtFrame = 0; int anWalkShirtState = 0;
	int anBlinkTimer = 0;

	//Physical properties
	double walkSpeed = 0.0;
	char facing = Camera::NORTH;
	int flip = 0; int rotation = 0;
	double defense = 0; double maxDefense = 100;
	double health = 100; double maxHealth = 100;
	double nutrition = 100; double maxNutrition = 100;
	double water = 100; double maxWater = 100;
	double oxygen = 100; double maxOxygen = 100;
	
	bool godMode = true;
	bool ghost = true;
	bool controllable = false;

	Camera camera;
	GUIHandler* guiHandler;

	int action = NONE;

	double runSpeed = 10;
	double age = 0;                                 //Age in ticks (60ticks = 1 second)
	double x  = 0; double y  = 0; double z  = 0;    //Position in world (meters)
	double vx = 0; double vy = 0; double vz = 0;    //Velocity in world (meters per tick)
	double ay = -9.81/3600.0;                       //Vertical acceleration in world (meters per tick^2)
	Box3D bounds;


};
