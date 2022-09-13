#include "Player.h"
#include <math.h>
#include <iostream>
#include "DebugScreen.h"
#include "Log.h"
#include "SpriteSheetBuilder.h"
#include "TextureLoader.h"
#include "TileMap.h"

Player::Player() { }
void Player::init(SDLHandler* sh, Controls* ctrls)
{
    BTEObject::init(sh, nullptr, ctrls);
    camera.init(ctrls);

    /** Build player spritesheet */
    playerPal.init(playerPal.DEFAULT_PLAYER);

    /** Build sprite sheet */
    SpriteSheetBuilder ssb(sdlHandler);
    ssb.buildSpriteSheet(ssb.DEFAULT_PLAYER, spsh, playerPal);

    /** Build player textures */
    playerTex.init(sdlHandler);
    playerTex.setTexDimensions(32, 64);

    /** Build bounding box */

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
    /** Frames */

    /** Rebuild player texture */
    playerTex.clear();
    Texture* sst = spsh.getTexture();

    int flip = sst->Flip::NONE;
    if(facing==WEST) {
        flip = sst->Flip::H;
    }


    //Draw head base
    playerTex.lock(0, 0, 32, 32);
    //playerTex.blit(sst, 0, 32, 32, 32);
    playerTex.blitEx(sst, 0, 32, 32, 32, flip);

    //Draw eyes layer 1
    if(timerBlink<0) {
        playerTex.lock(0, 0, 32, 32);
        playerTex.blitEx(sst, 0, 64, 32, 32, flip);
    }

    //Draw eyes layer 2
    if(timerBlink<0) {
        playerTex.lock(0, 0, 32, 32);
        playerTex.blitEx(sst, 0, 96, 32, 32, flip);
    }

    //Draw lips
    playerTex.lock(0, 0, 32, 32);
    playerTex.blitEx(sst, 0, 128, 32, 32, flip);

    //Draw Arms
    int aTX = (anWalkFrame%2);
    playerTex.lock(0+aTX, 16, 32+aTX, 32);
    playerTex.blitEx(sst, 0, 160, flip);

    //Draw Lower body
    playerTex.lock(0, 29, 32, 32);
    playerTex.blitEx(sst, 32*anWalkFrame, 224, 32, 32, flip);

    //Draw Middle body
    playerTex.lock(0+aTX, 9, 32+aTX, 32);
    playerTex.blitEx(sst, 0, 192, 32, 32, flip);

    //Draw Feet
    playerTex.lock(1, 48, 32, 32);
    playerTex.blitEx(sst, 0, 256, 31, 32, flip);

    //Draw hair
    playerTex.lock(0, 0, 32, 32);
    playerTex.blitEx(sst, 0, 0, 32, 32, flip);

    //If player is inside a tile, make sure part of the player isn't rendered



    /** Draw player texture to canvas */
    //playerTex.rect(0, 0, 32, 64, 255, 0, 0);
    csEntities->setSrc(&playerTex);
    csEntities->rcopy(x*32, y*32, 32, 64);
}

void Player::tick()
{
    if( Commands::cmdIntResult("tele")!=nullptr ) {
        x = (*Commands::cmdIntResult("tele.x"));
        y = (*Commands::cmdIntResult("tele.y"));
        Commands::resetCMDEntered(__PRETTY_FUNCTION__);
    }

    age++;

    /** Animations */
    /* Blinking */
    if(timerBlink==-1) {
        if( rand()%240==0 ) timerBlink++;
    } else {
        timerBlink++;
    }
    if(timerBlink>12) {
        timerBlink = -480;
    }

    /**
        Walking

    */
    //If the player is moving horizontally
    anTimer++;

    //walk velocity
    double walkingVelocity = abs(vx);

    //Set player direction / Make sure walkingVelocity is always positive
    if( vx<0 ) {
        facing = Directions::WEST;
    } else
    if( vx>0 )
    {
        facing = Directions::EAST;
    }

    if( anTimer>6.0/walkingVelocity*0.05 ) {
        anTimer = 0;
        if( walkingVelocity>=0.02 ) {
            if(anWalkState==0) anWalkState = 1;
            anWalkFrame += anWalkState;
            if( anWalkState==1 && anWalkFrame==10 ) {
                anWalkFrame = 3;
            }
        } else {
            //Reset aWalkState
            anWalkState = 0;

            if( anWalkFrame>0 ) anWalkFrame-=1;
            if( anWalkFrame>0 ) anWalkFrame-=1;
        }
    }


    //Physics
    x+=vx;
    y+=vy;
    z+=vz;




    /** God mode actions */
    action = NONE;
    if( godMode ) {
        if( controls->isHeld("HARDCODE_RIGHT_CLICK") ) {
            action = Action::GM_Place_Tile;
        } else if( controls->isHeld("HARDCODE_LEFT_CLICK") ) {
            action = Action::GM_Destroy_Tile;
        }
    }

    /** Player movement */
    if( !camera.isFreecam() ) {

        double speed = 0.05;
        if( controls->isHeld("PLAYER_SPRINT") ) speed = 0.10;
        if( controls->isHeld("PLAYER_CROUCH") ) speed = 0.005;
        if( godMode ) {
            if( controls->isHeld("PLAYER_SPRINT") ) {
                speed = 3;
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
            if( controls->isHeld("PLAYER_MOVE_UP")   ) vz =  speed;
            if( controls->isHeld("PLAYER_MOVE_DOWN") ) vz = -speed;
        }
    }

    /** Camera set to player's (x, y, z) - keep as last */
    camera.tick();
    double tx = 0.5;
    double ty = 1.0;
    camera.setXYZ( (int)(x*32.0)/32.0+tx, (int)(y*32.0)/32.0+ty, z+1);

    /** Set bounding box */
}

void Player::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << "Player(x, y, z)=(" << x << ", " << y << ", " << z << "); ";
    DebugScreen::newLine(ss);
}

int Player::getAction() { return action; }
std::tuple<double, double, double> Player::getPos() { return std::make_tuple(x, y, z); }
std::tuple<double, double, double> Player::getVelComponents() { return std::make_tuple(vx, vy, vz); }
double Player::getVel() { return sqrt( vx*vx + vy*vy + vz*vz ); }
Camera* Player::getCamera() { return &camera; }
