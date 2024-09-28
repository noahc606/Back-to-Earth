#include "PlayerMenu.h"
#include <nch/cpp-utils/log.h>
#include <cmath>
#include "PlayerMenuModules.h"
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
	if(lastMod!=mod) {
		lastMod = mod;
		pmm.putMenuInterface(guiHandler, items.getSandboxRGB(), mod);
	}
	
	//If the inventory is open at all...
	if(mod>=0) {
		tickInventoryOpen();
	}

	//If in sandbox, set sandboxRGB to match the selection GUI.
	if( mod==pmm.SANDBOX ) {
		updateSandboxRGB();
	}


	
}

void PlayerMenu::draw()
{
	
	//Update held item texture
	if(itemHeldShouldUpdate) {
		heldItem.init(sdlHandler, 32, 32, 2);
		heldItem.clear();
		heldItem.lock();
		heldItem.blit(TextureLoader::PLAYER_items, getItemTexSrcX(invHeldStack.getType()), getItemTexSrcY(invHeldStack.getType()) );
	}

	//If inventory is not closed...
	if( mod>-1 ) {
		uiOverlay.clear();
		
		if(mod==pmm.PLASMA_MATTER_TRANSFORMER) {
			drawInventory();
		}
		
		if(mod==pmm.SANDBOX) {
			uiOverlay.clear();
			uiOverlay.setTexDimensions(invW*32, invH*32);
			uiOverlay.setDrawPos(invScreenX-2+uiOverlayDX, invScreenY-2+uiOverlayDY);
			uiOverlay.setDrawScale(2);
			
			uiOverlay.lock();
			uiOverlay.blit(TextureLoader::WORLD_TILE_type_a);
			
			//Selected Slot Outline
			if( invSX!=-1 && invSY!=-1 ) {
				uiOverlay.lock(invSX*32, invSY*32, 32, 32);
				uiOverlay.blit(TextureLoader::GUI_player_interactions, 0, 0 );
			}
		}

		pmm.drawWidgets(sdlHandler, playerGamemode, mod, invScreenX, invScreenY, uiOverlayDX, uiOverlayDY);

		uiOverlay.draw();
	}

	//Draw held item
	if(invHeldStack.getType()!=-1) {
		int drawX = controls->getMouseX()/2*2-32;
		int drawY = controls->getMouseY()/2*2-32;
		
		heldItem.setDrawPos(drawX, drawY);
		heldItem.draw();
		invHeldStack.drawCount(sdlHandler, drawX+2+uiOverlayDX, drawY+2+uiOverlayDY);
	}

	drawHoverText();
}

int PlayerMenu::getSlotHoveringX() {
	if(mod==-1) return -1000;
	return std::floor((controls->getMouseX()-invScreenX)/64.0);
}
int PlayerMenu::getSlotHoveringY() {
	if(mod==-1) return -1000;
	return std::floor((controls->getMouseY()-invScreenY)/64.0);
}
int PlayerMenu::getItemTexSrcX(int itemID) { return (itemID%8)*32; }
int PlayerMenu::getItemTexSrcY(int itemID) { return (itemID/8)*32;}

int PlayerMenu::getModule() { return mod; }
uint8_t PlayerMenu::getSelectedSlotX() { return invSX; }
uint8_t PlayerMenu::getSelectedSlotY() { return invSY; }
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

void PlayerMenu::setModule(int newMod)
{
	mod = newMod;
	
	if(mod<=-1) {
		mod = -1;
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
		uiOverlay.clear();
	}
	
	if(mod>=0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open, playerGamemode);
		updateMenuCoordinates();
	}
}

void PlayerMenu::save(FileHandler* fh, std::string worldDataPath)
{
	nch::Log::log("Saving main player data within \"%s\"\n", worldDataPath.c_str());

	std::stringstream invdata;
	for(int y = 0; y<invH; y++)
	for(int x = 0; x<invW; x++) {

	}
}

void PlayerMenu::tickInventoryOpen()
{
	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();

	//Upon RIGHT click...
	if( controls->isPressed("HARDCODE_RIGHT_CLICK") ) {
		controls->stopPress("HARDCODE_RIGHT_CLICK", __PRETTY_FUNCTION__);	//Stop click

		if(inventorySlotExists(shx, shy)) {									//If we clicked outside of the inventory, do nothing.
			//If the clicked slot was already selected, reset selection
			if( invSX==shx && invSY==shy && mod==pmm.PLASMA_MATTER_TRANSFORMER ) {
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

		//If we clicked a potential module widget...
		if(shx==8) {
			int res = pmm.widgetClicked(playerGamemode, shx, shy);
			if(res!=-1) {
				setModule(res);
			}
		}
	}

	//If item selection has changed
	if( itemUIShouldUpdate && mod==pmm.PLASMA_MATTER_TRANSFORMER ) {
		items.putItemInterface( getInventorySlotItemType(invSX, invSY) );
		itemUIShouldUpdate = false;
	}
}

void PlayerMenu::drawInventory()
{
	//Reset uiOverlay...
	uiOverlay.clear();
	uiOverlay.setTexDimensions(invW*32, invH*32);
	uiOverlay.setDrawPos(invScreenX-2+uiOverlayDX, invScreenY-2+uiOverlayDY);
	uiOverlay.setDrawScale(2);
	
	//Draw inventory elements...
	double angle = ((SDL_GetTicks()/30)%360)*3.141/180.0;
	int oscillation = 64*std::abs(std::sin(angle));
	drawInventoryElements(oscillation);

	//Selected Slot Outline
	if( inventorySlotExists(invSX, invSY) ) {
		if( getInventorySlotItemType(invSX, invSY)!=-1 ) {
			SDL_Rect ssrc; ssrc.x = 0; ssrc.y = 0; ssrc.w = 32; ssrc.h = 32;
			SDL_Rect sdst; sdst.x = invScreenX+invSX*64+uiOverlayDX; sdst.y = invScreenY+invSY*64+uiOverlayDY; sdst.w = 64; sdst.h = 64;
			sdlHandler->renderCopy(TextureLoader::GUI_player_interactions, &ssrc, &sdst);
		}
	}
}

void PlayerMenu::drawInventoryElements(int oscillation)
{
	//Draw inventory elements
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!inventorySlotExists(ix, iy)) { continue; }

		//Rect containing 4 ridges and a 30x30 texture
		nch::Color box1(0, 16, 32+oscillation, 100);
		nch::Color box2(0, 0, 48+oscillation, 100);
		nch::Color box3(0, 32, 16, 100);
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
				sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, invScreenY+iy*64+uiOverlayDY, 64, 64);
			} else {
				sdlHandler->setRenderDrawColor(box2);
				sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, invScreenY+iy*64+uiOverlayDY, 64, 64);
			}

			//Draw the item...
			int itemID = getInventorySlotItemType(ix, iy);
			SDL_Rect isrc; isrc.x = getItemTexSrcX(itemID)+1; isrc.y = getItemTexSrcY(itemID)+1; isrc.w = 30; isrc.h = 30;
			SDL_Rect idst; idst.x = (invScreenX+ix*64+uiOverlayDX); idst.y = (invScreenY+iy*64+uiOverlayDY); idst.w = 60; idst.h = 60;
			if(getInventorySlotItemType(ix, iy)!=Items::WORLDTILE) {
				sdlHandler->renderCopy(TextureLoader::PLAYER_items, &isrc, &idst);
			} else {
				getInventorySlotItemStack(ix, iy).drawEDTileType(sdlHandler, idst.x, idst.y);
			}
			
			getInventorySlotItemStack(ix, iy).drawCount(sdlHandler, invScreenX+ix*64+uiOverlayDX, invScreenY+iy*64+uiOverlayDY);
		} else {
			sdlHandler->setRenderDrawColor(box3);
			sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, invScreenY+iy*64+uiOverlayDY, 64, 64);
		}
	}

	//Draw inventory ridges...
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!inventorySlotExists(ix, iy)) { continue; }

		//Set color of inventory slot ridges
		nch::Color ridge1(oscillation, oscillation, oscillation, 150);
		nch::Color ridge2(255, 255, 255, 30);

		//Draw ridges
		sdlHandler->setRenderDrawColor(ridge1);
		sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, 	invScreenY+iy*64+62+uiOverlayDY, 	64, 2);
		sdlHandler->renderFillRect(invScreenX+ix*64+62+uiOverlayDX, invScreenY+iy*64+uiOverlayDY,		2, 64);
		sdlHandler->setRenderDrawColor(ridge2);
		sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, 	invScreenY+iy*64+uiOverlayDY, 	2, 64);
		sdlHandler->renderFillRect(invScreenX+ix*64+uiOverlayDX, 	invScreenY+iy*64+uiOverlayDY, 	64, 2);
	}
}

void PlayerMenu::drawHoverText()
{
	//Draw normal item hover text
	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();
	int type = getInventorySlotItemType(shx, shy);
	int mx = controls->getMouseX();
	int my = controls->getMouseY();

	std::string itemName = Items::getItemName(type);
	if(itemName!="???null???") {		
		int trw = TextOld::draw(sdlHandler, itemName, mx, my, 2);

		SDL_Rect tr;
		tr.x = (controls->getMouseX()-6)/2*2;
		tr.y = (controls->getMouseY()-6)/2*2;
		tr.w = trw/2*2+12;
		tr.h = 7*2+12;
		
		sdlHandler->setRenderDrawColor(nch::Color(0, 0, 0, 128));
		sdlHandler->renderFillRect(&tr);

		TextOld::draw(sdlHandler, itemName, mx, my, 2);
	}

	//Draw special hovertext if applicable...
	std::string special = "???null???";
	
	if(shy==0 && shx==8) { special = "Plasma-Matter Transformer"; }		//PMT interface button
	if(shy==1 && shx==8) {
		//Sandbox interface button
		if(playerGamemode==Player::GameModes::SANDBOX) {
			special = "Sandbox";
		} else {
			//Mission log interface button
			special = "Mission Log";
		}
	}
	if(shy==2 && shx==8) { special = "Crafting"; }						//Crafting interface button
	if(special!="???null???") {
		int trw = TextOld::draw(sdlHandler, special, mx, my, 2);

		SDL_Rect tr;
		tr.x = (mx-6)/2*2;
		tr.y = (my-6)/2*2;
		tr.w = trw/2*2+12;
		tr.h = 7*2+12;
		
		sdlHandler->setRenderDrawColor(nch::Color(0, 0, 0, 128));
		sdlHandler->renderFillRect(&tr);

		TextOld::draw(sdlHandler, special, mx, my, 2, nch::Color(0, 255, 255), nch::Color(0, 0, 0, 0));
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
	switch(mod) {
		//PMT: all inventory slots (normal behavior)
		case pmm.PLASMA_MATTER_TRANSFORMER: {
			if( inventorySlots.find(std::make_pair(x, y))!=inventorySlots.end() && inventorySlotExists(x, y) ) {
				return inventorySlots.at(std::make_pair(x, y));
			}			
		} break;
		
		//Sandbox: always nothing
		case pmm.SANDBOX: {} break;

		//Crafting: inventorySlots if y==0 || y==1; nothing otherwise.
		case pmm.CRAFTING: {
			if( y==0 || y==1 ) {
				if( inventorySlots.find(std::make_pair(x, y))!=inventorySlots.end() && inventorySlotExists(x, y) ) {
					return inventorySlots.at(std::make_pair(x, y));
				}
			}
		} break;
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
		invScreenX = windowX+64*4+2*6;		invScreenW = invW*64;
		invScreenY = windowY+2*4;			invScreenH = invH*64;
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
				
				nch::Color newcolor = items.getSandboxRGB();
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
		nch::Log::warn(__PRETTY_FUNCTION__, "Invalid inventory slot (%d, %d)", x, y);
		return;
	}

	if(inventorySlots.find(std::make_pair(x, y))!=inventorySlots.end()) {
		inventorySlots.at(std::make_pair(x, y)) = iis;
	} else {
		inventorySlots.insert( std::make_pair(std::make_pair(x, y), iis) );
	}
}