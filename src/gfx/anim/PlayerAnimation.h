#pragma once
#include "ColorPalette.h"
#include "Texture.h"
#include "SpriteSheet.h"

class PlayerAnimation
{
public:
	enum Components {
		TOP_HAIR,
		TOP_ARMS,
		TOP_MIDDLE_BODY,
		TOP_LOWER_BODY, TOP_LOWER_BODY_B,
		
		SIDE_HAIR,																	//Side hair
		SIDE_HEAD_BASE, SIDE_HEAD_EYES, SIDE_HEAD_PUPILS, SIDE_HEAD_MOUTH,			//Side head objects
		SIDE_ARMS,																	//Side arms
		SIDE_MB_TURNING, SIDE_MB_WALKING, SIDE_MB_WALKING_FRONT, SIDE_MB_JUMPING,	//Side middle body (shirt)
		SIDE_LOWER_BODY, SIDE_LOWER_BODY_B, SIDE_LOWER_BODY_C,						//Side lower body (leggings)
		SIDE_FEET,

		LAST_INDEX
	};
    void init(SDLHandler* sh, Settings* stngs, Camera* cam);
    void destroy();

    void tick(double vx, double vy, double vz);
	void draw(Canvas* csEntities, bool debugging, bool noclip, double x, double y, double z);
    void drawCharInMenu();

    Texture* getPlayerTex();
    void updateFacingDirection(double vx, double vy, double vz);
    void rebuildPlayerTex(Texture& tex, bool alt);
private:
    //Accessors
    SDLHandler* sdlHandler;
    Camera* camera;
    Settings* settings;

	//Palette, textures, spritesheet
	ColorPalette playerPal;	
	Texture playerTex;		//Render this in world
	Texture playerTexAlt;	//Render this in character menu
	SpriteSheet spsh;
	//Player animation frame tracking + timers
	int anTimer = 0;
	int anStandFrame = 0;
	int anWalkFrameX = 0; int anWalkState = 0;
	int anWalkShirtFrame = 0; int anWalkShirtState = 0;
	int anBlinkTimer = 0;

	double walkSpeed = 0.0;
	char facing = Camera::NORTH;
	int flip = 0; int rotation = 0;
};
