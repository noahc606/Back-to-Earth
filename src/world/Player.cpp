#include "Player.h"
#include <cmath>
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

    //Render spritesheet on screen if it should be shown (debug feature)
    int* show_ss = Commands::getInt("player.show_ss");
    if( show_ss!=nullptr && *show_ss!=0 ) {
        sst->setDrawPos(0, 0);
        sst->draw();
    }

    //Determine player flip depending on if they are facing west or east
    int flip = sst->Flip::NONE;
    if(facing==WEST) {
        flip = sst->Flip::H;
    }

    /** Head */
    //Head base
    playerTex.lock(0, 0, 32, 32);
    playerTex.blitEx(sst, 0, 32, 32, 32, flip);

    //Eyes layer 1
    if(anBlinkTimer<0) {
        playerTex.lock(0, 0, 32, 32);
        playerTex.blitEx(sst, 0, 64, 32, 32, flip);
    }

    //Eyes layer 2
    if(anBlinkTimer<0) {
        playerTex.lock(0, 0, 32, 32);
        playerTex.blitEx(sst, 0, 96, 32, 32, flip);
    }

    //Lips
    playerTex.lock(0, 0, 32, 32);
    playerTex.blitEx(sst, 0, 128, 32, 32, flip);

    /** Arms */
    //Arms
    int aTX = (anWalkFrameX%2);
    playerTex.lock(0+aTX, 16, 32+aTX, 32);
    playerTex.blitEx(sst, 0, 160, flip);

    /** Body */
    //Legs

    playerTex.lock(0, 29, 32, 32);
    if( anWalkState!=0 ) {
        int dy = 1;
        if( facing==SOUTH || facing==NORTH ) {
            dy = 2;
        }
        playerTex.blitEx(sst, 32*anWalkFrameX, (LOWER_BODY+dy)*32, 32, 32, flip);
    } else {
        playerTex.blitEx(sst, 32*anWalkFrameX, (LOWER_BODY)*32, 32, 32, flip);
    }
    //Middle body
    {
        playerTex.lock(0+aTX, 9, 32+aTX, 32);

        playerTex.blitEx(sst, 0, (MIDDLE_BODY)*32, 32, 32, flip);
    }


    /** Extra */
    //Shoes
    playerTex.lock(1, 48, 32, 32);
    playerTex.blitEx(sst, 0, FEET*32, 31, 32, flip);

    //Hair
    playerTex.lock(0, 0, 32, 32);
    playerTex.blitEx(sst, 0, 0, 32, 32, flip);

    //If player is inside a tile, make sure part of the player isn't rendered



    /** Draw player texture to canvas */
    //playerTex.rect(0, 0, 32, 64, 255, 0, 0);
    csEntities->setSourceTex(&playerTex);
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
    if( anTimer>6.0/(walkSpeed/0.05) ) {
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

    /** Player control */
    //Movement (sprint/crouch, walk)
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
            if( controls->isHeld("PLAYER_MOVE_UP")   ) vz = -speed;
            if( controls->isHeld("PLAYER_MOVE_DOWN") ) vz = speed;
        }
    }
    //Open inventory
    if( controls->isPressed("PLAYER_INVENTORY") ) {
        if(charMenuOpen) {
            charMenuOpen = false;
        } else {
            charMenuOpen = true;
        }

        controls->stopPress("PLAYER_INVENTORY", __PRETTY_FUNCTION__);
    }

    /** Camera set to player's (x, y, z) - keep as last */
    camera.tick();
    double tx = 0.5;
    double ty = 1.0;
    camera.setXYZ( (int)(x*32.0)/32.0+tx, (int)(y*32.0)/32.0+ty, z-1);

    /** Set bounding box */
}

void Player::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << "Player(x, y, z)=(" << x << ", " << y << ", " << z << "); ";
    ss << "Player(vx, vy, vz)=(" << vx << ", " << vy << ", " << vz << "); ";
    DebugScreen::newLine(ss);

    DebugScreen::indentLine(ss, tabs);
    ss << "Player(anWalkFrameX, anWalkState)=(" << anWalkFrameX << ", " << anWalkState << "); ";
    ss << "Player(walkSpeed)=(" << walkSpeed << "); ";
    DebugScreen::newLine(ss);
}

int Player::getAction() { return action; }
std::tuple<double, double, double> Player::getPos() { return std::make_tuple(x, y, z); }
std::tuple<double, double, double> Player::getVelComponents() { return std::make_tuple(vx, vy, vz); }
double Player::getVel() { return sqrt( vx*vx + vy*vy + vz*vz ); }
Camera* Player::getCamera() { return &camera; }
bool Player::isCharMenuOpen() { return charMenuOpen; }

void Player::setPos(double x, double y, double z)
{
    Player::x = x;
    Player::y = y;
    Player::z = z;
}

void Player::updateFacingDirection()
{
    if( vy<0 ) {
        facing = Directions::NORTH;
    } else
    if( vy>0 ) {
        facing = Directions::SOUTH;
    }

    if( vx<0 ) {
        facing = Directions::WEST;
    } else
    if( vx>0 ) {
        facing = Directions::EAST;
    }
}
