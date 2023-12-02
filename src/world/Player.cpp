#include "Player.h"
#include <cmath>
#include <math.h>
#include <iomanip>
#include <iostream>
#include "DebugScreen.h"
#include "Log.h"
#include "SpriteSheetBuilder.h"
#include "TextureLoader.h"
#include "TileMap.h"

Player::Player() { }
void Player::init(SDLHandler* sh, GUIHandler* guih, Controls* ctrls)
{
	sdlHandler = sh;
	guiHandler = guih;
	controls = ctrls;
	camera.init(ctrls);

	menu.init(sdlHandler, guiHandler, ctrls);

	/** Build player spritesheet */
	playerPal.init(playerPal.DEFAULT_PLAYER);

	/** Build sprite sheet */
	SpriteSheetBuilder ssb(sdlHandler);
	ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

	/** Build player textures */
	//Main (top view)
	playerTex.init(sdlHandler);
	playerTex.setTexDimensions(32, 64);
	//Alternate (side view)
	playerTexAlt.init(sdlHandler);
	playerTexAlt.setTexDimensions(32, 64);
	//Init HUD
	hud.init(sdlHandler, 30, 30);
	hud.setDrawScale(2);
	hud.setDrawPos(0, 0);

	/** Build bounding box */
	//TODO
	
	godMode = true;
	ghost = true;

	camera.setFocused(false);
}

Player::~Player() {}
void Player::destroy()
{
	playerTex.destroy();
}

void Player::draw(Canvas* csEntities)
{
	/*
	 *	Rebuild player texture from spritesheet
	 *	
	 *	We rebuild the player's texture every frame.
	 *	Meanwhile, the spritesheet was built once during player initialization.
	 */
	bool cameraHorizontal = false;
	playerTex.clear();
	if(cameraHorizontal) {
		playerTex.setTexDimensions(32, 64);
	} else {
		playerTex.setTexDimensions(32, 32);
	}
	
	//Get sprite sheet texture
	Texture* sst = spsh.getSheetTexture();
	
	//Render spritesheet on screen if it should be shown (debug feature)
	int* show_ss = Commands::getInt("player.show_ss");
	if( show_ss!=nullptr && *show_ss!=0 ) {
		sst->setDrawPos(0, 0);
		sst->draw();
	}
	
	//Build player texture
	buildPlayerTex(playerTex, cameraHorizontal);
	
	//If player is inside a tile, make sure part of the player isn't rendered
	/** Draw player texture to canvas */
	//playerTex.rect(0, 0, 32, 64, 255, 0, 0);
	csEntities->setSourceTex(&playerTex);
	csEntities->rcopy(x*32, y*32, playerTex.getTexWidth(), playerTex.getTexHeight());
}

void Player::drawCharInMenu()
{
	//Build player texture
	playerTexAlt.clear();
	buildPlayerTex(playerTexAlt, 1);
	playerTexAlt.setDrawScale(4);
	
	int ptaX = sdlHandler->getWidth()/2+(-32+2)*4;
	int ptaY = sdlHandler->getHeight()/2+(-64+10)*4;
	playerTexAlt.setDrawPos(ptaX, ptaY);
	playerTexAlt.draw();
}

void Player::drawHUD()
{
	int hudW = (sdlHandler->getWidth()+1)/2;
	int hudH = (sdlHandler->getHeight()+1)/2;
	hud.clear();
	hud.setTexDimensions(hudW, hudH);
	
	//Health Icon
	hud.lock( 0, hudH-64, 32, 32 );
	hud.blit( TextureLoader::GUI_player_interactions, 96, 0 );
	//Nutrition Icon
	hud.lock( 0, hudH-32, 32, 32 );
	hud.blit( TextureLoader::GUI_player_interactions, 96, 32 );
	
	//Defense Bar
	hud.lock( 32, hudH-64+7, 213, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 73 );
	hud.lock( 32, hudH-64+7, 213.0*defense/maxDefense, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 64 );
	//Health Bar
	hud.lock( 32, hudH-64+16, 223.0, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 91 );
	hud.lock( 32, hudH-64+16, 223.0*health/maxHealth, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 82 );
	//Food Bar
	hud.lock( 32, hudH-32+3, 233, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 109 );
	hud.lock( 32, hudH-32+3, 233.0*nutrition/maxNutrition, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 100 );
	//Water Bar
	hud.lock( 32, hudH-32+12, 243, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 127 );
	hud.lock( 32, hudH-32+12, 243.0*water/maxWater, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 118 );
	//Oxygen Bar
	hud.lock( 32, hudH-32+21, 253, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 145 );
	hud.lock( 32, hudH-32+21, 253.0*oxygen/maxOxygen, 9 );
	hud.blit( TextureLoader::GUI_player_interactions, 0, 136 );
	hud.draw();
}

void Player::tick()
{
	if( Commands::cmdIntResult("tele")!=nullptr ) {
		x = (*Commands::cmdIntResult("tele.x"));
		y = (*Commands::cmdIntResult("tele.y"));
		Commands::resetCMDEntered(__PRETTY_FUNCTION__);
	}
	
	/** Physical */
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Losing oxygen - 100 points. Losing 0.01/tick = lasts 2min 46sec. May be shorter depending on sprinting/swimming/etc.
	oxygen -= 0.01;
	//Losing water - 100 points. 0.0004/tick = lasts 69.4min = 2.89 earth days. May be shorter depending on heat
	water -=  0.0004;
	//Losing food - 100 points. 0.000025/tick = lasts 1111.1min = 46.29 earth days. May be shorter depending on body activity
	for(int i = 0; i<100; i++) {
		nutrition -= 0.000025;
	}
	
	/** Handle things being out of range */
	//Less than 0
	if(defense<0) defense = 0;
	if(health<0) health = 0;
	if(nutrition<0) nutrition = 0;
	if(water<0) water = 0;
	if(oxygen<0) oxygen = 0;
	//Greater than max
	if(defense>maxDefense) defense = maxDefense;
	if(health>maxHealth) health = maxHealth;
	if(nutrition>maxNutrition) nutrition = maxNutrition;
	if(water>maxWater) water = maxWater;
	if(oxygen>maxOxygen) oxygen = maxOxygen;
	
	
	age++;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/** Graphical */
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Determine player flip + rotation
	flip = Texture::Flip::NONE;
	if(facing==WEST) {
		flip = Texture::Flip::H;
	}
	rotation = 0;
	switch(facing) {
		case WEST: rotation = 270;	break;
		case NORTH: rotation = 0;	break;
		case EAST: rotation = 90;	break;
		case SOUTH: rotation = 180;	break;
	}
	
	/* Blinking */
	if(anBlinkTimer==-1) {
		if( rand()%240==0 ) anBlinkTimer++;
	} else {
		anBlinkTimer++;
	}
	if(anBlinkTimer>12) {
		anBlinkTimer = -480;
	}

	/**
		Walking

	*/

	//Set walk speed from velocity
	walkSpeed = std::abs((double)sqrt(vx*vx+vy*vy));
	//Set player facing direction (NESW)
	updateFacingDirection();

	/* Animation */
	//Animation timer. This timer increments every draw() call
	anTimer++;
	//The block below runs when anTimer is above a certain threshold.
	//This threshold is lower the faster the player is walking, resulting in a faster animation.
	if( anTimer>2.0/(walkSpeed/0.05) ) {
		//Reset anTimer.
		anTimer = 0;
		//Play the player's walking animation
		if( walkSpeed>0 ) {
			if(anWalkState==0) anWalkState = 1;
			anWalkFrameX += anWalkState;
			if( anWalkState==1 && anWalkFrameX==12 ) {
				anWalkFrameX = 0;
			}
		} else {

		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//Physics
	x+=vx;
	y+=vy;
	z+=vz;

	//Reset player action
	action = NONE;

	/** God mode actions */
	if( godMode ) {
		if( controls->isHeld("HARDCODE_RIGHT_CLICK") ) {
			action = Action::GM_Place_Tile;
		} else if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
			action = Action::GM_Destroy_Tile;
		}
	}

	controllable = true;
	if( guiHandler->getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_DEBUG)!=nullptr ) {
		controllable = false;
	}

	/** Player control */
	//Movement (sprint/crouch, walk)
	if( !camera.isFreecam() && controllable ) {
		double speed = 0.05;
		if( controls->isHeld("PLAYER_SPRINT") ) speed = 0.10;
		if( controls->isHeld("PLAYER_CROUCH") ) speed = 0.005;
		if( godMode ) {
			if( controls->isHeld("PLAYER_SPRINT") ) {
				speed = runSpeed;
				if ( controls->isHeld("PLAYER_CROUCH") ) {
					speed = 1000000;
				}
			}
		}

		vx = 0;
		vy = 0;
		vz = 0;
		if( controls->isHeld("PLAYER_MOVE_NORTH") ) vy = -speed;
		if( controls->isHeld("PLAYER_MOVE_SOUTH") ) vy =  speed;
		if( controls->isHeld("PLAYER_MOVE_WEST")  ) vx = -speed;
		if( controls->isHeld("PLAYER_MOVE_EAST")  ) vx =  speed;

		if( godMode ) {
			if( controls->isHeld("PLAYER_MOVE_UP")   ) vz = -speed;
			if( controls->isHeld("PLAYER_MOVE_DOWN") ) vz = speed;
		}
	}
	
	/** Camera set to player's (x, y, z) - keep as last */
	camera.tick();
	double tx = 0.5;
	double ty = 1.0;
	camera.setXYZ( (int)(x*32.0)/32.0+tx, (int)(y*32.0)/32.0+ty, z-1);

	/** Set bounding box */
}

void Player::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "XYZ=(" << x << ", " << y << ", " << z << "); ";
	DebugScreen::newLine(ss);

	DebugScreen::indentLine(ss, tabs);
	ss << "dXYZ=(" << vx << ", " << vy << ", " << vz << "); ";
	DebugScreen::newLine(ss);

	DebugScreen::indentLine(ss, tabs);
	ss << "(anWalkFrameX, anWalkState)=(" << anWalkFrameX << ", " << anWalkState << "); ";
	ss << "(walkSpeed)=(" << walkSpeed << "); ";
	DebugScreen::newLine(ss);
}

int Player::getAction() { return action; }
std::tuple<double, double, double> Player::getPos() { return std::make_tuple(x, y, z); }
std::tuple<double, double, double> Player::getVelComponents() { return std::make_tuple(vx, vy, vz); }
double Player::getVel() { return sqrt( vx*vx + vy*vy + vz*vz ); }
Camera* Player::getCamera() { return &camera; }
PlayerMenu* Player::getMenu() {
	return &menu;
}

void Player::setPos(double x, double y, double z)
{
	Player::x = x;
	Player::y = y;
	Player::z = z;
}

void Player::setMenuState(int newMenuState)
{
	menu.setState(newMenuState);
}

void Player::updateFacingDirection()
{
	if( vy<0 ) {
		facing = Directions::NORTH;
	} else if( vy>0 ) {
		facing = Directions::SOUTH;
	}

	if( vx<0 ) {
		facing = Directions::WEST;
	} else if( vx>0 ) {
		facing = Directions::EAST;
	}
}

void Player::buildPlayerTex(Texture& tex, int camDirection)
{
	//Get sprite sheet texture
	Texture* sst = spsh.getSheetTexture();
	
	if(camDirection==0) {
		/* Lower body (legs) */
		tex.lock();
		//tex.blitEx(sst, 0, 32*TOP_LOWER_BODY, 32, 32, rotation);
		//if( anWalkState==0 ) {
		tex.blitEx(sst, 32*anWalkFrameX, (TOP_LOWER_BODY+1)*32, 32, 32, rotation);
		//}
		
		
		/* Middle body */
		tex.lock();
		tex.blitEx(sst, 0, 32*TOP_MIDDLE_BODY, 32, 32, rotation);
		
		/* Arms (if player is walking) */
		if(walkSpeed>0)
		{
			tex.lock();
			tex.blitEx(sst, 0, 32*TOP_ARMS, 32, 32, rotation);
		}
		
		
		/* Head */
		tex.lock();
		tex.blitEx(sst, 0, 32*TOP_HAIR, 32, 32, rotation);
	} else {
		/** Head */
		//Head base
		tex.lock(0, 0, 32, 32);
		tex.blitEx(sst, 0, 32*SIDE_HEAD_BASE, 32, 32, flip);

		//Eyes layer 1
		if(anBlinkTimer<0) {
			tex.lock(0, 0, 32, 32);
			tex.blitEx(sst, 0, 32*SIDE_HEAD_EYES, 32, 32, flip);
		}

		//Eyes layer 2
		if(anBlinkTimer<0) {
			tex.lock(0, 0, 32, 32);
			tex.blitEx(sst, 0, 32*SIDE_HEAD_PUPILS, 32, 32, flip);
		}

		//Lips
		tex.lock(0, 0, 32, 32);
		tex.blitEx(sst, 0, 32*SIDE_HEAD_MOUTH, 32, 32, flip);

		/** Arms */
		//Arms
		int aTX = (anWalkFrameX%2);
		tex.lock(0+aTX, 16, 32+aTX, 32);
		tex.blitEx(sst, 0, 32*SIDE_ARMS, flip);

		/** Body */
		//Legs
		tex.lock(0, 29, 32, 32);
		if( anWalkState!=0 ) {
			int dy = 1;
			if( facing==SOUTH || facing==NORTH ) {
				dy = 2;
			}
			tex.blitEx(sst, 32*anWalkFrameX, (SIDE_LOWER_BODY+dy)*32, 32, 32, flip);
		} else {
			tex.blitEx(sst, 32*anWalkFrameX, (SIDE_LOWER_BODY)*32, 32, 32, flip);
		}
		//Middle body
		{
			tex.lock(0+aTX, 9, 32+aTX, 32);

			tex.blitEx(sst, 0, (SIDE_MIDDLE_BODY)*32, 32, 32, flip);
		}
		
		/** Extra */
		//Shoes
		//tex.lock(1, 48, 32, 32);
		//tex.blitEx(sst, 0, FEET*32, 31, 32, flip);

		//Hair
		tex.lock(0, 0, 32, 32);
		tex.blitEx(sst, 0, 32*SIDE_HAIR, 32, 32, flip);
	}
}