#include "PlayerAnimation.h"
#include "SpriteSheetBuilder.h"

void PlayerAnimation::init(SDLHandler* sh, Settings* stngs, Camera* cam)
{
    sdlHandler = sh;
    camera = cam;
    settings = stngs;

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
}

void PlayerAnimation::destroy()
{
    playerTex.destroy();
}

void PlayerAnimation::tick(double vx, double vy, double vz)
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
	updateFacingDirection(vx, vy, vz);

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

void PlayerAnimation::draw(Canvas* csEntities, bool debugging, bool noclip, double x, double y, double z)
{
	//Get sprite sheet texture
	Texture* sst = spsh.getSheetTexture();
	
	//Render spritesheet on screen if it should be shown (debug feature)
	int* show_ss = Commands::getInt("player.show_ss");
	if( show_ss!=nullptr && *show_ss!=0 ) {
		sst->setDrawPos(0, 0);
		sst->draw();
	}



	/*
	 *	Rebuild player texture from spritesheet
	 *	
	 *	We rebuild the player's texture every frame.
	 *	Meanwhile, the spritesheet was built once during player initialization.
	 */
	bool cameraHorizontal = false;
	if(camera->getAxis()!=Camera::Z) {
		cameraHorizontal = true;
	}

	//Player may be slightly translucent if in noclip
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
	switch(camera->getAxis()) {
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

void PlayerAnimation::updateFacingDirection(double vx, double vy, double vz)
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

void PlayerAnimation::drawCharInMenu()
{
	//Build player texture
	rebuildPlayerTex(playerTexAlt, true);
	
	int ptaX = sdlHandler->getWidth()/2+(-32+2)*4;
	int ptaY = sdlHandler->getHeight()/2+(-64+10)*4;
	playerTexAlt.setDrawPos(ptaX, ptaY);
	playerTexAlt.setDrawScale(4);
	playerTexAlt.draw();
}

void PlayerAnimation::rebuildPlayerTex(Texture& tex, bool alt)
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