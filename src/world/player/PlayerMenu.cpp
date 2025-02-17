#include "PlayerMenu.h"
#include <nch/cpp-utils/log.h>
#include <cmath>
#include "PlayerMenuModules.h"
#include "TextBox.h"
#include "Tooltip.h"

using namespace nch;

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl, TileDict* td)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;
	tileDict = td;
	sandboxTile = tileDict->getBaseTileDef("hab_futuristic_hull");

	items.init(guiHandler, pl);
	playerGamemode = pl->getGameMode();

	for( int x = -10; x<10; x++ )
	for( int y = -10; y<10; y++ )
	if(invhdr.slotExists(x, y)) {
		invhdr.setSlotItem(x, y, -1);
	}

	hotbar.init(getInventoryHolder());

	uiOverlay.init(sdlHandler, 0, 0);
}

void PlayerMenu::tick() {
	//Update menu coords and dimensions
	updateInvCoordinates();
	if( invhdr.getMod()==PlayerMenuModules::SANDBOX ) {
		updateSandboxTile();
	}

	//If inventory module changes from -1 to >=0...
	if(lastInvMod!=invhdr.getMod()) {

		//Select hotbar slot
		if(lastInvMod==-1 && invhdr.getMod()>=0) {
			auto lsis = hotbar.getLastSelectedInvSlot();
			invhdr.selectSlot(std::get<0>(lsis), std::get<1>(lsis), std::get<2>(lsis));
		}

		timeMenuOpenedMS = SDL_GetTicks64();

		lastInvMod = invhdr.getMod();
	}

	//If the inventory is open at all...
	if(invhdr.getMod()>=0) {
		tickInventoryOpen();
		hotbar.setLastSelectedInvSlot(invhdr.getSelLoc(), invhdr.getSelX(), invhdr.getSelY());
	} else {
		hotbar.tick(controls);
	}
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
			getItemTexSrcX(invhdr.getHeldItemStack().getType()),
			getItemTexSrcY(invhdr.getHeldItemStack().getType())
		);
	}

	//If inventory is not closed...
	if( invhdr.getMod()>-1 ) {
		uiOverlay.clear();

		//Rectangle representing area of interactable inventory
		SDL_Rect invRect;
		invRect.x = invGfx.screenX-2+invGfx.uiOverlayDX;	invRect.w = invGfx.w*32;
		invRect.y = invGfx.screenY-2+invGfx.uiOverlayDY;	invRect.h = invGfx.h*32;

		SDL_Rect invRectInteractable = invRect;
		invRectInteractable.x += 4;
		invRectInteractable.y += 4;
		invRectInteractable.w -= 4;
		invRectInteractable.h -= 2;

		switch(invhdr.getMod()) {
			case PlayerMenuModules::PLASMA_MATTER_TRANSFORMER:
			case PlayerMenuModules::BACKPACK:
			case PlayerMenuModules::CRAFTING:
			{
				drawInventory();

				if(invhdr.getMod()==PlayerMenuModules::CRAFTING) {

				}
			} break;
			case PlayerMenuModules::SANDBOX: {
				invhdr.getPMM()->drawModuleSandbox(uiOverlay, invRect, invhdr.getSelX(), invhdr.getSelY());
			} break;
			case PlayerMenuModules::MISSION_LOG: {
				invhdr.getPMM()->drawModuleMissionLog(sdlHandler, invRectInteractable, mh);
			} break;
		}

		invhdr.getPMM()->setDrawPos(invGfx.screenX+invGfx.uiOverlayDX, invGfx.screenY+invGfx.uiOverlayDY);
		invhdr.getPMM()->drawWidgets(sdlHandler, playerGamemode, invhdr.getMod());
		uiOverlay.draw();
	}

	//Draw held item
	if(invhdr.getHeldItemStack().getType()!=-1) {
		int drawX = controls->getMouseX()/2*2-32;
		int drawY = controls->getMouseY()/2*2-32;
		
		heldItem.setDrawPos(drawX, drawY);
		heldItem.draw();
		invhdr.getHeldItemStack().drawCount(sdlHandler, drawX+2+invGfx.uiOverlayDX, drawY+2+invGfx.uiOverlayDY);
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
InventoryHolder* PlayerMenu::getInventoryHolder() { return &invhdr; }
bool PlayerMenu::isHotbarPMTActive() { return hotbar.isPMTActive(); }

void PlayerMenu::decrementSelectedItemStack()
{
	invhdr.decrementSelectedItemStack();
}

void PlayerMenu::giveItemStack(ItemStack is)
{
	if(is.getType()==-1) return;

	//Find location item should be stored
	int locToUse = -1;
	if(is.getType()==Items::WORLDTILE) {
		locToUse = 1;
	} else {
		locToUse = 0;
	}

	//Try to find a slot w/ the same item type
	for(int y = 0; y<8; y++)
	for(int x = 0; x<8; x++) {
		ItemStack tempIS = invhdr.getSlotItemStack(locToUse, x, y);
		if( tempIS.getType()==is.getType() &&
			tempIS.getExtraData()==is.getExtraData() &&
			tempIS.getCount()<1024
		) {
			if(tempIS.getCount()>=0) {
				int extraCount = tempIS.getCount()+is.getCount()-1024;
				if(extraCount>0) {
					invhdr.setSlotItemStack(locToUse, x, y, ItemStack(is.getType(), tempIS.getCount()+is.getCount()-extraCount, is.getExtraData()));
					giveItemStack(ItemStack(is.getType(), extraCount, is.getExtraData()));
				} else {
					invhdr.setSlotItemStack(locToUse, x, y, ItemStack(is.getType(), tempIS.getCount()+is.getCount(), is.getExtraData()));
				}
			}
			return;
		}
	}

	//Try to find an empty slot
	for(int y = 0; y<8; y++)
	for(int x = 0; x<8; x++) {
		ItemStack tempIS = invhdr.getSlotItemStack(locToUse, x, y);
		if(tempIS.getType()==-1) {
			invhdr.setSlotItemStack(locToUse, x, y, ItemStack(is.getType(), is.getCount(), is.getExtraData()));
			return;
		}
	}
}

void PlayerMenu::setModule(int newMod)
{
	invhdr.setMod(newMod);

	if(invhdr.getMod()<=-1) {
		invhdr.setMod(-1);
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_close);
		uiOverlay.clear();
	}
	
	if(invhdr.getMod()>=0) {
		guiHandler->setGUIs(GUIHandler::GUIs::WORLD_characterMenu_open, playerGamemode);
		invhdr.getPMM()->putMenuInterface(guiHandler, sandboxTile.mapColor, invhdr.getMod());
		updateInvCoordinates();
	}

	itemUIShouldUpdate = true;
}

void PlayerMenu::setInteractingTileEntity(TileMap* tm, Vec3<int64_t> pos)
{
	interactingPos = pos;
	
	if(tm!=nullptr) {
		TileEntity* te = tm->getTileEntity(pos);
		if(te!=nullptr) {
			invhdr.getPMM()->setInteractionType(te->getType());
			invhdr.setInteractingInv(te->getInventory());
			return;
		}
	}

	invhdr.getPMM()->setInteractionType(TileEntity::NONE);
	invhdr.setInteractingInv(nullptr);
}

void PlayerMenu::resetInteractingTileEntity() { setInteractingTileEntity(nullptr, Vec3<int64_t>(0, 0, 0)); }

void PlayerMenu::tickInventoryOpen()
{
	bool allowClicking = false;
	if(SDL_GetTicks64()-timeMenuOpenedMS>250) {
		allowClicking = true;
	}

	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();

	//Upon RIGHT click...
	if(allowClicking && controls->isPressed("HARDCODE_RIGHT_CLICK")) {
		controls->stopPress("HARDCODE_RIGHT_CLICK", __PRETTY_FUNCTION__);	//Stop click

		//If we clicked outside of the inventory, do nothing.
		if(invhdr.slotExists(shx, shy)) {
			//If the clicked slot was already selected, reset selection
			if( invhdr.getSelX()==shx && invhdr.getSelY()==shy && invhdr.getSelLoc()==invhdr.getSlotLoc(shx, shy)) {
				invhdr.selectSlot(-1, -1);
			} else {
				//Set selected inventory slot to the clicked slot
				invhdr.selectSlot(shx, shy);
			}

			itemUIShouldUpdate = true;
		}
	}

	//Upon MIDDLE click...
	if(allowClicking && controls->isPressed("HARDCODE_MIDDLE_CLICK")) {
		controls->stopPress("HARDCODE_MIDDLE_CLICK", __PRETTY_FUNCTION__);

		//If clicking inside the inventory...
		if(invhdr.slotExists(shx, shy)) {
			int locToUse = invhdr.getSlotLoc(shx, shy);
			ItemStack his = invhdr.getHeldItemStack();
			ItemStack iis = invhdr.getSlotItemStack(locToUse, shx, shy);

			//Average out count between slot and held item (only if both stacks can stack OR if heldItem || slotItem is empty)
			if(his.getType()==-1 || iis.getType()==-1) {

			}

			if(his.stacksWith(iis)) {
				
			}	
		}


	}

	//Upon LEFT click...
	if(allowClicking && controls->isPressed("HARDCODE_LEFT_CLICK")) {
		controls->stopPress("HARDCODE_LEFT_CLICK", __PRETTY_FUNCTION__);

		bool canSwitchModule = true;

		//If we clicked inside the inventory...
		if(invhdr.slotExists(shx, shy)) {
			int locToUse = invhdr.getSlotLoc(shx, shy);

			ItemStack heldItemStack = invhdr.getHeldItemStack();
			if(heldItemStack.getType()==-1) {
				//If no item is being held: pick up the item in this slot and clear the slot
				invhdr.setHeldItemStack(invhdr.getSlotItemStack(locToUse, shx, shy));
				invhdr.setSlotItemStack(locToUse, shx, shy, ItemStack(-1, 0));
			} else {
				//If an item is being held: 2 possibilities:

				//1. If items exactly the same, increment count of slot item based on how many held.
				if(heldItemStack.stacksWith(invhdr.getSlotItemStack(locToUse, shx, shy))) {
					ItemStack temp = invhdr.getSlotItemStack(locToUse, shx, shy);
					int finalCount = temp.getCount()+heldItemStack.getCount();
					if(finalCount<=1024) {
						invhdr.setSlotItemStack(locToUse, shx, shy, ItemStack(temp.getType(), finalCount, temp.getExtraData()));
						invhdr.setHeldItemStack(ItemStack(-1, 0));
					} else {
						invhdr.setSlotItemStack(locToUse, shx, shy, ItemStack(temp.getType(), 1024, temp.getExtraData()));
						invhdr.setHeldItemStack(ItemStack(heldItemStack.getType(), finalCount-1024, heldItemStack.getExtraData()));
					}
				
				//2. Switch the item in the slot and the held item
				} else {
					ItemStack temp = invhdr.getSlotItemStack(locToUse, shx, shy);
					invhdr.setSlotItemStack(locToUse, shx, shy, heldItemStack);
					invhdr.setHeldItemStack(temp);
				}
			}

			itemHeldShouldUpdate = true;
		} else {
			if(invhdr.getHeldItemStack().getType()!=-1) {
				canSwitchModule = false;
			}
		}

		//If we clicked a potential module widget...
		if(shx==8 && canSwitchModule) {
			int res = invhdr.getPMM()->widgetClicked(playerGamemode, shx, shy);
			if(res!=-1) {
				setModule(res);
			}
		}
	}

	//If item selection has changed
	if( itemUIShouldUpdate && invhdr.getMod()!=PlayerMenuModules::SANDBOX ) {
		items.putItemInterface( invhdr.getSlotItemType(invhdr.getSelX(), invhdr.getSelY()) );
		itemUIShouldUpdate = false;
	}
}

void PlayerMenu::drawInventory()
{
	//Reset uiOverlay...
	uiOverlay.clear();
	uiOverlay.setTexDimensions(invGfx.w*32, invGfx.h*32);
	uiOverlay.setDrawPos(invGfx.screenX-2+invGfx.uiOverlayDX, invGfx.screenY-2+invGfx.uiOverlayDY);
	uiOverlay.setDrawScale(2);
	
	//Draw inventory elements...
	double angle = ((SDL_GetTicks()/30)%360)*3.141/180.0;
	int oscillation = 64*std::abs(std::sin(angle));
	drawInventoryElements(oscillation);

	//Selected Slot Outline
	if( invhdr.slotExists(invhdr.getSelX(), invhdr.getSelY()) ) {
		if( invhdr.getSlotItemType(invhdr.getSelX(), invhdr.getSelY())!=-1 ) {
			SDL_Rect ssrc; ssrc.x = 0; ssrc.y = 0; ssrc.w = 32; ssrc.h = 32;
			SDL_Rect sdst;
			sdst.x = invGfx.screenX+invhdr.getSelX()*64+invGfx.uiOverlayDX;
			sdst.y = invGfx.screenY+invhdr.getSelY()*64+invGfx.uiOverlayDY;
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
		if(!invhdr.slotExists(ix, iy)) { continue; }

		//Rect containing 4 ridges and a 30x30 texture
		Color box1(0, 16, 32+oscillation, 100);
		Color box2(0, 0, 48+oscillation, 100);
		Color box3(0, 32, 16, 100);
		if(invhdr.getSlotItemType(ix, iy)!=-1) {
			//Set box1, box2, box3 to be a different color if this inventory spot is selected
			if( invhdr.getSelX()==ix && invhdr.getSelY()==iy ) {
				box1.r += 120; box1.b += 120;
				box2.r += 120; box2.b += 120;
				box3.r += 120; box3.b += 120;
			}

			//Draw box1 or box2 in a checkerboard fashion
			if((ix+iy)%2==1) {
				sdlHandler->setRenderDrawColor(box1);
				sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDX, invGfx.screenY+iy*64+invGfx.uiOverlayDY, 64, 64);
			} else {
				sdlHandler->setRenderDrawColor(box2);
				sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDX, invGfx.screenY+iy*64+invGfx.uiOverlayDY, 64, 64);
			}

			//Draw the item...
			invhdr.getSlotItemStack(ix, iy).draw(
				sdlHandler, tileDict, invGfx.screenX+ix*64+invGfx.uiOverlayDX, invGfx.screenY+iy*64+invGfx.uiOverlayDY
			);
		} else {
			sdlHandler->setRenderDrawColor(box3);
			sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDX, invGfx.screenY+iy*64+invGfx.uiOverlayDY, 64, 64);
		}
	}

	//Draw inventory ridges...
	for(int ix = -10; ix<10; ix++)
	for(int iy = -10; iy<10; iy++) {
		if(!invhdr.slotExists(ix, iy)) { continue; }

		//Set color of inventory slot ridges
		Color ridge1(oscillation, oscillation, oscillation, 150);
		Color ridge2(255, 255, 255, 30);

		//Draw ridges
		sdlHandler->setRenderDrawColor(ridge1);
		sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDX,		invGfx.screenY+iy*64+62+invGfx.uiOverlayDY,	64, 2);
		sdlHandler->renderFillRect(invGfx.screenX+ix*64+62+invGfx.uiOverlayDY, 	invGfx.screenY+iy*64+invGfx.uiOverlayDY,	2, 64);
		sdlHandler->setRenderDrawColor(ridge2);
		sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDX,		invGfx.screenY+iy*64+invGfx.uiOverlayDY, 	2, 64);
		sdlHandler->renderFillRect(invGfx.screenX+ix*64+invGfx.uiOverlayDY,		invGfx.screenY+iy*64+invGfx.uiOverlayDY, 	64, 2);
	}
}

void PlayerMenu::drawHoverText()
{
	//Get hovering information
	int shx = getSlotHoveringX();
	int shy = getSlotHoveringY();
	int type = invhdr.getSlotItemType(shx, shy);
	int mx = controls->getMouseX();
	int my = controls->getMouseY();

	//Draw item name if applicable...
	std::string itemName = Items::getItemName(type);
	if(itemName!="???null???") {		
		TextOld::drawBoxed(sdlHandler, itemName, mx, my-20, Color(255, 255, 255));
	}

	//Draw special hovertext if applicable...
	std::string special = invhdr.getPMM()->getWidgetHoverText(playerGamemode, shx, shy);
	if(special!="???null???") {
		TextOld::drawBoxed(sdlHandler, special, mx, my-20, Color(0, 255, 255));
	}
}

int PlayerMenu::getSlotHoveringX() {
	if(invhdr.getMod()==-1) return -1000;
	return std::floor((controls->getMouseX()-invGfx.screenX)/64.0);
}
int PlayerMenu::getSlotHoveringY() {
	if(invhdr.getMod()==-1) return -1000;
	return std::floor((controls->getMouseY()-invGfx.screenY)/64.0);
}

void PlayerMenu::updateSandboxTile()
{
	/* Set current color */
	Color currentColor(255, 255, 255);
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
	int resrcX = invhdr.getSelX();
	int resrcY = invhdr.getSelY();
	
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

void PlayerMenu::updateInvCoordinates()
{
	//Reset window's XYWH and inventory screen's XYWH
	invGfx.windowX = 0; invGfx.windowY = 0;	invGfx.windowW = 0; invGfx.windowH = 0;
	invGfx.screenX = 0; invGfx.screenY = 0;	invGfx.screenW = 0; invGfx.screenH = 0;	
	
	//Set XYWH of window and invScreen from parentWindow and invW/invH
	Window* parentWindow = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( parentWindow!=nullptr ) {
		//Window
		invGfx.windowX = parentWindow->getSX();		invGfx.windowY = parentWindow->getSY();
		invGfx.windowW = parentWindow->getWidth(); 	invGfx.windowH = parentWindow->getHeight();
		//invScreen
		invGfx.screenX = invGfx.windowX+64*4+2*6;	invGfx.screenW = invGfx.w*64;
		invGfx.screenY = invGfx.windowY+2*4;		invGfx.screenH = invGfx.h*64;
	}
}