#include "Hotbar.h"

Hotbar::Hotbar(){}
Hotbar::~Hotbar(){}

void Hotbar::init(InventoryHolder* invhdr)
{
	Hotbar::invhdr = invhdr;
	lastSelectedISlot = std::make_tuple(-1, -1, -1);
}

void Hotbar::tick(Controls* ctrls)
{
	//Change selected hotbar slot using keys
	for(int i = 1; i<=8; i++) {
		std::stringstream ss;
		ss << "HOTBAR_" << i;

		if(ctrls->isPressed(ss.str())) {
			selectedHBSlot = i-1;
			ctrls->stopPress(ss.str(), __PRETTY_FUNCTION__);
		}
	}

	//Change selected hotbar slot using scroll
	if(!ctrls->isHeld("MAP_USE_ZOOM")) {
		int mw = ctrls->getMouseWheel();
		if(mw!=0) {
			selectedHBSlot -= mw;
			while(selectedHBSlot<0) selectedHBSlot += 8;
			while(selectedHBSlot>7) selectedHBSlot -= 8;
			ctrls->resetWheel(__PRETTY_FUNCTION__);
		}
	}

	//Switch hotbar
	if(ctrls->isPressed("HOTBAR_SWITCH")) {
		if(activePMT) 	{ activePMT = false; }
		else 			{ activePMT = true; }
		
		ctrls->stopPress("HOTBAR_SWITCH", __PRETTY_FUNCTION__);
	}

	//Update item selected depending on hotbar
	if(activePMT) 	{	invhdr->selectSlot(1, selectedHBSlot, 0); }
	else 			{	invhdr->selectSlot(0, selectedHBSlot, 0); }
}

void Hotbar::draw(SDLHandler* sh, TileDict* td)
{
	TextureLoader* tl = sh->getTextureLoader();
	int texID = TextureLoader::GUI_player_interactions;
	//Hotbar will be (36*9) by (36*2) pixels
	int hbW = 36*9*2;
	int hbH = 36*2*2;
	int hbX = sh->getWidth()/2-hbW/2;   hbX = hbX/2*2;
	if(hbX<576) { hbX = 576; }
	int hbY = sh->getHeight()-hbH;		hbY = hbY/2*2;

	//Readjust if not enough space
	if(sh->getWidth()<576+hbW) {
		hbX = sh->getWidth()/2-hbW/2; 	hbX = hbX/2*2;
		hbY = 0;
	}
	
	//Active slots (top row)
	for(int i = 0; i<8; i++) {
		SDL_Rect src; src.x = 132; src.y = 160; src.w = 36; src.h = 36;
		SDL_Rect dst; dst.x = hbX+i*36*2; dst.y = hbY; dst.w = 72; dst.h = 72;
		sh->renderCopy(texID, &src, &dst);

		int loc = 0;
		if(activePMT) loc = 1;
		ItemStack is = invhdr->getSlotItemStack(loc, i, 0);

		//Square background
		SDL_Rect src2 = src;
		src2.w = 32; src2.h = 32;
		SDL_Rect dst2 = dst;
		dst2.x += (2*2); dst2.y += (2*2);
		dst2.w -= (4*2); dst2.h -= (4*2);
		if(is.getType()==-1) {
			src2.x = 32; sh->renderCopy(texID, &src2, &dst2);
		} else {
			src2.x = 64; sh->renderCopy(texID, &src2, &dst2);
		}

		//Item count
		is.draw(sh, td, dst.x+6, dst.y+6);
	}
	
	//Inactive slots (bottom row)
	for(int i = 0; i<8; i++) {
		SDL_Rect src; src.x = 96; src.y = 160; src.w = 36; src.h = 36;
		SDL_Rect dst; dst.x = hbX+i*36*2; dst.y = hbY+72; dst.w = 72; dst.h = 72;
		sh->renderCopy(texID, &src, &dst);

		//Square background
		SDL_Rect src2 = src;
		SDL_Rect dst2 = dst;
		dst2.x += (2*2); dst2.y += (2*2);
		dst2.w -= (4*2); dst2.h -= (4*2);
		
		src2.x = 0;
		src2.w = 32; src2.h = 32;
		sh->renderCopy(texID, &src2, &dst2);

		int loc = 1;
		if(activePMT) loc = 0;
		ItemStack is = invhdr->getSlotItemStack(loc, i, 0);
		is.draw(sh, td, dst.x+6, dst.y+6);
	}

	//Selected slot (in top row)
	SDL_Rect src; src.x = 168; src.y = 160; src.w = 36; src.h = 36;
	SDL_Rect dst; dst.x = hbX+selectedHBSlot*36*2; dst.y = hbY; dst.w = 36*2; dst.h = 36*2;
	sh->renderCopy(texID, &src, &dst);

	//sdlHandler->setRenderDrawColor()    
}


std::tuple<int8_t, int8_t, int8_t> Hotbar::getLastSelectedInvSlot() { return lastSelectedISlot; }
bool Hotbar::isPMTActive() { return activePMT; }


void Hotbar::setLastSelectedInvSlot(int8_t sl, int8_t sx, int8_t sy) { lastSelectedISlot = std::make_tuple(sl, sx, sy); }