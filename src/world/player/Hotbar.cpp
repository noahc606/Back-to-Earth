#include "Hotbar.h"

Hotbar::Hotbar(){}
Hotbar::~Hotbar(){}

void Hotbar::init(Inventory* inv) { Hotbar::inv = inv; }

void Hotbar::tick(Controls* ctrls)
{
	//Change selected hotbar slot using keys
	for(int i = 1; i<=8; i++) {
		std::stringstream ss;
		ss << "HOTBAR_" << i;

		if(ctrls->isPressed(ss.str())) {
			selectedSlot = i-1;

			if(activePMT) 	{	inv->selectSlot(1, i-1, 0); }
 			else 			{	inv->selectSlot(0, i-1, 0);	}

			ctrls->stopPress(ss.str(), __PRETTY_FUNCTION__);
		}
	}

	//Change selected hotbar slot using scroll
	if(!ctrls->isHeld("MAP_USE_ZOOM")) {
		int mw = ctrls->getMouseWheel();
		if(mw!=0) {
			selectedSlot += mw;
			while(selectedSlot<0) selectedSlot += 8;
			while(selectedSlot>7) selectedSlot -= 8;
			ctrls->resetWheel(__PRETTY_FUNCTION__);
		}
	}

	if(ctrls->isPressed("HOTBAR_SWITCH")) {
		if(activePMT) 	{ activePMT = false; }
		else 			{ activePMT = true; }
		
		ctrls->stopPress("HOTBAR_SWITCH", __PRETTY_FUNCTION__);
	}
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
		InvItemStack iis = inv->getSlotItemStack(loc, i, 0);

		//Square background
		SDL_Rect src2 = src;
		src2.w = 32; src2.h = 32;
		SDL_Rect dst2 = dst;
		dst2.x += (2*2); dst2.y += (2*2);
		dst2.w -= (4*2); dst2.h -= (4*2);
		if(iis.getType()==-1) {
			src2.x = 32; sh->renderCopy(texID, &src2, &dst2);
		} else {
			src2.x = 64; sh->renderCopy(texID, &src2, &dst2);
		}

		//Item count
		iis.draw(sh, td, dst.x+6, dst.y+6);
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
		InvItemStack iis = inv->getSlotItemStack(loc, i, 0);
		iis.draw(sh, td, dst.x+6, dst.y+6);
	}

	//Selected slot (in top row)
	SDL_Rect src; src.x = 168; src.y = 160; src.w = 36; src.h = 36;
	SDL_Rect dst; dst.x = hbX+selectedSlot*36*2; dst.y = hbY; dst.w = 36*2; dst.h = 36*2;
	sh->renderCopy(texID, &src, &dst);

	//sdlHandler->setRenderDrawColor()    
}