#include "PlayerMenu.h"
#include <cmath>
#include "TextBox.h"
#include "Tooltip.h"

void PlayerMenu::init(SDLHandler* sh, GUIHandler* gh, Controls* ctrls, Player* pl)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;
	player = pl;
	
	for( int x = 0; x<invW; x++ ) {
		for( int y = 0; y<invH; y++ ) {
			inventorySlots[x][y] = -1;
		}
	}
	
	inventorySlots[3][0] = ION_BLASTER;
	inventorySlots[4][1] = GEOPORTER;
	inventorySlots[6][2] = SOLID_PLASMA_ANNIHILATOR;
	inventorySlots[6][3] = ATOM_PRINTER;
	inventorySlots[7][0] = NYLON_EXOSUIT_HELMET;
	inventorySlots[7][1] = NYLON_EXOSUIT_BODY;
	inventorySlots[7][2] = NYLON_EXOSUIT_LEGGINGS;
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
			putItemInterface(-1);
		}
		if(state==2) {
			putItemInterface(SANDBOX);
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
			putItemInterface( inventorySlots[invSX][invSY] );
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
					
					switch(i) {
						case 0: sandboxRed = colorInt; break;
						case 1: sandboxGreen = colorInt; break;
						case 2: sandboxBlue = colorInt; break;
					}
					
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
uint8_t PlayerMenu::getSandboxTexRed() { return sandboxRed; } 
uint8_t PlayerMenu::getSandboxTexGreen() { return sandboxGreen; } 
uint8_t PlayerMenu::getSandboxTexBlue() { return sandboxBlue; } 

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
					uiOverlay.blit(TextureLoader::PLAYER_items, inventorySlots[x][y]*32, 0);
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

std::string PlayerMenu::getItemName(int itemID)
{
	switch(itemID) {
	case SANDBOX:					return "Tile Selection: Texture & Color";
	case ION_BLASTER: 				return "Ion Blaster";
	case GEOPORTER:					return "Geo-porter";
	case SOLID_PLASMA_ANNIHILATOR:	return "Solid-Plasma Annihilator";
	case ATOM_PRINTER:				return "Atom Printer";
	case NYLON_EXOSUIT_HELMET:		return "Nylon Exosuit Helmet";
	case NYLON_EXOSUIT_BODY:		return "Nylon Exosuit Body";
	case NYLON_EXOSUIT_LEGGINGS:	return "Nylon Exosuit Leggings";
	case INFINITE_OXYGEN_TANK:		return "Infinite Oxygen Tank";
	default: 						return "";
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

void PlayerMenu::putItemInterface(int itemID)
{
	Window* win = guiHandler->getWindow(GUIHandler::win_CHARACTER);
	if( win==nullptr ) {
		return;
	}
	
	guiHandler->removeGUIs(GUIHandler::ttp_CHARACTER_item, GUIHandler::tbx_CHARACTER_item);

	int dX = 12;
	int tbxDY = 26;
	int ttpDY = tbxDY+8;
	
	//Add tooltip for every item type
	guiHandler->addGUI( new Tooltip(win, dX, 524+4, getItemName(itemID), GUIHandler::ttp_CHARACTER_item) );
	
	switch( itemID ) {
	case SANDBOX: {
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "Red (0-255):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx1 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*0+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2000 );
		std::stringstream ss1; ss1 << (int)sandboxRed;
		tbx1->setString( ss1.str() );
		
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*1+ttpDY, "Green (0-255):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx2 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*1+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2001 );
		std::stringstream ss2; ss2 << (int)sandboxGreen;
		tbx2->setString( ss2.str() );

		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*2+ttpDY, "Blue (0-255):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx3 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*2+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2002 );
		std::stringstream ss3; ss3 << (int)sandboxBlue;
		tbx3->setString( ss3.str() );
	} break;
	case GEOPORTER: {
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "X-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx1 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*0+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1000 );
		std::stringstream ss1; ss1 << (int)std::get<0>(player->getPos());
		tbx1->setString( ss1.str() );
		
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*1+ttpDY, "Y-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx2 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*1+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1001 );
		std::stringstream ss2; ss2 << (int)std::get<1>(player->getPos());
		tbx2->setString( ss2.str() );

		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*2+ttpDY, "Z-position (meters):", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx3 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*06, 524+32*2+tbxDY, 100, TextBox::FREE_NUMBERS_INTEGERS, GUIHandler::tbx_CHARACTER_item), 1002 );
		std::stringstream ss3; ss3 << (int)std::get<2>(player->getPos());
		tbx3->setString( ss3.str() );

		guiHandler->addGUI( new Tooltip(win, dX+32*10, 524+32*0+ttpDY, "Planet ID:", GUIHandler::ttp_CHARACTER_item) );
		TextBox* tbx4 = (TextBox*)guiHandler->addGUI( new TextBox(win, dX+32*13+4, 524+32*0+tbxDY, 350, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 1003 );
		
		
		guiHandler->addGUI( new Button (win, dX+32*10, 524+32*2+tbxDY, 200, "Teleport", GUIHandler::btn_CHARACTER_item), 1004 );
	} break;
	case SOLID_PLASMA_ANNIHILATOR: {
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "Destroy solid matter with Left Click", GUIHandler::ttp_CHARACTER_item) );
	} break;
	case ATOM_PRINTER: {
		guiHandler->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "Build tiles with Right Click - select the tile type from Engineering", GUIHandler::ttp_CHARACTER_item) );
	} break;
	}
}