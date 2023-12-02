#include "PlayerMenu.h"
#include <cmath>

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;
	
	for( int x = 0; x<invW; x++ ) {
		for( int y = 0; y<invH; y++ ) {
			inventorySlots[x][y] = -1;
		}
	}
	
	inventorySlots[0][0] = 0;
	//inventorySlots[0][4] = 1;
	//inventorySlots[0][5] = 1;
	//inventorySlots[1][4] = 1;
	//inventorySlots[2][4] = 1;
	
	uiOverlay.init(sdlHandler, 0, 0);
}

void PlayerMenu::tick() {
	updateMenuCoordinates();
	
	if(state==1) {
		if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
			int imx = (controls->getMouseX()-invScreenX)/64;
			int imy = (controls->getMouseY()-invScreenY)/64;
			if( 
				controls->getMouseX()<invScreenX ||
				controls->getMouseX()>=invScreenX+invScreenW ||
				controls->getMouseY()<invScreenY ||
				controls->getMouseY()>=invScreenY+invScreenH
			) {
				imx = -1;
				imy = -1;
			}
		
			std::cout << imx << ", " << imy << "\n";
		
			controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
		}
	}
}

void PlayerMenu::draw()
{
	double angle = ((SDL_GetTicks()/30)%360)*3.141/180.0;
	oscillation = 32*std::abs(std::sin(angle));
	
	if( state>0 ) {
	uiOverlay.clear();
	
	//Inventory
	if(state==1) {
		uiOverlay.clear();
		uiOverlay.setTexDimensions(invW*32, invH*32);
		uiOverlay.setDrawPos(invScreenX-2, invScreenY-2);
		uiOverlay.setDrawScale(2);
		
		Color box1(16+oscillation, 16+oscillation, 0, 100);
		Color box2(0, 32+oscillation, 0, 100);
		Color box3(0, 0, 0, 100);
		Color ridge1(0, 0, 0, 100);
		Color ridge2(128+oscillation, 128+oscillation, 128+oscillation, 100);
		
		for( int x = 0; x<invW; x++ ) {
			for( int y = 0; y<invH; y++ ) {
				//Rect containing 4 ridges and a 30x30 texture
				if(inventorySlots[x][y]!=-1) {
					if((x+y)%2==1) {
						uiOverlay.rect(x*32, y*32, 32, 32, box1);
					} else {
						uiOverlay.rect(x*32, y*32, 32, 32, box2);
					}
					
					uiOverlay.lock(x*32, y*32, 32, 32);
					uiOverlay.blit(TextureLoader::PLAYER_items, 0, 0);
				} else {
					uiOverlay.rect(x*32, y*32, 32, 32, box3);
				}

				
				//Top left corner: intersect with 2 ridges
				uiOverlay.rect(x*32, y*32, 1, 32, ridge1);
				uiOverlay.rect(x*32, y*32, 32, 1, ridge1);
				//Bottom left corner: intersect with 2 ridges
				uiOverlay.rect(x*32, y*32+31, 32, 1, ridge2);
				uiOverlay.rect(x*32+31, y*32, 1, 32, ridge2);
			}
		}
		
		//Outline
		uiOverlay.rect(0, 0, invW*32, 1, 0, 0, 0, 0);
		uiOverlay.rect(0, 0, 1, invH*32, 0, 0, 0, 0);
		uiOverlay.rect(0, invH*32-1, invW*32, 1, 0, 0, 0, 0);
		uiOverlay.rect(invW*32-1, 0, 1, invH*32, 0, 0, 0, 0);
	}

	uiOverlay.draw();
	}
	
}

int PlayerMenu::getState() { return state; }

void PlayerMenu::setState(int newState)
{
	state = newState;
	
	if(state==0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
		uiOverlay.clear();
	}
	
	if(state>0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open);
		updateMenuCoordinates();
	}
	
	//Engineering
	if(state==2) {
	
	}
}

void PlayerMenu::updateMenuCoordinates()
{
	windowW = 0; windowH = 0;
	windowX = 0; windowY = 0;
	invScreenW = 0; invScreenH = 0;
	invScreenX = 0; invScreenY = 0;
	
	Window* parentWindow = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( parentWindow!=nullptr ) {
		windowW = parentWindow->getWidth();
		windowH = parentWindow->getHeight();
		windowX = parentWindow->getSX();
		windowY = parentWindow->getSY();
		
		invScreenX = windowX+32*20+2*6;
		invScreenY = windowY+2*4;
		invScreenW = invW*32*2;
		invScreenH = invH*32*2;
	}
}