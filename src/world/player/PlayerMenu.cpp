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
}

void PlayerMenu::tick() {
	//Update menu coords and dimensions
	updateMenuCoordinates();
	updateSandboxRGB();
	
	//When there is a state change...
	if(lastState!=state) {
		lastState = state;
		switch(state) {
			default:	items.putItemInterface(-1); break;		//Default interface
			case 2: 	items.putItemInterface(Items::SANDBOX); //Sandbox interface
		}
	}
	
	//If backpack or engineering is open (state 1 or 2)
	if(state==1 || state==2) {
		int shx = getSlotHoveringX();
		int shy = getSlotHoveringY();

		//Upon RIGHT click...
		if( controls->isPressed("HARDCODE_RIGHT_CLICK") ) {
			controls->stopPress("HARDCODE_RIGHT_CLICK", __PRETTY_FUNCTION__);	//Stop click
			if( shx!=-1 || shy!=-1 ) {											//If we clicked outside of the inventory, do nothing.
				//If the clicked slot was already selected, reset selection
				if( invSX==shx && invSY==shy && state==1 ) {
					selectInventorySlot(-1, -1);
				} else {
					//Set selected inventory slot to the clicked slot
					selectInventorySlot(shx, shy);
				}
				itemUIShouldUpdate = true;
			}
		}
	
		//Upon LEFT click...
		if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
			controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
			
			//If we clicked inside the inventory...
			if( shx!=-1 || shy!=-1 ) {
				if(invHeldID==-1) {		//If no item is being held: pick up the item in this slot and clear the slot
					invHeldID = inventorySlots[shx][shy];
					inventorySlots[shx][shy] = -1;
				} else {				//If an item is being held: put the item in the slot and set the held item to be whatever was in the slot.
					int temp = inventorySlots[shx][shy];
					inventorySlots[shx][shy] = invHeldID;
					invHeldID = temp;
				}

				itemHeldShouldUpdate = true;
			}
		}

		//If item selection has changed
		if( itemUIShouldUpdate && state==1 ) {
			items.putItemInterface( inventorySlots[invSX][invSY] );
			itemUIShouldUpdate = false;
		}
	}

	//If in engineering, set sandboxRGB to match the selection GUI.
	if( state==2 ) {
		updateSandboxRGB();
	}

}

void PlayerMenu::draw()
{
	double angle = ((SDL_GetTicks()/30)%360)*3.141/180.0;
	oscillation = 32*std::abs(std::sin(angle));
	
	if(itemHeldShouldUpdate) {
		heldItem.init(sdlHandler, 32, 32, 2);
		heldItem.clear();
		heldItem.lock();
		heldItem.blit(TextureLoader::PLAYER_items, getItemTexSrcX(invHeldID), getItemTexSrcY(invHeldID) );
	}

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

		//Draw held item
		if(invHeldID!=-1) {
			heldItem.setDrawPos(controls->getMouseX()/2*2-32, controls->getMouseY()/2*2-32);
			heldItem.draw();
		}
	}
	
}

int PlayerMenu::getSlotHoveringX()
{
	if( controls->getMouseX()<invScreenX || controls->getMouseX()>=invScreenX+invScreenW ) {
		return -1;
	}
	return (controls->getMouseX()-invScreenX)/64;
}
int PlayerMenu::getSlotHoveringY()
{
	if( controls->getMouseY()<invScreenY || controls->getMouseY()>=invScreenY+invScreenH ) {
		return -1;
	}
	return (controls->getMouseY()-invScreenY)/64;
}
int PlayerMenu::getItemTexSrcX(int itemID) { return (itemID%8)*32; }
int PlayerMenu::getItemTexSrcY(int itemID) { return (itemID/8)*32;}

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
				uiOverlay.lock(x*32, y*32, 32, 32);
				
				int itemID = inventorySlots[x][y];
				uiOverlay.blit(TextureLoader::PLAYER_items, getItemTexSrcX(itemID), getItemTexSrcY(itemID));
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
}

void PlayerMenu::updateMenuCoordinates()
{
	//Reset window's XYWH and inventory screen's XYWH
	windowX = 0; windowY = 0; 		windowW = 0; windowH = 0;
	invScreenX = 0; invScreenY = 0; invScreenW = 0; invScreenH = 0;	
	
	//Set XYWH of window and invScreen from parentWindow and invW/invH
	Window* parentWindow = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( parentWindow!=nullptr ) {
		//Window
		windowX = parentWindow->getSX(); 	windowY = parentWindow->getSY();
		windowW = parentWindow->getWidth(); windowH = parentWindow->getHeight();
		//invScreen
		invScreenX = windowX+32*20+2*6;		invScreenW = invW*32*2;
		invScreenY = windowY+2*4;			invScreenH = invH*32*2;
	}
}

void PlayerMenu::updateSandboxRGB()
{
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
/*
	Select the slot given by (x, y) within the current inventory. This should be called when the user right clicks.
	Different inventories have different sizes - and a maximum size of 8x8 or (0,0) to (7,7).
*/
void PlayerMenu::selectInventorySlot(int x, int y)
{
	//If selection is out of bounds
	if( !(x>=0 && x<=7 && y>=0 && y<=7) ) {
		invSX = -1;
		invSY = -1;
		return;
	}
	
	//If previous selected item slot not empty, we are selecting a new item
	if( inventorySlots[invSX][invSY]!=-1 ) {
		itemUIShouldUpdate = true;
	}
	
	//If in sandbox mode, set sandboxTex(X, Y).
	if( state==2 ) {
		sandboxTexX = x;
		sandboxTexY = y;
	}
	
	//Set invSX and invSY to x, y
	invSX = x;
	invSY = y;
}