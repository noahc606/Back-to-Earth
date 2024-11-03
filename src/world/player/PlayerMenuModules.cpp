#include "PlayerMenuModules.h"
#include "Player.h"
#include "PlayerMenu.h"
#include "TextBox.h"
#include "TextureLoader.h"
#include "Tooltip.h"
#include <vector>

PlayerMenuModules::PlayerMenuModules()
{
	widgetsGroupList.insert(std::make_pair(Player::SURVIVAL, std::vector<int>({ BACKPACK, PLASMA_MATTER_TRANSFORMER, MISSION_LOG, CRAFTING })));
	widgetsGroupList.insert(std::make_pair(Player::HARDCORE, std::vector<int>({ BACKPACK, PLASMA_MATTER_TRANSFORMER, MISSION_LOG, CRAFTING })));
	widgetsGroupList.insert(std::make_pair(Player::SANDBOX,  std::vector<int>({ BACKPACK, PLASMA_MATTER_TRANSFORMER, SANDBOX })));
}
PlayerMenuModules::~PlayerMenuModules(){}

void PlayerMenuModules::drawWidgets(SDLHandler* sh, int playerGamemode, int selectedWidgetID, int isx, int isy, int odx, int ody)
{
	std::vector<int> widgetIDs = widgetsGroupList[playerGamemode];
	int ix = 8;

	//Widgets
	for(int i = 0; i<widgetIDs.size(); i++) {
		int iy = i;
		SDL_Rect isrc; isrc.x = PlayerMenu::getItemTexSrcX(widgetIDs[i]); isrc.y = PlayerMenu::getItemTexSrcY(widgetIDs[i]); isrc.w = 32; isrc.h = 32;
		SDL_Rect idst; idst.x = (isx+ix*64+odx); idst.y = (isy+iy*64+ody); idst.w = 64; idst.h = 64;
		sh->renderCopy(TextureLoader::PLAYER_modules, &isrc, &idst);
	}

	//Currently selected widget
	int swid = -1;
	for(int i = 0; i<widgetIDs.size(); i++) {
		if(widgetIDs[i]==selectedWidgetID) {
			swid = i;
		}
	}
	SDL_Rect isrc; isrc.x = 7*32; isrc.y = 0; isrc.w = 32; isrc.h = 32;
	SDL_Rect idst; idst.x = (isx+ix*64+odx); idst.y = (isy+swid*64+ody); idst.w = 64; idst.h = 64;
	sh->renderCopy(TextureLoader::PLAYER_modules, &isrc, &idst);
}

void PlayerMenuModules::drawModuleSandbox(Texture& uiOverlay, SDL_Rect invRect, int invSX, int invSY)
{
	uiOverlay.clear();
	uiOverlay.setTexDimensions(invRect.w, invRect.h);
	uiOverlay.setDrawPos(invRect.x, invRect.y);
	uiOverlay.setDrawScale(2);
	
	uiOverlay.lock();
	uiOverlay.blit(TextureLoader::WORLD_TILE_type_a);
	
	//Selected Slot Outline
	if( invSX!=-1 && invSY!=-1 ) {
		uiOverlay.lock(invSX*32, invSY*32, 32, 32);
		uiOverlay.blit(TextureLoader::GUI_player_interactions, 0, 0 );
	}
}

void PlayerMenuModules::drawModuleMissionLog(SDLHandler* sh, SDL_Rect invRect, MissionHolder& mh)
{
	sh->setRenderDrawColor(nch::Color(0, 0, 0));
	sh->renderFillRect(invRect.x, invRect.y, invRect.w*2, invRect.h*2);

	mh.draw(sh, invRect.x, invRect.y);
}

std::string PlayerMenuModules::getWidgetHoverText(int playerGameMode, int shx, int shy)
{
	std::string res = "???null???";
	if(shx!=8) {
		return res;
	}

	auto itr = widgetsGroupList.find(playerGameMode);
	if(itr!=widgetsGroupList.end()) {
		
		if(shy>=0 && shy<itr->second.size()) {
			switch(itr->second[shy]) {
				case BACKPACK: 					{ res = "Backpack"; } break;
				case PLASMA_MATTER_TRANSFORMER: { res = "Plasma-Matter Transformer"; } break;
				case MISSION_LOG: 				{ res = "Mission Log"; } break;
				case CRAFTING: 					{ res = "Crafting"; } break;
				case SANDBOX: 					{ res = "Sandbox"; } break;
				case STARMAP: 					{ res = "Starmap"; } break;
			}
		}
	}

	return res;
}

int PlayerMenuModules::widgetClicked(int playerGamemode, int shx, int shy)
{
	int res = -1;
	std::vector<int> widgetIDs = widgetsGroupList[playerGamemode];
	if(shy>=0 && shy<widgetIDs.size()) {
		res = widgetIDs[shy];
	}

	return res;
}

void PlayerMenuModules::putMenuInterface(GUIHandler* gh, nch::Color sandboxRGB, int widgetID)
{
	Window* win = gh->getWindow(GUIHandler::win_CHARACTER);
	if( win==nullptr ) {
		return;
	}
	gh->removeGUIs(GUIHandler::ttp_CHARACTER_item, GUIHandler::tbx_CHARACTER_item);

	int dX = 16+4*64;
	int tbxDY = 26;
	int ttpDY = tbxDY+8;

	switch(widgetID) {
		case SANDBOX: {
			gh->addGUI( new Tooltip(win, dX+32*00, 524+32*0+ttpDY, "Red (0-255):", GUIHandler::ttp_CHARACTER_item) );
			TextBox* tbx1 = (TextBox*)gh->addGUI( new TextBox(win, dX+32*06, 524+32*0+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2000 );
			std::stringstream ss1; ss1 << (int)sandboxRGB.r;
			tbx1->setString( ss1.str() );
			
			gh->addGUI( new Tooltip(win, dX+32*00, 524+32*1+ttpDY, "Green (0-255):", GUIHandler::ttp_CHARACTER_item) );
			TextBox* tbx2 = (TextBox*)gh->addGUI( new TextBox(win, dX+32*06, 524+32*1+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2001 );
			std::stringstream ss2; ss2 << (int)sandboxRGB.g;
			tbx2->setString( ss2.str() );

			gh->addGUI( new Tooltip(win, dX+32*00, 524+32*2+ttpDY, "Blue (0-255):", GUIHandler::ttp_CHARACTER_item) );
			TextBox* tbx3 = (TextBox*)gh->addGUI( new TextBox(win, dX+32*06, 524+32*2+tbxDY, 100, TextBox::FREE_NUMBERS_BASIC, GUIHandler::tbx_CHARACTER_item), 2002 );
			std::stringstream ss3; ss3 << (int)sandboxRGB.b;
			tbx3->setString( ss3.str() );
		} break;
		case CRAFTING: {

		} break;
	}
}

void PlayerMenuModules::putMenuInterface(GUIHandler* gh, int widgetID)
{
	putMenuInterface(gh, nch::Color(), widgetID);
}