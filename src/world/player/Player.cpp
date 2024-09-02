#include "Player.h"
#include <cmath>
#include <math.h>
#include <nch/cpp-utils/io/Log.h>
#include <iomanip>
#include <iostream>
#include "DebugScreen.h"
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

	plan.init(sdlHandler, settings, &camera);
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
	plan.destroy();
}

void Player::draw(Canvas* csEntities, bool debugging)
{
	plan.draw(csEntities, debugging, noclip, x, y, z);
}

void Player::drawCharInMenu() { plan.drawCharInMenu(); }
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

		//Oxygen tank icons
		if(spaceSuitState!=NO_SUIT) {
			hud.lock( 0, hudH-128, 32, 64 );
			hud.blit( TextureLoader::GUI_player_interactions, 160, 0 );
			hud.lock( 32, hudH-128, 32, 64 );
			hud.blit( TextureLoader::GUI_player_interactions, 160, 0 );
		}

		//Draw HUD
		hud.draw();
	}
}

void Player::tick(TileMap* tm)
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
	if(spaceSuitState!=SpaceSuitStates::STABLE) {
		oxygen -= 0.015;
		AudioLoader* al = sdlHandler->getAudioLoader();
		al->playOnce(AudioLoader::SFX_WORLD_air_release_1);
	} else {
		oxygen += 0.1;
	}
	//Losing water - 100 points. 0.0006/tick = lasts 69.4min = 2.89 earth days. May be shorter depending on heat
	water -=  0.0006;
	//Losing food - 100 points. 0.0000375/tick = lasts 1111.1min = 46.29 earth days. May be shorter depending on body activity
	nutrition -= 0.0000375;

	if(oxygen<20) {
		health--;
	}

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
	} else {
		if( controls->isHeld("HARDCODE_RIGHT_CLICK") ) {
			action = Action::SURV_Place_Tile;
		} else if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
			action = Action::SURV_Destroy_Tile;
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
	


/** Player acceleration, velocity, and position */
//If necessary, set downward accel. based on planet gravity
	if(!noclip && !snapD) {
		az = 9.81/1600.;	//9.81m/(40ticks)^2 = 9.81m/sec^2
	}
	if(noclip) {
		az = 0;
	}
//Based on snap calculated last tick cancel certain acceleration and velocity if necessary.
	if( (snapE && (vx>0||ax>0)) || (snapW && (vx<0||ax<0)) ) { 	vx = 0; ax = 0; }	//If snapped East/West
	if( (snapS && (vy>0||ay>0)) || (snapN && (vy<0||ay<0)) ) {	vy = 0; ay = 0;	}	//If snapped South/North
	if( (snapD && (vz>0||az>0)) || (snapU && (vz<0||az<0)) ) {
		vz = 0; az = 0;
	}	//If snapped Down/Up
//vx, vy, vz velocity components added to x, y, z
	vx += ax;	vy += ay;	vz += az;	//Add acceleration to velocity
	applyVelAndCollision(tm);			//Add velocity to position



/** Visuals */
//Player animation
	//Tick animation
	plan.tick(vx, vy, vz);

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
	double cx = ((int)(x*32.0))/32.0;
	double cy = ((int)(y*32.0))/32.0;
	double cz = z;
	//Set camera to appropriate coordinates
	camera.setXYZ(cx+dcx, cy+dcy, cz+dcz);
}

void Player::tickControls()
{
	noclip = false;
	double speed = 0.085;
	if( controls->isHeld("PLAYER_SPRINT") ) speed = 0.170;
	if( controls->isHeld("PLAYER_CROUCH") ) speed = 0.009;
	if( godMode ) {
		if( controls->isHeld("PLAYER_SPRINT") ) {
			speed = runSpeed;
			if ( controls->isHeld("PLAYER_CROUCH") ) {
				speed = 1000;
			}
		}
		if(controls->isHeld("PLAYER_NOCLIP")) {
			noclip = true;
		}
	}

	//When the player wants to jump and is standing on the ground (snapD == standing on ground)
	if( snapD && controls->isHeld("PLAYER_JUMP") ) {
		//If hugging wall, slightly increase jump
		if( snapN||snapE||snapS||snapW ) {
			vz = -0.16;
		//If not next to a wall, do a lower jump
		} else {
			vz = -0.1;
		}
		snapD = false;
	}
	
	//Reset velocity
	vx = 0;
	vy = 0;
	
	//Set velocity components from movement
	double mv1 = 0;
	double mv2 = 0;
	double mv3 = 0;
	if( controls->isHeld("PLAYER_MOVE_NORTH")	) mv2 = -speed;		//-mv2: Move 'forward' from this perspective
	if( controls->isHeld("PLAYER_MOVE_SOUTH")	) mv2 =  speed;		// mv2: Move 'backward' from this perspective
	if( controls->isHeld("PLAYER_MOVE_WEST")	) mv1 = -speed;		//-mv1: Move 'left' from this perspective
	if( controls->isHeld("PLAYER_MOVE_EAST")	) mv1 =  speed;		// v1: Move 'right' from this perspective
	if( controls->isHeld("PLAYER_JUMP")			) mv3 = -speed;		//-v3: Move vertically up

	//Depending on camera direction, apply velocity in the appropriate direction.
	switch(camera.getDirection()) {
		default: {
			vx = mv1;
			vy = mv2;
		} break;
		case Camera::EAST: case Camera::WEST: {
			vx = mv2;
			vy = mv1;
		} break;
	}
	if(noclip) {
		vz = mv3;
	}
}

void Player::applyVelAndCollision(TileMap* tm)
{	
	int vxNumSteps = std::ceil(std::abs(vx/0.08));
	int vyNumSteps = std::ceil(std::abs(vy/0.08));
	int vzNumSteps = std::ceil(std::abs(vz/0.08));
	
	if(vxNumSteps==0) vxNumSteps = 1;
	if(vyNumSteps==0) vyNumSteps = 1;
	if(vzNumSteps==0) vzNumSteps = 1;

	for(int i = 0; i<vxNumSteps||i<vyNumSteps||i<vzNumSteps; i++) {
		collision(tm);
		if(i<vxNumSteps) x += (vx/(double)vxNumSteps);
		if(i<vyNumSteps) y += (vy/(double)vyNumSteps);
		if(i<vzNumSteps) z += (vz/(double)vzNumSteps);
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
	
	//Determine up/down snap (special case)
	if(tm->collides(getBounds(LOWER), cx, cy, cz)) 	collisionSnap(cz, 64, z, snapD);
	if(tm->collides(getBounds(UPPER), cx, cy, cz)) {
		//If head nearly matches up with the ceiling, teleport player to be right under the ceiling.
		double headDepth = z+0.125;
		if( cz+1>headDepth-0.1 && cz+1<headDepth+0.1 ) {
			z = cz+0.875;
			snapU = true;
		}
	}

	//Determine whether to snap to SOUTH, NORTH (y), EAST, WEST (x)
	if(tm->collides(getBounds(SOUTH), cx, cy, cz)) 	collisionSnap(cy, 10, y, snapS);
	if(tm->collides(getBounds(NORTH), cx, cy, cz)) 	collisionSnap(cy+1, -10, y, snapN);
	if(tm->collides(getBounds(EAST), cx, cy, cz)) 	collisionSnap(cx, 10, x, snapE);
	if(tm->collides(getBounds(WEST), cx, cy, cz)) 	collisionSnap(cx+1, -10, x, snapW);
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

	if(!true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "vXYZ=(" << vx << ", " << vy << ", " << vz << "); ";
		DebugScreen::newLine(ss);
		DebugScreen::indentLine(ss, tabs);
		ss << "aXYZ=(" << ax << ", " << ay << ", " << az << "); ";
		DebugScreen::newLine(ss);
	}

	if(!true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "snap(NESWUD)=(" << snapN << ", " << snapE << ", " << snapS << ", " << snapW << ", " << snapU << ", " << snapD << ")";
	}


	/*
	if(!true) {
		DebugScreen::indentLine(ss, tabs);
		ss << "(anWalkFrameX, anWalkState)=(" << anWalkFrameX << ", " << anWalkState << "); ";
		ss << "(walkSpeed)=(" << walkSpeed << "); ";
		DebugScreen::newLine(ss);
	}*/

}

int Player::getAction() { return action; }

Box3X<double> Player::getBounds(int bbt)
{
	Box3X<double> b3d;
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
		nch::Log::warnv(__PRETTY_FUNCTION__, "returning default Box3D", "Invalid bounding box type %d", bbt);
		break;
	}

	return b3d;
}
Box3X<double> Player::getBounds() { return getBounds(MAIN); }


std::tuple<double, double, double> Player::getPos() { return std::make_tuple(x, y, z); }
std::tuple<double, double, double> Player::getVelComponents() { return std::make_tuple(vx, vy, vz); }
double Player::getVel() { return sqrt( vx*vx + vy*vy + vz*vz ); }
Camera* Player::getCamera() { return &camera; }
bool Player::inGodMode() { return godMode; }
int Player::getGameMode() { return gameMode; }

void Player::setPos(double x, double y, double z)
{
	Player::x = x;
	Player::y = y;
	Player::z = z;
}

void Player::setMode(int newGameMode)
{
	gameMode = newGameMode;
	if(gameMode==SANDBOX) {
		godMode = true;
	} else {
		godMode = false;
		noclip = false;
	}
}

void Player::setModeFromStr(std::string newGameMode)
{
	if(newGameMode=="sandbox") {
		setMode(SANDBOX);
	} else if(newGameMode=="survival") {
		setMode(SURVIVAL);
	} else if (newGameMode=="hardcore") {
		setMode(HARDCORE);
	} else {
		nch::Log::warnv(__PRETTY_FUNCTION__, "defaulting to \"sandbox\"", "Unknown gamemode \"%s\"", newGameMode.c_str());
	}
}

void Player::setSpaceSuitState(int sss)
{
	Player::spaceSuitState = sss;
}