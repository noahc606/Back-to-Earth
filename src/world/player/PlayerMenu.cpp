#include "PlayerMenu.h"
#include <cmath>
#include "Log.h"
#include "TextBox.h"
#include "Tooltip.h"

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;

	items.init(guiHandler, pl);
	playerGamemode = pl->getGameMode();

	for( int x = -10; x<10; x++ ) {
		for( int y = -10; y<10; y++ ) {
			if(inventorySlotExists(x, y)) {
				setInventorySlotItem(x, y, -1);
			}
		}
	}

	setInventorySlotItem(-4, 2, Items::QUANTUM_EXOSUIT_HELMET);
	setInventorySlotItem(-4, 3, Items::QUANTUM_EXOSUIT_BODY);
	setInventorySlotItem(-4, 4, Items::QUANTUM_EXOSUIT_LEGGINGS);

	setInventorySlotItem(3, 0, Items::ION_BLASTER);
	setInventorySlotItem(4, 1, Items::GEOPORTER);
	setInventorySlotItem(6, 2, Items::SOLID_PLASMA_ANNIHILATOR);
	setInventorySlotItem(6, 3, Items::ATOM_PRINTER);

	setInventorySlotItemStack(7, 7, InvItemStack(Items::FOOD_RATION_A, 19));
	setInventorySlotItemStack(7, 5, InvItemStack(Items::FOOD_RATION_B, 20));
	setInventorySlotItemStack(7, 4, InvItemStack(Items::FOOD_RATION_C, 14));
	setInventorySlotItemStack(7, 3, InvItemStack(Items::FOOD_RATION_D, 17));
	setInventorySlotItemStack(6, 7, InvItemStack(Items::FOOD_RATION_E, 29));
	setInventorySlotItemStack(6, 5, InvItemStack(Items::FOOD_RATION_F, 31));
	
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
			if(inventorySlotExists(shx, shy)) {									//If we clicked outside of the inventory, do nothing.
				//If the clicked slot was already selected, reset selection
				if( invSX==shx && invSY==shy && state==1 ) {
					selectInventorySlot(-1, -1);
				} else {
					//Set selected inventory slot to the clicked slot
					selectInventorySlot(shx, shy);
					invSelectedStack = getInventorySlotItemStack(shx, shy);
				}
				itemUIShouldUpdate = true;
			}
		}
	
		//Upon LEFT click...
		if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
			controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);
			
			//If we clicked inside the inventory...
			if(inventorySlotExists(shx, shy)) {
				if(invHeldStack.getType()==-1) {		//If no item is being held: pick up the item in this slot and clear the slot
					invHeldStack = getInventorySlotItemStack(shx, shy);
					setInventorySlotItemStack(shx, shy, InvItemStack(-1, 0));
				} else {				//If an item is being held: put the item in the slot and set the held item to be whatever was in the slot.
					InvItemStack temp = getInventorySlotItemStack(shx, shy);
					setInventorySlotItemStack(shx, shy, invHeldStack);
					invHeldStack = temp;
				}

				itemHeldShouldUpdate = true;
			}
		}

		//If item selection has changed
		if( itemUIShouldUpdate && state==1 ) {
			items.putItemInterface( getInventorySlotItemType(invSX, invSY) );
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
		heldItem.blit(TextureLoader::PLAYER_items, getItemTexSrcX(invHeldStack.getType()), getItemTexSrcY(invHeldStack.getType()) );
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
		if(invHeldStack.getType()!=-1) {
			int drawX = controls->getMouseX()/2*2-32;
			int drawY = controls->getMouseY()/2*2-32;
			
			heldItem.setDrawPos(drawX, drawY);
			heldItem.draw();
			invHeldStack.drawCount(sdlHandler, drawX+2, drawY+2);
		}
	}
	
}

int PlayerMenu::getSlotHoveringX() { return std::floor((controls->getMouseX()-invScreenX)/64.0); }
int PlayerMenu::getSlotHoveringY() { return std::floor((controls->getMouseY()-invScreenY)/64.0); }
int PlayerMenu::getItemTexSrcX(int itemID) { return (itemID%8)*32; }
int PlayerMenu::getItemTexSrcY(int itemID) { return (itemID/8)*32;}

int PlayerMenu::getState() { return state; }
uint8_t PlayerMenu::getSandboxTexX() { return sandboxTexX; }
uint8_t PlayerMenu::getSandboxTexY() { return sandboxTexY; }
uint8_t PlayerMenu::getSandboxTexRed() { return items.getSandboxRGB().r; } 
uint8_t PlayerMenu::getSandboxTexGreen() { return items.getSandboxRGB().g; } 
uint8_t PlayerMenu::getSandboxTexBlue() { return items.getSandboxRGB().b; } 

InvItemStack PlayerMenu::getSelectedItemStack()
{
	return invSelectedStack;
}

void PlayerMenu::decrementSelectedItemStack()
{
	InvItemStack iss = getInventorySlotItemStack(invSX, invSY);
	if(iss.getType()==-1 || iss.getCount()==0) {
		return;
	}

	if(iss.getCount()>1) {
		setInventorySlotItemStack(invSX, invSY, InvItemStack(iss.getType(), iss.getCount()-1, iss.getExtraData()));
	} else if(iss.getCount()==1) {
		setInventorySlotItemStack(invSX, invSY, InvItemStack(-1, 0, 0));
	} else {
		setInventorySlotItemStack(invSX, invSY, InvItemStack(iss.getType(), -1, iss.getExtraData()));
	}

	invSelectedStack = getInventorySlotItemStack(invSX, invSY);
	if(getInventorySlotItemStack(invSX, invSY).getCount()==0) {
		invSelectedStack = InvItemStack(-1, 0, 0);
		invSX = -1;
		invSY = -1;
	}
}

void PlayerMenu::giveItemStack(InvItemStack iis)
{
	//TODO account for IIS counts other than 1
	if(iis.getType()==-1) {
		return;
	}

	//Try to find the same item type
	for(int x = 0; x<8; x++)
	for(int y = 0; y<8; y++) {
		InvItemStack tempIIS = getInventorySlotItemStack(x, y);
		if(
			tempIIS.getType()==iis.getType() &&
			tempIIS.getExtraData()==iis.getExtraData() &&
			tempIIS.getCount()<1024)
		{
			setInventorySlotItemStack(x, y, InvItemStack(iis.getType(), tempIIS.getCount()+1, iis.getExtraData()));
			return;
		}
	}

	//Try to find an empty slot
	for(int x = 0; x<8; x++)
	for(int y = 0; y<8; y++) {
		InvItemStack tempIIS = getInventorySlotItemStack(x, y);
		if(tempIIS.getType()==-1) {
			setInventorySlotItemStack(x, y, InvItemStack(iis.getType(), 1, iis.getExtraData()));
			return;
		}
	}
}

void PlayerMenu::setState(int newState)
{
	state = newState;
	
	if(state==0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
		uiOverlay.clear();
	}
	
	if(state>0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open, playerGamemode);
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
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!inventorySlotExists(ix, iy)) {
			continue;
		}

		//Rect containing 4 ridges and a 30x30 texture
		Color box1(0, 16, 32+oscillation, 100);
		Color box2(0, 0, 48+oscillation, 100);
		Color box3(0, 32, 16, 100);
		if(getInventorySlotItemType(ix, iy)!=-1) {
			//Set box1, box2, box3 to be a different color if this inventory spot is selected
			if( invSX==ix && invSY==iy ) {
				box1.r += 120; box1.b += 120;
				box2.r += 120; box2.b += 120;
				box3.r += 120; box3.b += 120;
			}

			//Draw box1 or box2 in a checkerboard fashion
			if((ix+iy)%2==1) {
				sdlHandler->setRenderDrawColor(box1);
				sdlHandler->renderFillRect(invScreenX+ix*64-2, invScreenY+iy*64-2, 64, 64);
			} else {
				sdlHandler->setRenderDrawColor(box2);
				sdlHandler->renderFillRect(invScreenX+ix*64-2, invScreenY+iy*64-2, 64, 64);
			}
			
			int itemID = getInventorySlotItemType(ix, iy);
			
			SDL_Rect isrc; isrc.x = getItemTexSrcX(itemID)+1; isrc.y = getItemTexSrcY(itemID)+1; isrc.w = 30; isrc.h = 30;
			SDL_Rect idst; idst.x = (invScreenX+ix*64); idst.y = (invScreenY+iy*64); idst.w = 60; idst.h = 60;
			if(getInventorySlotItemType(ix, iy)!=Items::WORLDTILE) {
				sdlHandler->renderCopy(TextureLoader::PLAYER_items, &isrc, &idst);
			} else {
				getInventorySlotItemStack(ix, iy).drawEDTileType(sdlHandler, idst.x, idst.y);
			}
			
			getInventorySlotItemStack(ix, iy).drawCount(sdlHandler, invScreenX+ix*64, invScreenY+iy*64);
		} else {
			sdlHandler->setRenderDrawColor(box3);
			sdlHandler->renderFillRect(invScreenX+ix*64-2, invScreenY+iy*64-2, 64, 64);
		}

		//Draw ridges
		sdlHandler->setRenderDrawColor(ridge1);
		sdlHandler->renderFillRect(invScreenX+ix*64-2, 	invScreenY+iy*64-2, 	2, 64);
		sdlHandler->renderFillRect(invScreenX+ix*64-2, 	invScreenY+iy*64-2, 	64, 2);
		sdlHandler->setRenderDrawColor(ridge2);
		if(iy!=7) sdlHandler->renderFillRect(invScreenX+ix*64-2, 	invScreenY+iy*64+60, 	64, 2);
		if(ix!=7) sdlHandler->renderFillRect(invScreenX+ix*64+60, invScreenY+iy*64-2,		2, 64);
	}
	
	//Selected Slot Outline
	if( inventorySlotExists(invSX, invSY) ) {
		if( getInventorySlotItemType(invSX, invSY)!=-1 ) {
			SDL_Rect ssrc; ssrc.x = 0; ssrc.y = 0; ssrc.w = 32; ssrc.h = 32;
			SDL_Rect sdst; sdst.x = invScreenX+invSX*64-2; sdst.y = invScreenY+invSY*64-2; sdst.w = 64; sdst.h = 64;
			sdlHandler->renderCopy(TextureLoader::GUI_player_interactions, &ssrc, &sdst);
		}
	}
}

bool PlayerMenu::inventorySlotExists(int x, int y)
{
	if(x>=0 && x<=7 && y>=0 && y<=7) 	return true;	//Main slots
	if(x==-4 && y>=2 && y<=4)			return true;	//Clothing/spacesuit slots
	return false;
}

int PlayerMenu::getInventorySlotItemType(int x, int y)
{
	return getInventorySlotItemStack(x, y).getType();
}

InvItemStack PlayerMenu::getInventorySlotItemStack(int x, int y)
{
	if( inventorySlots.find(std::make_pair(x, y))!=inventorySlots.end() && inventorySlotExists(x, y) ) {
		return inventorySlots.at(std::make_pair(x, y));
	}

	return InvItemStack(-1, 0);
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
	if(!inventorySlotExists(x, y)) {
		invSX = -1;
		invSY = -1;
		return;
	}
	
	//If previous selected item slot not empty, we are selecting a new item
	if( getInventorySlotItemType(invSX, invSY)!=-1 ) {
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

void PlayerMenu::setInventorySlotItem(int x, int y, int i)
{
	setInventorySlotItemStack(x, y, InvItemStack(i, 1));
}

void PlayerMenu::setInventorySlotItemStack(int x, int y, InvItemStack iis)
{
	if(!inventorySlotExists(x, y)) {
		Log::warn(__PRETTY_FUNCTION__, "Invalid inventory slot (%d, %d)", x, y);
		return;
	}

	if(inventorySlots.find(std::make_pair(x, y))!=inventorySlots.end()) {
		inventorySlots.at(std::make_pair(x, y)) = iis;
	} else {
		inventorySlots.insert( std::make_pair(std::make_pair(x, y), iis) );
	}
}