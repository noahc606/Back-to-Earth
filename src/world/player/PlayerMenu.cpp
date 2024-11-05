#include "PlayerMenu.h"
#include <nch/cpp-utils/log.h>
#include <cmath>
#include "PlayerMenuModules.h"
#include "TextBox.h"
#include "Tooltip.h"

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl, TileDict* td)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;
	tileDict = td;
	sandboxTile = tileDict->getBaseTileDef("hab_futuristic_hull");

	items.init(guiHandler, pl);
	playerGamemode = pl->getGameMode();

	for( int x = -10; x<10; x++ ) {
		for( int y = -10; y<10; y++ ) {
			if(inv.slotExists(x, y)) {
				inv.setSlotItem(x, y, -1);
			}
		}
	}

	hotbar.init(getInventory());

	uiOverlay.init(sdlHandler, 0, 0);
}

void PlayerMenu::tick() {
	//Update menu coords and dimensions
	inv.gfxUpdateCoordinates(guiHandler);
	if( inv.getMod()==PlayerMenuModules::SANDBOX ) {
		updateSandboxTile();
	}
	
	//If the inventory is open at all...
	if(inv.getMod()>=0) {
		tickInventoryOpen();
	}

	hotbar.tick(controls);
}

void PlayerMenu::draw(MissionHolder& mh)
{
	//Update held item texture
	if(itemHeldShouldUpdate) {
		heldItem.init(sdlHandler, 32, 32, 2);
		heldItem.clear();
		heldItem.lock();
		heldItem.blit(
			TextureLoader::PLAYER_items,
			getItemTexSrcX(inv.getHeldItemStack().getType()),
			getItemTexSrcY(inv.getHeldItemStack().getType())
		);
	}

	//If inventory is not closed...
	if( inv.getMod()>-1 ) {
		uiOverlay.clear();

		//Rectangle representing area of interactable inventory
		SDL_Rect invRect;
		invRect.x = inv.getScrX()-2+inv.getUIODX();	invRect.w = inv.getWidth()*32;
		invRect.y = inv.getScrY()-2+inv.getUIODY();	invRect.h = inv.getHeight()*32;

		SDL_Rect invRectInteractable = invRect;
		invRectInteractable.x += 4;
		invRectInteractable.y += 4;
		invRectInteractable.w -= 4;
		invRectInteractable.h -= 2;

		switch(inv.getMod()) {
			case PlayerMenuModules::PLASMA_MATTER_TRANSFORMER:
			case PlayerMenuModules::BACKPACK:
			case PlayerMenuModules::CRAFTING:
			{
				drawInventory();

				if(inv.getMod()==PlayerMenuModules::CRAFTING) {

				}
			} break;
			case PlayerMenuModules::SANDBOX: {
				inv.getPMM()->drawModuleSandbox(uiOverlay, invRect, inv.getSelX(), inv.getSelY());
			} break;
			case PlayerMenuModules::MISSION_LOG: {
				inv.getPMM()->drawModuleMissionLog(sdlHandler, invRectInteractable, mh);
			} break;
		}

		inv.getPMM()->drawWidgets(sdlHandler, playerGamemode, inv.getMod(), inv.getScrX(), inv.getScrY(), inv.getUIODX(), inv.getUIODY());
		uiOverlay.draw();
	}

	//Draw held item
	if(inv.getHeldItemStack().getType()!=-1) {
		int drawX = controls->getMouseX()/2*2-32;
		int drawY = controls->getMouseY()/2*2-32;
		
		heldItem.setDrawPos(drawX, drawY);
		heldItem.draw();
		inv.getHeldItemStack().drawCount(sdlHandler, drawX+2+inv.getUIODX(), drawY+2+inv.getUIODY());
	}
	
	drawHoverText();
}

void PlayerMenu::drawHotbar()
{
	hotbar.draw(sdlHandler, tileDict);
}

int PlayerMenu::getItemTexSrcX(int itemID) { return (itemID%8)*32; }
int PlayerMenu::getItemTexSrcY(int itemID) { return (itemID/8)*32;}

std::string PlayerMenu::getSandboxTileID() { return sandboxTile.id; }
Inventory* PlayerMenu::getInventory() { return &inv; }

void PlayerMenu::decrementSelectedItemStack()
{
	inv.decrementSelectedItemStack();
}

void PlayerMenu::giveItemStack(InvItemStack iis)
{
	if(iis.getType()==-1) {
		return;
	}

	//Find location item should be stored
	int locToUse = -1;
	if(iis.getType()==Items::WORLDTILE) {
		locToUse = 1;
	} else {
		locToUse = 0;
	}

	//Try to find a slot w/ the same item type
	for(int y = 0; y<8; y++)
	for(int x = 0; x<8; x++) {
		InvItemStack tempIIS = inv.getSlotItemStack(locToUse, x, y);
		if( tempIIS.getType()==iis.getType() &&
			tempIIS.getExtraData()==iis.getExtraData() &&
			tempIIS.getCount()<1024
		) {
			if(tempIIS.getCount()>=0) {
				int extraCount = tempIIS.getCount()+iis.getCount()-1024;
				if(extraCount>0) {
					inv.setSlotItemStack(locToUse, x, y, InvItemStack(iis.getType(), tempIIS.getCount()+iis.getCount()-extraCount, iis.getExtraData()));
					giveItemStack(InvItemStack(iis.getType(), extraCount, iis.getExtraData()));
				} else {
					inv.setSlotItemStack(locToUse, x, y, InvItemStack(iis.getType(), tempIIS.getCount()+iis.getCount(), iis.getExtraData()));
				}
			}
			return;
		}
	}

	//Try to find an empty slot
	for(int y = 0; y<8; y++)
	for(int x = 0; x<8; x++) {
		InvItemStack tempIIS = inv.getSlotItemStack(locToUse, x, y);
		if(tempIIS.getType()==-1) {
			inv.setSlotItemStack(locToUse, x, y, InvItemStack(iis.getType(), iis.getCount(), iis.getExtraData()));
			return;
		}
	}
}

void PlayerMenu::setModule(int newMod)
{
	inv.setMod(newMod);

	if(inv.getMod()<=-1) {
		inv.setMod(-1);
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
		uiOverlay.clear();
	}
	
	if(inv.getMod()>=0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open, playerGamemode);
		inv.getPMM()->putMenuInterface(guiHandler, sandboxTile.mapColor, inv.getMod());
		inv.gfxUpdateCoordinates(guiHandler);
	}

	itemUIShouldUpdate = true;
}

void PlayerMenu::tickInventoryOpen()
{
	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();

	//Upon RIGHT click...
	if( controls->isPressed("HARDCODE_RIGHT_CLICK") ) {
		controls->stopPress("HARDCODE_RIGHT_CLICK", __PRETTY_FUNCTION__);	//Stop click

		//If we clicked outside of the inventory, do nothing.
		if(inv.slotExists(shx, shy)) {
			//If the clicked slot was already selected, reset selection
			if( inv.getSelX()==shx && inv.getSelY()==shy && inv.getSelLoc()==inv.getSlotLoc(shx, shy)) {
				inv.selectSlot(-1, -1);
			} else {
				//Set selected inventory slot to the clicked slot
				inv.selectSlot(shx, shy);
			}

			itemUIShouldUpdate = true;
		}
	}

	//Upon LEFT click...
	if( controls->isPressed("HARDCODE_LEFT_CLICK") ) {
		controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);

		bool canSwitchModule = true;

		//If we clicked inside the inventory...
		if(inv.slotExists(shx, shy)) {
			int locToUse = inv.getSlotLoc(shx, shy);

			if(inv.getHeldItemStack().getType()==-1) {
				//If no item is being held: pick up the item in this slot and clear the slot
				inv.setHeldItemStack(inv.getSlotItemStack(locToUse, shx, shy));
				inv.setSlotItemStack(locToUse, shx, shy, InvItemStack(-1, 0));
			} else {
				//If an item is being held: put the item in the slot and set the held item to be whatever was in the slot.
				InvItemStack temp = inv.getSlotItemStack(locToUse, shx, shy);
				inv.setSlotItemStack(locToUse, shx, shy, inv.getHeldItemStack());
				inv.setHeldItemStack(temp);
			}

			itemHeldShouldUpdate = true;
		} else {
			if(inv.getHeldItemStack().getType()!=-1) {
				canSwitchModule = false;
			}
		}

		//If we clicked a potential module widget...
		if(shx==8 && canSwitchModule) {
			int res = inv.getPMM()->widgetClicked(playerGamemode, shx, shy);
			if(res!=-1) {
				setModule(res);
			}
		}
	}

	//If item selection has changed
	if( itemUIShouldUpdate && inv.getMod()!=PlayerMenuModules::SANDBOX ) {
		items.putItemInterface( inv.getSlotItemType(inv.getSelX(), inv.getSelY()) );
		itemUIShouldUpdate = false;
	}
}

void PlayerMenu::drawInventory()
{
	//Reset uiOverlay...
	uiOverlay.clear();
	uiOverlay.setTexDimensions(inv.getWidth()*32, inv.getHeight()*32);
	uiOverlay.setDrawPos(inv.getScrX()-2+inv.getUIODX(), inv.getScrY()-2+inv.getUIODY());
	uiOverlay.setDrawScale(2);
	
	//Draw inventory elements...
	double angle = ((SDL_GetTicks()/30)%360)*3.141/180.0;
	int oscillation = 64*std::abs(std::sin(angle));
	drawInventoryElements(oscillation);

	//Selected Slot Outline
	if( inv.slotExists(inv.getSelX(), inv.getSelY()) ) {
		if( inv.getSlotItemType(inv.getSelX(), inv.getSelY())!=-1 ) {
			SDL_Rect ssrc; ssrc.x = 0; ssrc.y = 0; ssrc.w = 32; ssrc.h = 32;
			SDL_Rect sdst;
			sdst.x = inv.getScrX()+inv.getSelX()*64+inv.getUIODX();
			sdst.y = inv.getScrY()+inv.getSelY()*64+inv.getUIODY();
			sdst.w = 64; sdst.h = 64;
			sdlHandler->renderCopy(TextureLoader::GUI_player_interactions, &ssrc, &sdst);
		}
	}
}

void PlayerMenu::drawInventoryElements(int oscillation)
{
	//Draw inventory elements
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!inv.slotExists(ix, iy)) { continue; }

		//Rect containing 4 ridges and a 30x30 texture
		nch::Color box1(0, 16, 32+oscillation, 100);
		nch::Color box2(0, 0, 48+oscillation, 100);
		nch::Color box3(0, 32, 16, 100);
		if(inv.getSlotItemType(ix, iy)!=-1) {
			//Set box1, box2, box3 to be a different color if this inventory spot is selected
			if( inv.getSelX()==ix && inv.getSelY()==iy ) {
				box1.r += 120; box1.b += 120;
				box2.r += 120; box2.b += 120;
				box3.r += 120; box3.b += 120;
			}

			//Draw box1 or box2 in a checkerboard fashion
			if((ix+iy)%2==1) {
				sdlHandler->setRenderDrawColor(box1);
				sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), inv.getScrY()+iy*64+inv.getUIODY(), 64, 64);
			} else {
				sdlHandler->setRenderDrawColor(box2);
				sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), inv.getScrY()+iy*64+inv.getUIODY(), 64, 64);
			}

			//Draw the item...
			inv.getSlotItemStack(ix, iy).draw(
				sdlHandler, tileDict, inv.getScrX()+ix*64+inv.getUIODX(), inv.getScrY()+iy*64+inv.getUIODY()
			);
		} else {
			sdlHandler->setRenderDrawColor(box3);
			sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), inv.getScrY()+iy*64+inv.getUIODY(), 64, 64);
		}
	}

	//Draw inventory ridges...
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!inv.slotExists(ix, iy)) { continue; }

		//Set color of inventory slot ridges
		nch::Color ridge1(oscillation, oscillation, oscillation, 150);
		nch::Color ridge2(255, 255, 255, 30);

		//Draw ridges
		sdlHandler->setRenderDrawColor(ridge1);
		sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), 		inv.getScrY()+iy*64+62+inv.getUIODY(), 	64, 2);
		sdlHandler->renderFillRect(inv.getScrX()+ix*64+62+inv.getUIODX(), 	inv.getScrY()+iy*64+inv.getUIODY(),		2, 64);
		sdlHandler->setRenderDrawColor(ridge2);
		sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), 		inv.getScrY()+iy*64+inv.getUIODY(), 	2, 64);
		sdlHandler->renderFillRect(inv.getScrX()+ix*64+inv.getUIODX(), 		inv.getScrY()+iy*64+inv.getUIODY(), 	64, 2);
	}
}

void PlayerMenu::drawHoverText()
{
	//Get hovering information
	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();
	int type = inv.getSlotItemType(shx, shy);
	int mx = controls->getMouseX();
	int my = controls->getMouseY();

	//Draw item name if applicable...
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
	std::string special = inv.getPMM()->getWidgetHoverText(playerGamemode, shx, shy);
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

int PlayerMenu::getSlotHoveringX() {
	if(inv.getMod()==-1) return -1000;
	return std::floor((controls->getMouseX()-inv.getScrX())/64.0);
}
int PlayerMenu::getSlotHoveringY() {
	if(inv.getMod()==-1) return -1000;
	return std::floor((controls->getMouseY()-inv.getScrY())/64.0);
}

void PlayerMenu::updateSandboxTile()
{
	/* Set current color */
	nch::Color currentColor(255, 255, 255);
	for( int i = 0; i<3; i++ ){
		auto tbx = guiHandler->getGUI( BTEObject::GUI_textbox, GUIHandler::tbx_CHARACTER_item, 2000+i );
		if(tbx!=nullptr) {
			try {
				std::string colorString = ((TextBox*)tbx)->getString();
				int colorInt = std::stoi(colorString);
				if(colorInt>255) {
					colorInt = 255;
				}
				
				switch(i) {
					case 0: currentColor.r = colorInt; break;
					case 1: currentColor.g = colorInt; break;
					case 2: currentColor.b = colorInt; break;
				}
				
			} catch(...) { }
		}
	}

	/* Set current resrc(x, y) */
	int resrcX = inv.getSelX();
	int resrcY = inv.getSelY();
	
	bool visionBlocking = true;
	if(resrcY==6) visionBlocking = false;

	bool solid = true;
	if(resrcX==0 && resrcY==4) solid = false;

	Tile newTile("null", solid, std::make_pair(resrcX, resrcY), currentColor, visionBlocking);

	//Below block runs whenever the sandbox tile changes
	if(sandboxTile!=newTile) {
		//Find any tiles whose definition matches the 'newTile'.
		std::vector<std::string> matches = tileDict->getIDsMatchingDef(newTile);
		if(matches.size()==0) {	/* If no previous instances of this tile */
			//Generate newTile.id
			std::stringstream ss;
			ss << "_sbx" << tileDict->getNumSandboxTiles();
			newTile.id = ss.str();

			//Add newTile if it doesn't already exist
			tileDict->addTileDefUnique(ss.str(), newTile);
			sandboxTile = newTile;
		} else {				/* If at least one matching instance found */
			sandboxTile = tileDict->at(matches[0]);
		}
	}
}
