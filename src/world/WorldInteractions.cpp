#include "WorldInteractions.h"
#include "InvItemStack.h"
#include "Player.h"
#include "RegTexInfo.h"
#include "TileMap.h"
#include "TileIterator.h"
#include "Vec3F.h"

WorldInteractions::WorldInteractions(){}
WorldInteractions::~WorldInteractions(){}

void WorldInteractions::initPlayerEtc(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls, TileDict* td, std::tuple<double, double, double> pXYZ, std::string pMode)
{
	sdlHandler = sh;

	//Player
	localPlayer.init(sh, gh, fh->getSettings(), ctrls);
	localPlayer.setModeFromStr(pMode);
	localPlayer.setPos(std::get<0>(pXYZ)-0.5, std::get<1>(pXYZ)-0.5, std::get<2>(pXYZ));
	//Player menus
	localPlayerMenu.init(sh, gh, ctrls, &localPlayer, td);
	localPlayerMenu.getInventory()->load(td, worldDirPath);

	missionHolder.init(worldDirName);	
}

void WorldInteractions::draw(Canvas& csInteractions)
{
	/* Tile selector */
	csInteractions.clearCanvas();
	csInteractions.setSourceTex(TextureLoader::GUI_player_interactions, 0, 0);					// Set tex src to GUI_player_interactions
	int64_t csx = localPlayer.getCamera()->getCsXFromPos(32*mouseXL, 32*mouseYL, 32*mouseZL);	// Get location where tile selector should be
	int64_t csy = localPlayer.getCamera()->getCsYFromPos(32*mouseXL, 32*mouseYL, 32*mouseZL);	// ...
	csInteractions.rcopyNI(csx, csy, 32, 32);													// Draw tile selector
	csInteractions.draw();
}

void WorldInteractions::drawLocalPlayerMenu()
{
	//Draw player within menu if player's menu is open
	if(localPlayerMenu.getInventory()->getMod()>=0) {
		localPlayer.drawCharInMenu();
	}
	
	//Draw specific menu elements
	localPlayerMenu.draw(missionHolder);
}

void WorldInteractions::updateMouseAndCamInfo(Canvas& csInteractions, TileMap* tileMap)
{
	//Get screen mouseX and screen mouseY both in double and in long long (int64_t) form.
	double mouseSX = std::get<0>(csInteractions.getMouseXY());
	int64_t mouseSXL = (int64_t)floor(mouseSX);
	double mouseSY = std::get<1>(csInteractions.getMouseXY());
	int64_t mouseSYL = (int64_t)floor(mouseSY);

	//Get X, Y, and Z region of camera
	int64_t mouseSRX = TileMap::getRegRXYZ(mouseSXL);
	int64_t mouseSRY = TileMap::getRegRXYZ(mouseSYL);
	int64_t cSL = localPlayer.getCamera()->getLayer();
	int64_t cSRZ = TileMap::getRegRXYZ(cSL);
	
	TileIterator ti(tileMap);
	

	switch( localPlayer.getCamera()->getAxis() ) {
		case Camera::X: {
			mouseY = mouseSX;
			mouseYL = mouseSXL;
			mouseZ = mouseSY;
			mouseZL = mouseSYL;
			
			ti.setBoundsByRXYZ( cSRZ, mouseSRX, mouseSRY );
			ti.setTrackerSub( TileMap::getRegSubPos(cSL), TileMap::getRegSubPos(mouseSXL), TileMap::getRegSubPos(mouseSYL) );
			
			mouseXL = cSL+( std::get<0>(RegTexInfo::camTopSolidTile(ti, localPlayer.getCamera()->getDirection(), 32) ) );
		} break;
		case Camera::Y: {
			mouseX = mouseSX;
			mouseXL = mouseSXL;
			mouseZ = mouseSY;
			mouseZL = mouseSYL;
			
			ti.setBoundsByRXYZ( cSRZ, mouseSRY, mouseSRX );
			ti.setTrackerSub( TileMap::getRegSubPos(cSL), TileMap::getRegSubPos(mouseSYL), TileMap::getRegSubPos(mouseSXL) );
			
			mouseYL = cSL+( std::get<0>(RegTexInfo::camTopSolidTile(ti, localPlayer.getCamera()->getDirection(), 32) ) );
		} break;
		case Camera::Z: {
			mouseX = mouseSX;
			mouseXL = mouseSXL;
			mouseY = mouseSY;
			mouseYL = mouseSYL;
			
			ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseSXL), TileMap::getRegRXYZ(mouseSYL), cSRZ);
			ti.setTrackerSub( TileMap::getRegSubPos(mouseSXL), TileMap::getRegSubPos(mouseSYL), TileMap::getRegSubPos(cSL) );
			
			mouseZL = cSL+( std::get<0>(RegTexInfo::camTopSolidTile(ti, localPlayer.getCamera()->getDirection(), 32) ) );
		} break;
	}
}

void WorldInteractions::playerInteractions(SDLHandler* sdlHandler, GUIHandler& guiHandler, Controls* controls, TileMapScreen* tms, TileMap* tm, bool paused)
{
	//Control character menu state
	if( !paused && controls->isPressed("PLAYER_INVENTORY") ) {
		if(lpMenuOpen) {
			lpMenuOpen = false;
		} else {
			lpMenuOpen = true;
		}
		controls->stopPress("PLAYER_INVENTORY", __PRETTY_FUNCTION__);
	}
	
	if( lpMenuOpenLast!=lpMenuOpen ) {
		lpMenuOpenLast = lpMenuOpen;
		
		if( lpMenuOpen ) {
			if(lpMenuLastModule<0) lpMenuLastModule = 0;
			setLocalPlayerMenuState(lpMenuLastModule);
		} else {
			lpMenuLastModule = localPlayerMenu.getInventory()->getMod();
			setLocalPlayerMenuState(-1);
		}
	}

	//Player actions
	if( !lpMenuOpen ) {		
		bool audio = false;
		AudioLoader* al = sdlHandler->getAudioLoader();
		if(al!=nullptr) {
			audio = true;
		}
				
		switch( localPlayer.getAction() )
		{
			case Player::Action::GM_Destroy_Tile: {
				playerTryDestroyTile(tms, tm);
			}; break;
			case Player::Action::GM_Place_Tile: {
				TileDict* td = tm->getTileDict();
				Tile sbt = td->at(localPlayerMenu.getSandboxTileID());
				if(sbt.id=="null") {
					sbt = td->at("debug_tile");	
				}
				playerTryPlaceTile(tms, tm, sbt, false);
			}; break;

			case Player::Action::SURV_Destroy_Tile: {
				playerTryDestroyTile(tms, tm);
			} break;
			case Player::Action::SURV_Place_Tile: {
				InvItemStack iis = localPlayerMenu.getInventory()->getSelItemStack();
				if( iis.getType()==Items::WORLDTILE ) {
					Tile t = tm->getTileDict()->at(iis.getExtraData());
					if(t.id!="null") {
						playerTryPlaceTile(tms, tm, t, false);
					}
				}
			} break;
		}
	}
}

void WorldInteractions::playerTryPlaceTile(TileMapScreen* tms, TileMap* tm, Tile t, bool force)
{
	Camera* cam = localPlayer.getCamera();
	
	int64_t mouseCsXL = mouseXL;
	int64_t mouseCsYL = mouseYL;
	int64_t mouseCsZL = mouseZL;
	switch(localPlayer.getCamera()->getAxis()) {
		case Camera::X: {
			mouseCsXL = mouseYL;
			mouseCsYL = mouseZL;
			mouseCsZL = mouseXL;
		} break;
		case Camera::Y: {
			mouseCsXL = mouseXL;
			mouseCsYL = mouseZL;
			mouseCsZL = mouseYL;
		} break;
	}

	//Try to place tile
	Tile tLast = tm->getTileByCsXYZ(cam, mouseCsXL, mouseCsYL, cam->getLayer());
	bool canPlace = false;
	if( localPlayer.inGodMode() || force ) {
		canPlace = true;
	} else {
		if( !tLast.solid ) {
			canPlace = true;
		}
	}

	double placeDist = 0;
	if(localPlayer.getAction()==Player::SURV_Destroy_Tile || localPlayer.getAction()==Player::SURV_Place_Tile) {
		Point3X<double> canvasCam(localPlayer.getCamera()->getCsX(), localPlayer.getCamera()->getCsY(), localPlayer.getCamera()->getCsZ());
		Point3X<double> canvasMouse(mouseCsXL, mouseCsYL, mouseCsZL);
		placeDist = Vec3F::dist(canvasCam.x, canvasCam.y, canvasCam.z, canvasMouse.x, canvasMouse.y, canvasMouse.z);
	}
	if(placeDist>4.) {
		canPlace = false;
	}

	if(canPlace) {
		tm->setTileByCsXYZ(cam, mouseCsXL, mouseCsYL, cam->getLayer(), t);
		
		if(localPlayer.getAction()==Player::SURV_Place_Tile) {
			localPlayerMenu.decrementSelectedItemStack();
		}
	}

	//If just destroyed a tile...
	if(canPlace && tLast.solid && tLast.id!="null" && force) {
		//Play sound
		AudioLoader* al = sdlHandler->getAudioLoader();
 			switch(tLast.material) {
			case Tile::Material::METAL: { al->play(AudioLoader::SFX_WORLD_DESTROY_metal_1+(std::rand()%3)); } break;
			case Tile::Material::ROCK: 	{ al->play(AudioLoader::SFX_WORLD_DESTROY_rock_1+(std::rand()%3)); } break;
			case Tile::Material::SAND: 	{ al->play(AudioLoader::SFX_WORLD_DESTROY_sand_1+(std::rand()%3)); } break;
			case Tile::Material::SOIL: 	{ al->play(AudioLoader::SFX_WORLD_DESTROY_soil_1+(std::rand()%3)); } break;
		}
		
 		//In survival mode, add item to inventory
		if(!localPlayer.inGodMode()) {
			InvItemStack iis(Items::WORLDTILE, 1, tLast.id);
			localPlayerMenu.giveItemStack(iis);
		}
	}

	//Add a 3x3 of tile updates regardless of tile placed or not
	switch(cam->getAxis()) {
		case Camera::X: { tms->getUpdater()->addTileUpdates(mouseYL, mouseZL); } break;
		case Camera::Y: { tms->getUpdater()->addTileUpdates(mouseXL, mouseZL); } break;
		case Camera::Z: { tms->getUpdater()->addTileUpdates(mouseXL, mouseYL); } break;
	}
}

void WorldInteractions::playerTryDestroyTile(TileMapScreen* tms, TileMap* tm)
{
	Tile t = tm->getTileDict()->at("generic_air");
	playerTryPlaceTile(tms, tm, t, true);
}

void WorldInteractions::setLocalPlayerMenuState(int newMenuState)
{
	localPlayerMenu.setModule(newMenuState);
}