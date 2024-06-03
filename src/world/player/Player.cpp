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

const double Player::unit = 0.03125; // 0.03125 = 1/32

Player::Player() { }
void Player::init(SDLHandler* sh, GUIHandler* guih, Settings* stngs, Controls* ctrls)
{
	sdlHandler = sh; guiHandler = guih;
	settings = stngs; controls = ctrls;
	
	camera.init(sdlHandler, ctrls);
	camera.setFocused(false);

/** Build player palette, spritesheets and textures */
//Palette
	playerPal.initPlayerPalette(settings);
//Build sprite sheet
	SpriteSheetBuilder ssb(sdlHandler);
	ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);
//Textures
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
	
/** Player properties */
//Booleans
	godMode = true;
	noclip = false;
}

Player::~Player() {}
void Player::destroy()
{
	playerTex.destroy();
}

void Player::draw(Canvas* csEntities, bool debugging)
{
	/*
	 *	Rebuild player texture from spritesheet
	 *	
	 *	We rebuild the player's texture every frame.
	 *	Meanwhile, the spritesheet was built once during player initialization.
	 */
	bool cameraHorizontal = false;
	if(camera.getAxis()!=Camera::Z) {
		cameraHorizontal = true;
	}
	
	//Get sprite sheet texture
	Texture* sst = spsh.getSheetTexture();
	
	//Render spritesheet on screen if it should be shown (debug feature)
	int* show_ss = Commands::getInt("player.show_ss");
	if( show_ss!=nullptr && *show_ss!=0 ) {
		sst->setDrawPos(0, 0);
		sst->draw();
	}
	
	//Player is slightly translucent
	int opacity = 255;
	if(noclip) {
		opacity = 150;
	}

	//Build player texture
	if(0) {
		playerTex.clear();
		playerTex.setTexDimensions(20, 64);
	} else if(!debugging) {
		playerTex.setColorMod(255, 255, 255, opacity);
		rebuildPlayerTex(playerTex, cameraHorizontal);
	} else {
		if(cameraHorizontal) {
			playerTex.clear();
			playerTex.setTexDimensions(32, 64);
			
			playerTex.lock();
			playerTex.setColorMod(255, 0, 0, opacity);
			playerTex.fill();
		} else {
			playerTex.clear();
			playerTex.setTexDimensions(32, 32);

			playerTex.lock();
			playerTex.setColorMod(255, 0, 0, opacity);
			playerTex.fill();
		}
	}


	//If player is inside a tile, make sure part of the player isn't rendered
	/** Draw player texture to canvas */
	//playerTex.rect(0, 0, 32, 64, 255, 0, 0);
	csEntities->setSourceTex(&playerTex);
	switch(camera.getAxis()) {
		case Camera::X: {
			csEntities->rcopy(y*32-16, z*32, playerTex.getTexWidth(), playerTex.getTexHeight());
		} break;
		case Camera::Y: {
			csEntities->rcopy(x*32-16, z*32, playerTex.getTexWidth(), playerTex.getTexHeight());
		} break;
		case Camera::Z: {
			csEntities->rcopy(x*32-16, y*32-16, playerTex.getTexWidth(), playerTex.getTexHeight());
		} break;
	}
	
}

void Player::drawCharInMenu()
{
	//Build player texture
	rebuildPlayerTex(playerTexAlt, true);
	
	int ptaX = sdlHandler->getWidth()/2+(-32+2)*4;
	int ptaY = sdlHandler->getHeight()/2+(-64+10)*4;
	playerTexAlt.setDrawPos(ptaX, ptaY);
	playerTexAlt.setDrawScale(4);
	playerTexAlt.draw();
}

void Player::drawHUD()
{
	int hudW = (sdlHandler->getWidth()+1)/2;
	int hudH = (sdlHandler->getHeight()+1)/2;
	hud.clear();
	hud.setTexDimensions(hudW, hudH);
	
	if( !godMode ) {
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
}

void Player::tick()
{
/** Process commands */
	//"tele" command
	if( Commands::cmdIntResult("tele")!=nullptr ) {
		x = (*Commands::cmdIntResult("tele.x"));
		y = (*Commands::cmdIntResult("tele.y"));
		z = (*Commands::cmdIntResult("tele.z"));
		Commands::resetCMDEntered(__PRETTY_FUNCTION__);
	}
	
/** Player's body state */
//Incrementers
	//Gaining age - +1/tick = +40/sec.
	age++;
//Decrementers
	//Losing oxygen - 100 points. Losing 0.015/tick = lasts 2min 46sec. May be shorter depending on sprinting/swimming/etc.
	oxygen -= 0.015;
	//Losing water - 100 points. 0.0006/tick = lasts 69.4min = 2.89 earth days. May be shorter depending on heat
	water -=  0.0006;
	//Losing food - 100 points. 0.0000375/tick = lasts 1111.1min = 46.29 earth days. May be shorter depending on body activity
	nutrition -= 0.0000375;
//Limiters (handle things out of range)
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

/** Player actions and controls */
//Set player actions
	//Player action reset to NONE
	action = NONE;
	//Special actions in godMode
	if( godMode ) {
		if( controls->isHeld("HARDCODE_RIGHT_CLICK") ) {
			action = Action::GM_Place_Tile;
		} else if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
			action = Action::GM_Destroy_Tile;
		}
	}
//Set whether player is controllable
	//Controllable reset to true
	controllable = true;
	//If a tbx_DEBUG exists (console command textbox), disable controlling
	if( guiHandler->getGUI(BTEObject::GUI_textbox, GUIHandler::tbx_DEBUG)!=nullptr ) {
		controllable = false;
	}
//Tick controls if appropriate
	//Movement (sprint/crouch, walk)
	if(!camera.isFreecam() && controllable) {
		tickControls();
	}
	

/** Snap from bounding boxes (after velocity processing) */
//Based on snap calculated last tick, limit certain velocity components
	if(snapD && vz>0) { vz = 0; }
	if(snapU && vz<0) { vz = 0; }
	if(snapE && vx>0) { vx = 0; }
	if(snapW && vx<0) { vx = 0; }
	if(snapS && vy>0) { vy = 0; }
	if(snapN && vy<0) { vy = 0; }

/** Player position and velocity */
	//vx, vy, vz velocity components added to x, y, z
	x+=vx;
	y+=vy;
	z+=vz;

/** Visuals */
//Player animation
	//Tick animation
	tickAnimation();
//Camera - keep as last
	//Tick camera
	camera.tick();
	//Camera translations
	double dcx = 0.0;
	double dcy = 0.0;
	double dcz = 0.0;
	switch(camera.getAxis()) {
		case Camera::X: { dcx = 0.0; dcy = 0.0; dcz = 1.0; } break;
		case Camera::Y: { dcx = 0.0; dcy = 0.0; dcz = 1.0; } break;
	}
	//Camera coordinates
	double cx = (int)(x*32.0)/32.0;
	double cy = (int)(y*32.0)/32.0;
	double cz = z;
	//Set camera to appropriate coordinates
	camera.setXYZ(cx+dcx, cy+dcy, cz+dcz);
}

void Player::tickAnimation()
{
	//Determine player flip + rotation
	flip = Texture::Flip::NONE;
	if(facing==Camera::WEST) {
		flip = Texture::Flip::H;
	}
	rotation = 0;
	switch(facing) {
		case Camera::WEST: rotation = 270;	break;
		case Camera::NORTH: rotation = 0;	break;
		case Camera::EAST: rotation = 90;	break;
		case Camera::SOUTH: rotation = 180;	break;
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
	walkSpeed = std::abs((double)sqrt(vx*vx+vy*vy))/2.0;
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
}

void Player::tickControls()
{
	noclip = false;
	double speed = 0.085;
	if( controls->isHeld("PLAYER_SPRINT") ) speed = 0.170;
	if( controls->isHeld("PLAYER_CROUCH") ) speed = 0.009;
	if( godMode ) {
		if( controls->isHeld("PLAYER_SPRINT") ) {
			noclip = true;
			speed = runSpeed;
			if ( controls->isHeld("PLAYER_CROUCH") ) {
				speed = 1000;
			}
		}
	}
	
	//Reset velocity
	vx = 0;
	vy = 0;
	vz = 0;
	
	//Set velocity components from movement
	double mv1 = 0;
	double mv2 = 0;
	double mv3 = 0;
	if( controls->isHeld("PLAYER_MOVE_NORTH")	) mv2 = -speed;		//-mv2: Move 'up' from this perspective
	if( controls->isHeld("PLAYER_MOVE_SOUTH")	) mv2 =  speed;		// mv2: Move 'down' from this perspective
	if( controls->isHeld("PLAYER_MOVE_WEST")	) mv1 = -speed;		//-mv1: Move 'left' from this perspective
	if( controls->isHeld("PLAYER_MOVE_EAST")	) mv1 =  speed;		// v1: Move 'right' from this perspective
	if( controls->isHeld("PLAYER_MOVE_UP")		) mv3 = -speed;		//-v3: Move 'backward' from this perspective
	if( controls->isHeld("PLAYER_MOVE_DOWN")	) mv3 =  speed;		// v3: Move 'forward' from this perspective

	//Depending on camera direction, apply velocity in the appropriate direction.
	switch(camera.getDirection()) {
		case Camera::NORTH:
		case Camera::SOUTH: {
			vx = mv1; vy = mv3;
			if(godMode) vz = mv2;
		} break;
		
		case Camera::EAST:
		case Camera::WEST: {
			vx = mv3; vy = mv1;
			if(godMode) vz = mv2;
		} break;
		
		case Camera::DOWN:
		case Camera::UP: {
			vx = mv1; vy = mv2;
			if(godMode) vz = mv3;
		} break;
	}
}

void Player::collision(TileMap* tm)
{
	//Reset snap states
	snapS = 0; snapN = 0; snapE = 0; snapW = 0;
	snapD = 0; snapU = 0;

	//If player is intangible, do no collision.
	if(noclip) {
		return;
	}

	//Store location of tile being collided with
	int64_t cx = 0;
	int64_t cy = 0;
	int64_t cz = 0;
	
	//Determine whether to snap to SOUTH, NORTH (y), EAST, WEST (x), or DOWN (z).
	if(tm->collides(getBounds(SOUTH), cx, cy, cz)) 	collisionSnap(cy, 10, y, snapS);
	if(tm->collides(getBounds(NORTH), cx, cy, cz)) 	collisionSnap(cy+1, -10, y, snapN);
	if(tm->collides(getBounds(EAST), cx, cy, cz)) 	collisionSnap(cx, 10, x, snapE);
	if(tm->collides(getBounds(WEST), cx, cy, cz)) 	collisionSnap(cx+1, -10, x, snapW);
	if(tm->collides(getBounds(LOWER), cx, cy, cz)) 	collisionSnap(cz, 64, z, snapD);

	//Determine up/ceiling snap (special case)
	if(tm->collides(getBounds(UPPER), cx, cy, cz)) {
		//If head nearly matches up with the ceiling, teleport player to be right under the ceiling.
		double headDepth = z+0.125;
		if( cz+1>headDepth-0.1 && cz+1<headDepth+0.1 ) {
			z = cz+0.875;
			snapU = true;
		}
	}
}

void Player::collisionSnap(int64_t tileCoord, double depthMod, double& playerCoordRef, bool& snapRef)
{
	double depth = playerCoordRef+unit*depthMod;
	if(tileCoord>depth-0.1 && tileCoord<depth+0.1) {
		playerCoordRef = tileCoord-unit*depthMod;
		snapRef = true;
	}
}

void Player::putInfo(std::stringstream& ss, int& tabs)
{
	DebugScreen::indentLine(ss, tabs);
	ss << "XYZ=(" << x << ", " << y << ", " << z << "); ";
	DebugScreen::newLine(ss);

	if(true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "vXYZ=(" << vx << ", " << vy << ", " << vz << "); ";
		DebugScreen::newLine(ss);
	}

	if(true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "snap(NESWUD)=(" << snapN << ", " << snapE << ", " << snapS << ", " << snapW << ", " << snapU << ", " << snapD << ")";
	}

	if(!true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "(anWalkFrameX, anWalkState)=(" << anWalkFrameX << ", " << anWalkState << "); ";
		ss << "(walkSpeed)=(" << walkSpeed << "); ";
		DebugScreen::newLine(ss);
	}

}

int Player::getAction() { return action; }

Box3D Player::getBounds(int bbt)
{
	Box3D b3d;
	b3d.c1.x = x-unit*11.;	b3d.c2.x = x+unit*11.;
	b3d.c1.y = y-unit*11.;	b3d.c2.y = y+unit*11.;
	b3d.c1.z = z+4.*unit;	b3d.c2.z = z+2;

	switch (bbt) {
	case MAIN: 	{ } break;
	case EAST: 	{
		b3d.c1.x = x+unit*11.;
		b3d.c1.y = y-unit*10.;	b3d.c2.y = y+unit*10.;
	} break;
	case WEST: 	{
		b3d.c2.x = x-unit*11.;
		b3d.c1.y = y-unit*10.;	b3d.c2.y = y+unit*10.;
	} break;
	case SOUTH: {
		b3d.c1.x = x-unit*10.;	b3d.c2.x = x+unit*10.;
		b3d.c1.y = y+unit*11.;
	} break;
	case NORTH: {
		b3d.c1.x = x-unit*10.;	b3d.c2.x = x+unit*10.;
		b3d.c2.y = y-unit*11.;
	} break;

	case LOWER: {
		b3d.c1.x = x-unit*10.;	b3d.c2.x = x+unit*10.;
		b3d.c1.y = y-unit*10.;	b3d.c2.y = y+unit*10.;
		b3d.c1.z = z+unit*65.;	b3d.c2.z = z+unit*65.;
	} break;
	case UPPER: {
		b3d.c1.x = x-unit*10.;	b3d.c2.x = x+unit*10.;
		b3d.c1.y = y-unit*10.;	b3d.c2.y = y+unit*10.;
		b3d.c1.z = z+unit*3.;	b3d.c2.z = z+unit*3.;
	} break;
	default:
		Log::warnv(__PRETTY_FUNCTION__, "returning default Box3D", "Invalid bounding box type %d", bbt);
		break;
	}

	return b3d;
}
Box3D Player::getBounds() { return getBounds(MAIN); }


std::tuple<double, double, double> Player::getPos() { return std::make_tuple(x, y, z); }
std::tuple<double, double, double> Player::getVelComponents() { return std::make_tuple(vx, vy, vz); }
double Player::getVel() { return sqrt( vx*vx + vy*vy + vz*vz ); }
Camera* Player::getCamera() { return &camera; }
bool Player::inGodMode() { return godMode; }

void Player::setPos(double x, double y, double z)
{
	Player::x = x;
	Player::y = y;
	Player::z = z;
}

void Player::updateFacingDirection()
{
	if( vy<0 ) {
		facing = Camera::NORTH;
	} else if( vy>0 ) {
		facing = Camera::SOUTH;
	}

	if( vx<0 ) {
		facing = Camera::WEST;
	} else if( vx>0 ) {
		facing = Camera::EAST;
	}
}

void Player::rebuildPlayerTex(Texture& tex, bool alt)
{
	//Get sprite sheet texture
	Texture* sst = spsh.getSheetTexture();
	
	if(!alt) {
		tex.clear();
		tex.setTexDimensions(32, 32);
	} else {
		tex.clear();
		tex.setTexDimensions(32, 64);
		tex.setDrawScale(4);
	}
	
	if(!alt) {
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
		if(walkSpeed>0) {
			tex.lock();
			tex.blitEx(sst, 0, 32*TOP_ARMS, 32, 32, rotation);
		}
		
		
		/* Head */
		tex.lock();
		tex.blitEx(sst, 0, 32*TOP_HAIR, 32, 32, rotation);
	} else {
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
			if( facing==Camera::SOUTH || facing==Camera::NORTH ) {
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