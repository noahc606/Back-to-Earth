#include "WorldInteractions.h"
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/noah-alloc-table.h>
#include <nch/cpp-utils/timer.h>
#include "ItemStack.h"
#include "Player.h"
#include "RegTexInfo.h"
#include "ShapeBuilder.h"
#include "TileMap.h"
#include "TileIterator.h"

using namespace nch;

WorldInteractions::WorldInteractions(){}
WorldInteractions::~WorldInteractions(){}

void WorldInteractions::loadPlayerEtc(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls, TileDict* td)
{
	sdlHandler = sh;
	guiHandler = gh;
	controls = ctrls;

	/* Player Init */
	localPlayer.init(sh, gh, fh->getSettings(), ctrls);

	/* Player misc. data load */
	//Open NAT
	NoahAllocTable nat("backtoearth/"+worldDirPath+"/playerdata");
	//Load position
	nlohmann::json pos = R"([0, 0, -64])"_json;
	try { pos = nlohmann::json::from_bson(nat.load("pos"))["pos"]; } catch(...) {}
	localPlayer.setPos(pos[0], pos[1], pos[2]);
	//Load mode
	nlohmann::json mode = R"("survival")"_json;
	try{ mode = nlohmann::json::from_bson(nat.load("mode"))["mode"]; } catch(...) {}
	localPlayer.setModeFromStr(mode);
	//Load playtime
	nlohmann::json pt = R"(0)"_json;
	try { pt = nlohmann::json::from_bson(nat.load("playTime"))["playTime"]; } catch(...) {}
	playTime = pt;
	//Close NAT
	nat.close();

	/* Player inventory load */
	//Inventory
	localPlayerMenu.init(sh, gh, ctrls, &localPlayer, td);
	localPlayerMenu.getInventoryHolder()->load(td, worldDirPath);

	/* Other */
	missionHolder.init(worldDirName);	
}

void WorldInteractions::savePlayerEtc()
{
	/* Misc. player info */
	//Open NAT
	NoahAllocTable nat("backtoearth/"+worldDirPath+"/playerdata");
	//Save position
	nlohmann::json jPos; jPos["pos"] = localPlayer.getPos();
	auto bPos = nlohmann::json::to_bson(jPos);
	nat.save("pos", bPos);
	//Save mode
	nlohmann::json jMode; jMode["mode"] = localPlayer.getGameModeStr();
	auto bMode = nlohmann::json::to_bson(jMode);
	nat.save("mode", bMode);
	//Save playtime
	nlohmann::json jPlayTime; jPlayTime["playTime"] = playTime;
	auto bPlayTime = nlohmann::json::to_bson(jPlayTime);
	nat.save("playTime", bPlayTime);
	//NAT close
	nat.close();

	/* Player inventory */
	localPlayerMenu.getInventoryHolder()->save(worldDirPath);
}

void WorldInteractions::draw(Canvas& csInteractions)
{
	Vec3<int64_t> mouseCsPos = localPlayer.getCamera()->getCsPosFromWorldPos(mousePosI*32);

	/* Tile selector */
	csInteractions.clearCanvas();
	csInteractions.setSourceTex(TextureLoader::GUI_player_interactions, 0, 0);
	
	int64_t csx = mouseCsPos.x;					// Get location where tile selector should be
	int64_t csy = mouseCsPos.y;					// ...
	csInteractions.rcopyNI(csx, csy, 32, 32);	// Draw tile selector

	//Draw interactions canvas
	csInteractions.draw();
}

void WorldInteractions::drawLocalPlayerMenu()
{
	//Draw player within menu if player's menu is open
	if(localPlayerMenu.getInventoryHolder()->getMod()>=0) {
		localPlayer.drawCharInMenu();
	}
	
	//Draw specific menu elements
	localPlayerMenu.draw(missionHolder);
}

void WorldInteractions::updateMouseAndCamInfo(Canvas& csInteractions, TileMap* tileMap)
{
	Camera* cam = localPlayer.getCamera();

	//Get screen mouseX and screen mouseY both in double and in long long (int64_t) form.
	Vec3<double> mouseSVD(std::get<0>(csInteractions.getMouseXY()), std::get<1>(csInteractions.getMouseXY()), cam->getLayer());
	Vec3<int64_t> mouseSVI(std::floor(mouseSVD.x), std::floor(mouseSVD.y), std::floor(mouseSVD.z));

	//Get X, Y, and Z region of camera
	Vec3<int64_t> mouseSRV(TileMap::getRegRXYZ(mouseSVI.x), TileMap::getRegRXYZ(mouseSVI.y), TileMap::getRegRXYZ(mouseSVD.z));
	Vec3<int64_t> mouseRV = cam->getWorldPosFromCsPos(mouseSRV);

	TileIterator ti(tileMap);

	mousePosD = cam->getWorldPosFromCsPos(mouseSVD);
	mousePosI = cam->getWorldPosFromCsPos(mouseSVI);
	ti.setBoundsByRXYZ(mouseRV.x, mouseRV.y, mouseRV.z);
	ti.setTrackerSub(TileMap::getRegSubPos(mouseRV.x), TileMap::getRegSubPos(mouseRV.y), TileMap::getRegSubPos(mouseRV.z));
	
	switch(cam->getAxis()) {
		case Camera::X: {
			mousePosI.x = cam->getLayer()+( std::get<0>(RegTexInfo::camTopSolidTile(ti, cam->getDirection(), 32) ) );
			mousePosD.x = mousePosI.x;
		} break;
		case Camera::Y: {
			mousePosI.y = cam->getLayer()+( std::get<0>(RegTexInfo::camTopSolidTile(ti, cam->getDirection(), 32) ) );
			mousePosD.y = mousePosI.y;
		} break;
		case Camera::Z: {
			mousePosI.z = cam->getLayer()+( std::get<0>(RegTexInfo::camTopSolidTile(ti, cam->getDirection(), 32) ) );
			mousePosD.z = mousePosI.z;
		} break;
	}
}

void WorldInteractions::playerController(TileMapScreen* tms, TileMap* tm, bool paused)
{
	//Control opening and closing of player menu
	if( !paused && controls->isPressed("PLAYER_OPEN_INVENTORY") ) {
		playerToggleMenu();
		controls->stopPress("PLAYER_OPEN_INVENTORY", __PRETTY_FUNCTION__);
	}

	//Upon player menu opening/closing...
	if( lpMenuOpenLast!=lpMenuOpen ) {
		lpMenuOpenLast = lpMenuOpen;
		
		//Opening
		if(lpMenuOpen) {
			if(lpMenuLastModule<0) lpMenuLastModule = 0;
			localPlayerMenu.setModule(lpMenuLastModule);
		//Closing
		} else {
			lpMenuLastModule = localPlayerMenu.getInventoryHolder()->getMod();
			localPlayerMenu.setModule(-1);
			localPlayerMenu.resetInteractingTileEntity();
		}
	}

	//Player actions
	if(!lpMenuOpen) {
		playerActions(tms, tm);
	}

	//Commands
	if( Commands::cmdIntResult("boom")!=nullptr ) {
		Commands::resetCMDEntered(__PRETTY_FUNCTION__);

		sdlHandler->getAudioLoader()->play(AudioLoader::SFX_WORLD_antimatter_explosion);
		
		ShapeBuilder sb(tm);
		nch::Timer t("Regional mass destruction", true);
		sb.fillSphere(mousePosI, 128, tm->getTileDict()->at("accrio_air"));

	}
}

void WorldInteractions::playerActions(TileMapScreen* tms, TileMap* tm)
{
	switch(localPlayer.getAction())
	{
		case Player::Action::GM_Apply: {
			TileDict* td = tm->getTileDict();
			Tile sbt = td->at(localPlayerMenu.getSandboxTileID());
			if(sbt.id=="null") {
				sbt = td->at("debug_tile");	
			}
			playerTryPlaceTile(tms, tm, sbt, false);
		}; break;
		case Player::Action::GM_Interact: {
			playerTryDestroyTile(tms, tm);
		}; break;

		case Player::Action::SURV_Apply: {
			ItemStack is = localPlayerMenu.getInventoryHolder()->getSelItemStack();

			if(localPlayerMenu.isHotbarPMTActive()) {
				Tile t;
				if( is.getType()==Items::WORLDTILE ) 			{ t = tm->getTileDict()->at(is.getExtraData()); }
				
				if(t.id!="null") {
					playerTryPlaceTile(tms, tm, t, false);
				}
			} else {
				Tile t;
				if( is.getType()==Items::ANTIMATTER_WARHEAD ) 	{ t = tm->getTileDict()->at("antimatter_warhead"); }
				if( is.getType()==Items::PLASMA_MATTER_STORAGE ){ t = tm->getTileDict()->at("plasma_matter_storage"); }
				if(t.id!="null") {
					playerTryPlaceTile(tms, tm, t, false);
				}
			}
		} break;
		case Player::Action::SURV_Interact: {

			if(localPlayerMenu.isHotbarPMTActive()) {
				playerTryDestroyTile(tms, tm);
			} else {
				playerInteractTile(tms, tm);
			}
		} break;
	}
}

void WorldInteractions::playerInteractTile(TileMapScreen* tms, TileMap* tm)
{
	Camera* cam = localPlayer.getCamera();
	std::string id = tm->getTile(mousePosI).id;

	//Check if tile entity exists for this ID
	if(TileEntity::getTypeFromID(id)==TileEntity::NONE) return;

	//Try to get tile entity, or create it if nonexistent.
	TileEntity* te = tm->getTileEntity(mousePosI);
	if(te==nullptr) {	
		tm->setTileEntity(mousePosI, new TileEntity(id));
	}

	nch::Log::log("Interacting with tile entity \"%s\" @ %s", id.c_str(), mousePosI.toString().c_str());
	playerToggleMenu();
	localPlayerMenu.setInteractingTileEntity(tm, mousePosI);
}

void WorldInteractions::playerTryPlaceTile(TileMapScreen* tms, TileMap* tm, Tile t, bool force)
{
	Camera* cam = localPlayer.getCamera();
	Vec3<int64_t> mouseCsPos = cam->getCsPosFromWorldPos(mousePosI);

	//Determine if we 'canPlace' the tile 't' at current position
	Tile tLast = tm->getTileByCsXYZ(cam, mouseCsPos);
	bool canPlace = false;
	if(localPlayer.inGodMode() || force) {
		canPlace = true;
	} else {
		if(!tLast.solid) {
			canPlace = true;
		}
	}

	double placeDist = 0;
	if(localPlayer.getAction()==Player::SURV_Apply || localPlayer.getAction()==Player::SURV_Interact) {
		placeDist = cam->getCsPos().distanceTo(mouseCsPos.toDouble());
	}
	if(placeDist>4.) {
		canPlace = false;
	}

	if(canPlace) {
		if(t.id!="null")
			tm->setTileByCsXYZ(cam, mouseCsPos, t);
		
		if(localPlayer.getAction()==Player::SURV_Apply) {
			localPlayerMenu.decrementSelectedItemStack();
		}
	}

	//If player just destroyed a tile...
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
			if(tLast.id=="antimatter_warhead") {
				ItemStack is(Items::ANTIMATTER_WARHEAD, 1, tLast.id);
				localPlayerMenu.giveItemStack(is);
			} else
			if(tLast.id=="plasma_matter_storage") {
				ItemStack is(Items::PLASMA_MATTER_STORAGE, 1, tLast.id);
				localPlayerMenu.giveItemStack(is);
			} else {
				ItemStack is(Items::WORLDTILE, 1, tLast.id);
				localPlayerMenu.giveItemStack(is);
			}
		}
	}

	//If player just placed a tile...
	if(canPlace && !force && tLast.id!=t.id) {
		AudioLoader* al = sdlHandler->getAudioLoader();
		al->play(AudioLoader::SFX_WORLD_PLACE_solid_1+(std::rand()%3));
	}

	//Add a 3x3 of tile updates regardless of tile placed or not
	tms->getUpdater()->addTileUpdates(mouseCsPos.x, mouseCsPos.y);
}

void WorldInteractions::playerTryDestroyTile(TileMapScreen* tms, TileMap* tm)
{
	Tile t = tm->getTileDict()->at("generic_air");
	playerTryPlaceTile(tms, tm, t, true);
}

void WorldInteractions::playerToggleMenu()
{
	if(lpMenuOpen) 	{ lpMenuOpen = false; }
	else 			{ lpMenuOpen = true; }
}

void WorldInteractions::playMusic()
{
	AudioLoader* al = sdlHandler->getAudioLoader();
	srand(time(NULL));

	std::vector<int> commonTracks = {
		AudioLoader::MUSIC_kc_alien_ruins,
		AudioLoader::MUSIC_kc_astronomy,
		AudioLoader::MUSIC_kc_digital_sunset,
		AudioLoader::MUSIC_kc_last_stop,
		AudioLoader::MUSIC_kc_nuclear_winter,
		AudioLoader::MUSIC_kc_space_dust,
	};

	
	if(rand()%100==0) {	/* Rare tracks */
		int rare = rand();
		if(rare%3==0) al->playOnce(AudioLoader::MUSIC_kc_50_million_year_trip);
		if(rare%3==1) al->playOnce(AudioLoader::MUSIC_kc_distant_planet);
		if(rare%3==2) al->playOnce(AudioLoader::MUSIC_kc_the_witching_hour);
	
	} else {			/* Common tracks */
		al->playOnce(commonTracks[(rand()%6)]);
	}
}