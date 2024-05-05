#include "PlayerMenu.h"
#include <cmath>
#include "TextBox.h"
#include "Tooltip.h"

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;

	items.init(guiHandler, pl);

	for( int x = 0; x<invW; x++ ) {
		for( int y = 0; y<invH; y++ ) {
			inventorySlots[x][y] = -1; 
		}
	}
	
	inventorySlots[3][0] = Items::ION_BLASTER;
	inventorySlots[4][1] = Items::GEOPORTER;
	inventorySlots[6][2] = Items::SOLID_PLASMA_ANNIHILATOR;
	inventorySlots[6][3] = Items::ATOM_PRINTER;
	inventorySlots[7][0] = Items::NYLON_EXOSUIT_HELMET;
	inventorySlots[7][1] = Items::NYLON_EXOSUIT_BODY;
	inventorySlots[7][2] = Items::NYLON_EXOSUIT_LEGGINGS;
	
	inventorySlots[7][7] = Items::FOOD_RATION_F;
	inventorySlots[7][6] = Items::FOOD_RATION_F;
	inventorySlots[7][5] = Items::FOOD_RATION_A;
	inventorySlots[7][4] = Items::FOOD_RATION_B;
	inventorySlots[7][3] = Items::FOOD_RATION_C;
	inventorySlots[6][7] = Items::FOOD_RATION_D;
	inventorySlots[6][6] = Items::FOOD_RATION_E;
	uiOverlay.init(sdlHandler, 0, 0);
	
	movingItemA.init(sdlHandler, 32, 32);
	movingItemA.setDrawScale(2);
	movingItemB.init(sdlHandler, 32, 32);
	movingItemB.setDrawScale(2);
}

void PlayerMenu::tick() {
	updateMenuCoordinates();
	
	if(lastState!=state) {
		lastState = state;
		
		if(state==1) {
			items.putItemInterface(-1);
		}
		if(state==2) {
			items.putItemInterface(Items::SANDBOX);
		}
	}
	
	if(itemMoveTimer>=0) {
		itemMoveTimer = -1;
	}
	
	//If backpack or engineering is opened
	if(state==1 || state==2) {
		//Upon click
		if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
			//Get inventory slot that was clicked
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
			
			//If we clicked outside of the inventory, do nothing.
			if( imx!=-1 && imy!=-1 ) {
				//If the clicked slot was already selected, reset selection
				if( invSX==imx && invSY==imy && state==1 ) {
					selectInventorySlot(-1, -1);
					newItemSelected = true;
				} else {
					//Set selected inventory slot to the clicked slot
					selectInventorySlot(imx, imy);
					newItemSelected = true;
				}
			}
			
			controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
		}
		
		//Upon right click
		if( controls->isPressed("HARDCODE_RIGHT_CLICK") ) {
			//Deselect item
			invSX = -1; invSY = -1;
			invLSX = -1; invLSY = -1;
			
			controls->stopPress("HARDCODE_RIGHT_CLICK", __PRETTY_FUNCTION__);
		}
	
		//If item selection has changed
		if( newItemSelected && state==1 ) {
			items.putItemInterface( inventorySlots[invSX][invSY] );
			newItemSelected = false;
		}
	}

	if( state==2 ) {
		
		for( int i = 0; i<3; i++ ){
			auto tbx = guiHandler->getGUI( BTEObject::GUI_textbox, GUIHandler::tbx_CHARACTER_item, 2000+i );
			if(tbx!=nullptr) {
				try {
					std::string colorString = ((TextBox*)tbx)->getString();
					int colorInt = std::stoi(colorString);
					if(colorInt>255) {
						colorInt = 255;
					}
					
					Color newcolor = items.getSandboxRGB();
					switch(i) {
						case 0: newcolor.r = colorInt; break;
						case 1: newcolor.g = colorInt; break;
						case 2: newcolor.b = colorInt; break;
					}
					items.setSandboxRGB(newcolor.r, newcolor.g, newcolor.b);
					
				} catch(...) { }
			}
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
		drawInventory();
	}
	
	if(state==2) {
		uiOverlay.setTexDimensions(invW*32, invH*32);
		uiOverlay.setDrawPos(invScreenX-2, invScreenY-2);
		uiOverlay.setDrawScale(2);
		
		uiOverlay.lock();
		uiOverlay.blit(TextureLoader::WORLD_TILE_type_a);
		
		//Selected Slot Outline
		if( invSX!=-1 && invSY!=-1 ) {
			uiOverlay.lock(invSX*32, invSY*32, 32, 32);
			uiOverlay.blit(TextureLoader::GUI_player_interactions, 0, 0 );
		}
	}

	uiOverlay.draw();
	}
	
}

int PlayerMenu::getState() { return state; }
uint8_t PlayerMenu::getSandboxTexX() { return sandboxTexX; } 
uint8_t PlayerMenu::getSandboxTexY() { return sandboxTexY; } 
uint8_t PlayerMenu::getSandboxTexRed() { return items.getSandboxRGB().r; } 
uint8_t PlayerMenu::getSandboxTexGreen() { return items.getSandboxRGB().g; } 
uint8_t PlayerMenu::getSandboxTexBlue() { return items.getSandboxRGB().b; } 

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
}

void PlayerMenu::drawInventory()
{
	//Set texture settings for uiOverlay
	uiOverlay.clear();
	uiOverlay.setTexDimensions(invW*32, invH*32);
	uiOverlay.setDrawPos(invScreenX-2, invScreenY-2);
	uiOverlay.setDrawScale(2);
	
	//Set color of inventory slot ridges
	Color ridge1(0, 0, 0, 100);
	Color ridge2(128+oscillation, 128+oscillation, 128+oscillation, 100);
	
	//Draw inventory elements
	for( int x = 0; x<invW; x++ ) {
		for( int y = 0; y<invH; y++ ) {
			
			//Skip items that are moving
			bool drawingItem = true;
			if( itemMoveTimer>=0 && ((x==invSX&&y==invSY) || (x==invLSX&&y==invLSY)) ) {
				drawingItem = false;
			}
			
			//Rect containing 4 ridges and a 30x30 texture
			Color box1(16+oscillation, 16+oscillation, 0, 100);
			Color box2(0, 32+oscillation, 0, 100);
			Color box3(0, 0, 0, 100);
			if(inventorySlots[x][y]!=-1) {
				//Set box1, box2, box3 to be a different color if this inventory spot is selected
				if( invSX==x && invSY==y ) {
					box1.r += 120; box1.b += 120;
					box2.r += 120; box2.b += 120;
					box3.r += 120; box3.b += 120;
				}

				//Draw box1 or box2 in a checkerboard fashion
				if((x+y)%2==1) { uiOverlay.rect(x*32, y*32, 32, 32, box1); }
				else { uiOverlay.rect(x*32, y*32, 32, 32, box2); }
				
				//Draw item
				if( drawingItem ) {
					uiOverlay.lock(x*32, y*32, 32, 32);
					uiOverlay.blit(TextureLoader::PLAYER_items, (inventorySlots[x][y]%8)*32, (inventorySlots[x][y]/8)*32);
				}
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
	
	//Inventory Slot Outline
	uiOverlay.rect(0, 0, invW*32, 1, 0, 0, 0, 0);
	uiOverlay.rect(0, 0, 1, invH*32, 0, 0, 0, 0);
	uiOverlay.rect(0, invH*32-1, invW*32, 1, 0, 0, 0, 0);
	uiOverlay.rect(invW*32-1, 0, 1, invH*32, 0, 0, 0, 0);
	
	//Selected Slot Outline
	if( invSX!=-1 && invSY!=-1 ) {
		if( inventorySlots[invSX][invSY]!=-1 ) {
			uiOverlay.lock(invSX*32, invSY*32, 32, 32);
			uiOverlay.blit(TextureLoader::GUI_player_interactions, 0, 0 );
		}
	}
	
	if(itemMoveTimer>=0) {
		//movingItemA.draw();
		//movingItemB.draw();
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

void PlayerMenu::moveItemsBetween(int x1, int y1, int x2, int y2)
{
	//Physically switch items
	int temp = inventorySlots[x1][y1];
	inventorySlots[x1][y1] = inventorySlots[x2][y2];
	inventorySlots[x2][y2] = temp;
	
	//Moving item A texture (lastInvSelect)
	movingItemA.clear();
	if( invLSX!=-1 && invLSY!=-1 ) {
		movingItemA.lock();
		movingItemA.blit( TextureLoader::PLAYER_items, 32*inventorySlots[invLSX][invLSY], 0 );
	}
	
	//Moving item B texture (invSelect)
	movingItemB.clear();
	if( invSX!=-1 && invSY!=-1 ) {
		movingItemB.lock();
		movingItemB.blit( TextureLoader::PLAYER_items, 32*inventorySlots[invSX][invSY], 0 );
	}
	
	//Set item move timer and invLSX,Y
	itemMoveTimer = 30;
	invLSX = x1;
	invLSY = y1;
}

void PlayerMenu::selectInventorySlot(int x, int y)
{
	bool outofbounds = true;
	if( x>=0 && x<=7 && y>=0 && y<=7 ) {
		outofbounds = false;
	}
	
	bool itemSelected = false;
	if( inventorySlots[invSX][invSY]!=-1 ) {
		itemSelected = true;
		newItemSelected = true;
	}
	
	
	if( !outofbounds) {
		if( state==1 && itemSelected && invSX!=-1 && invSY!=-1 && !(x==invSX && y==invSY) ) {
			moveItemsBetween(x, y, invSX, invSY);
		}
		
		if( state==2 ) {
			sandboxTexX = x;
			sandboxTexY = y;
		}
		
		invSX = x;
		invSY = y;
	} else {
		invSX = -1;
		invSY = -1;
	}
}