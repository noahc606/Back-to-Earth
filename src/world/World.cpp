#include "World.h"
#include <math.h>
#include "TextureBuilder.h"
#include "Log.h"
#include "MainLoop.h"
#include "Timer.h"

World::World(){}

void World::init(SDLHandler* sh, GUIHandler* gh, FileHandler* fh, Controls* ctrls)
{
	if( exists() ) return;
	BTEObject::init(sh, fh, ctrls);
	guiHandler = gh;

	Commands::cKV("x1", -16);
	Commands::cKV("y1", -16);

	//Init player, tileMap, tileMapScreen.
	localPlayer.init(sh, guiHandler, ctrls);
	localPlayer.setPos(0, 0, -32);
	tileMap.init(sdlHandler, fileHandler);
	tileMapScreen.init(sdlHandler, fileHandler, controls, &localPlayer, &tileMap, &csTileMap);

	//Init csTileMap
	csTileMap.init(sdlHandler, controls, localPlayer.getCamera());
	csTileMap.setTexUsingDynamicLOD(true);
	csTileMap.setTexAllocCount(1);
	csTileMap.setTexAllocRadiusX(1);
	csTileMap.setTexAllocRadiusY(1);

	//Init csInteractions
	csInteractions.init(sdlHandler, controls, localPlayer.getCamera());
	csInteractions.setMaximumFPS(20);
	csInteractions.setTexAllocRadiusX(1);
	csInteractions.setTexAllocRadiusY(1);

	//Init csEntities
	csEntities.init(sdlHandler, controls, localPlayer.getCamera());
	csEntities.setTexAllocRadiusX(1);
	csEntities.setTexAllocRadiusY(1);

	//Build textures
	TextureBuilder tb(sdlHandler);
	tb.init(tb.TILE_default, defaultTile);
}

World::~World()
{
	//player.destroy();
	//tileMap.destroy();
	//tileMapScreen.destroy();
	
	//defaultTile.destroy();

	csTileMap.destroy();
	csInteractions.destroy();
	csEntities.destroy();
}

void World::draw()
{
	/** Tile Canvas */
	tileMapScreen.draw();

	/** Entity Canvas */
	csEntities.clearCanvas();
	localPlayer.draw(&csEntities);
	csEntities.draw();


	/** Interactions Canvas */
	csInteractions.clearCanvas();
	csInteractions.setSourceTex(TextureLoader::GUI_player_interactions, 0, 0);
	csInteractions.rcopyNI( 32*mouseXLL, 32*mouseYLL, 32, 32 );
	csInteractions.draw();
	
	/** HUDs */
	localPlayer.drawHUD();
}

void World::tick(bool paused, GUIHandler& guiHandler)
{
	performanceCounter = 0;
	Timer t("World tick timer", false);

	/** Tick world objects if not paused */
	if( !paused ) {
		/** World objects */
		localPlayer.tick();
		localPlayer.getMenu()->tick();
		tileMapScreen.tick();
	} else {
		//Force-disable character menu when paused
		lpMenuState = 0;
	}

	/** Canvases */
	csTileMap.tick();
	csInteractions.tick();
	csEntities.tick();

	/** Interactions with world */
	updateMouseAndCamInfo();
	playerInteractions(guiHandler, paused);

	performanceCounter = t.getElapsedTimeMS();
}

void World::putInfo(std::stringstream& ss, int& tabs)
{
	if(localPlayer.getCamera()==nullptr) return;

	//Mouse Info
	DebugScreen::indentLine(ss, tabs);
	ss << "Mouse(XYZ)=(" << mouseXLL << ", " << mouseYLL << ", " << mouseZLL << "); ";
	ss << "Mouse(xy)=(" << mouseX << ", " << mouseY << "); ";

	DebugScreen::newLine(ss);
	DebugScreen::indentLine(ss, tabs);
	ss << "World tick=" << performanceCounter << "ms; ";
	DebugScreen::newLine(ss);


	//Player
	DebugScreen::newGroup(ss, tabs, "World::player");
	localPlayer.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMap
	DebugScreen::newGroup(ss, tabs, "World::tileMap");
	if(localPlayer.getCamera()!=nullptr)
		tileMap.putInfo(ss, tabs);
	DebugScreen::endGroup(tabs);

	//TileMapScreen
	DebugScreen::newGroup(ss, tabs, "World::tileMapScreen");
	tileMapScreen.putInfo(ss, tabs, mouseXLL, mouseYLL, mouseZLL);
	DebugScreen::endGroup(tabs);
}

Player* World::getLocalPlayer()
{
	return &localPlayer;
}

void World::updateMouseAndCamInfo()
{
	//Get mouseX and mouseY both in double and in long long (t_ll) form.
	mouseX = std::get<0>(csInteractions.getMouseXY());
	mouseXLL = (int64_t)floor(mouseX);
	mouseY = std::get<1>(csInteractions.getMouseXY());
	mouseYLL = (int64_t)floor(mouseY);

	//Get Z region of camera
	long cRZ = TileMap::getRegRXYZ(localPlayer.getCamera()->getLayer());

	//Get mouseZLL - the position of the topmost visible tile when looking down from (mouseXLL, mouseYLL, camera layer).
	TileIterator ti(&tileMap);
	ti.setBoundsByRXYZ( TileMap::getRegRXYZ(mouseXLL), TileMap::getRegRXYZ(mouseYLL), cRZ);
	ti.setTrackerSub( TileMap::getRegSubPos(mouseXLL), TileMap::getRegSubPos(mouseYLL), TileMap::getRegSubPos(localPlayer.getCamera()->getLayer()) );
	mouseZLL = localPlayer.getCamera()->getLayer() +( std::get<0>(RegTexUpdates::topTrackedTile(ti)) );
	
	//Get mouseCLL - position of camera layer.
	mouseCLL = localPlayer.getCamera()->getLayer();
}

void World::playerInteractions(GUIHandler& guiHandler, bool paused)
{
	/*
		//Get localPlayer x, y, and z both in double and in t_ll form.
		double pxd = std::get<0>(localPlayer.getPos());
		int px = floor(pxd);
		double pyd = std::get<1>(localPlayer.getPos());
		int py = floor(pyd);
		double pzd = std::get<2>(localPlayer.getPos());
		int pz = floor(pzd);
	*/

	//Control character menu state
	if( !paused && controls->isPressed("PLAYER_INVENTORY") ) {
		if(lpMenuState) {
			lpMenuState = false;
		} else {
			lpMenuState = true;
		}
		controls->stopPress("PLAYER_INVENTORY", __PRETTY_FUNCTION__);
	}
	
	if( lpMenuStateLast!=lpMenuState ) {
		lpMenuStateLast = lpMenuState;
		localPlayer.setMenuState(lpMenuState);
		
		if( lpMenuState ) {
			localPlayer.setMenuState(1);
		} else {
			localPlayer.setMenuState(0);
		}
	}

	if( !lpMenuState ) {
		switch( localPlayer.getAction() )
		{
			case Player::Action::GM_Destroy_Tile: {
				if( tileMap.getTile(mouseXLL, mouseYLL, mouseCLL).getVal()!=0x0 ) {
					TileType tt;
					tt.init();
					tt.setTechnical(true);
					tileMap.setTile(mouseXLL, mouseYLL, mouseCLL, tt);
				}
				tileMap.addTileUpdates(mouseXLL, mouseYLL, mouseCLL);
			}; break;
			case Player::Action::GM_Place_Tile: {
				
				TileIterator ti(&tileMap);
				int squareSize = 1;
				ti.setBounds(
					mouseXLL-squareSize,
					mouseYLL-squareSize,
					localPlayer.getCamera()->getLayer(),
					mouseXLL+squareSize,
					mouseYLL+squareSize,
					localPlayer.getCamera()->getLayer()
				);

				while( !ti.invalidIndex() ) {
					TileRegion* tr = ti.peekRegion();
					if( tr!=nullptr ) {
						for( int sx = ti.gbs(0); sx<=ti.ges(0); sx++ ) {
							for( int sy = ti.gbs(1); sy<=ti.ges(1); sy++ ) {
								for( int sz = ti.gbs(2); sz<=ti.ges(2); sz++ ) {

									int64_t tileX = ti.getItrReg(0)+sx;
									int64_t tileY = ti.getItrReg(1)+sy;
									int64_t tileZ = ti.getItrReg(2)+sz;

									TileType tt;
									tt.init();
									if( std::abs(tileX+tileY)%3==0 ) {
										tt.setRGB(200, 100, 100);
									} else if ( std::abs(tileX+tileY)%3==1 ) {
										tt.setRGB(20, 255, 255);
									} else {
										tt.setRGB( std::abs(tileX)%64*4, std::abs(tileY)%64*4, std::abs(tileZ)%64*4 );
									}
									tt.setSolid(true);
									tt.setTextureXY(0, 2);
									tt.setVisionBlocking(true);

									tr->setTile( sx, sy, sz, tt );
								}
							}
						}
					}

					ti.nextRegion();
				}


				tileMap.addTileUpdates(
					mouseXLL-squareSize-1,
					mouseYLL-squareSize-1,
					localPlayer.getCamera()->getLayer(),
					mouseXLL+squareSize+1,
					mouseYLL+squareSize+1,
					localPlayer.getCamera()->getLayer() );
			}; break;
		}
	}
}
