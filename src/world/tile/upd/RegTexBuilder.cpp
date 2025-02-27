#include "RegTexBuilder.h"
#include "DebugScreen.h"
#include <math.h>

RegTexBuilder::RegTexBuilder(Texture* tex, Planet* plnt, TileIterator& ti, int camDirection, int blitScale, TileDict* tileDict, std::vector<std::pair<int64_t, Tile>>& tilesToDraw)
{
	//Store blit's texture, size and destinationo
	RegTexBuilder::tex = tex;
	RegTexBuilder::plnt = plnt;
	RegTexBuilder::blitScale = blitScale;

	switch (Camera::getAxisFromDirection(camDirection)) {
		case Camera::X: {
			dstX = blitScale*ti.getTrackerSub(1);
			dstY = blitScale*ti.getTrackerSub(2);
		} break;
		case Camera::Y: {
			dstX = blitScale*ti.getTrackerSub(0);
			dstY = blitScale*ti.getTrackerSub(2);
		} break;
		default: {
			dstX = blitScale*ti.getTrackerSub(0);
			dstY = blitScale*ti.getTrackerSub(1);
		} break;
	}
	
	//RegTexBuilder::ttdfc = ttdfc;
	bool lookingDown = camDirection==Camera::Directions::DOWN;
	
	switch(tilesToDraw.size()) {
		case 0: {
			//If the camera is looking far downward, draw darkness instead of the sky
			if(lookingDown) {
				Tile t;
				drawTypeA(tileDict, t, camDirection, 0);
			//If not, draw invisibilty for the sky
			} else {
				tex->rect(dstX, dstY, blitScale, blitScale, 0, 0, 0, 0, SDL_BLENDMODE_NONE);
			}
		} break;
		case 1: {
			if(tilesToDraw[0].second.isVisionBlocking(camDirection)) {
				drawTypeA(tileDict, tilesToDraw[0].second, camDirection, tilesToDraw[0].first);
				
				//Detail tiles further if they are depth 0
				if(tilesToDraw[0].first==0) {
					detailDepth0Tiles(ti, camDirection);
				}
			} else {
				tex->rect(dstX, dstY, blitScale, blitScale, 192, 192, 192, 0, SDL_BLENDMODE_NONE);
				drawTypeA(tileDict, tilesToDraw[0].second, camDirection, tilesToDraw[0].first);
			}
		} break;
		default: {
			tex->rect(dstX, dstY, blitScale, blitScale, 64, 64, 64, 0, SDL_BLENDMODE_NONE);
			for(int i = tilesToDraw.size()-1; i>=0; i--) {
				drawTypeA(tileDict, tilesToDraw[i].second, camDirection, tilesToDraw[i].first);
			}
		} break;
	}
}

RegTexBuilder::~RegTexBuilder(){}

void RegTexBuilder::info(std::stringstream& ss, int& tabs, TileIterator& ti, int ttdfc)
{
	bool bl[8] = { 0, 0, 0, 0, 0, 0, 0, 0 }; // "Block-light" tile locations
	infoBl(ti, bl, ttdfc);

	bool co[4] = { 0, 0, 0, 0 }; //Corners
	bool si[4] = { 0, 0, 0, 0 }; //Sides
	bool sh[4] = { 1, 1, 1, 1 };  //Shadows
	infoCoSiSh(bl, co, si, sh);

	DebugScreen::indentLine(ss, tabs);
	ss << "bl={" << bl[0]<<bl[1]<<bl[2]<<bl[3]<<bl[4]<<bl[5]<<bl[6]<<bl[7] << "}, ";
	ss << "co={" << co[0]<<co[1]<<co[2]<<co[3] << "}, ";
	ss << "si={" << si[0]<<si[1]<<si[2]<<si[3] << ", ";
	ss << "sh={" << sh[0]<<sh[1]<<sh[2]<<sh[3] << "}; ";
	DebugScreen::newLine(ss);
}

void RegTexBuilder::infoBl(TileIterator& ti, bool bl[8], int ttdfc, int camDirection)
{
	//'bl' = represents a tile that is opaque
	//012
	//3 4
	//567
	
	// 0 = Camera looking toward X axis
	// 1 = ... Y axis
	// 2 = ... Z axis
	int axis = Camera::getAxisFromDirection(camDirection);
	
	for( int i = 0; i<8; i++ ) {
		int c1 = 0;
		int c2 = 0;
		
		switch(i) {
		case 0: c1 = -1; c2 = -1; break;
		case 1: c1 = 00; c2 = -1; break;
		case 2: c1 = 01; c2 = -1; break;
		case 3: c1 = -1; c2 = 00; break;
		case 4: c1 = 01; c2 = 00; break;
		case 5: c1 = -1; c2 = 01; break;
		case 6: c1 = 00; c2 = 01; break;
		case 7: c1 = 01; c2 = 01; break;
		}
	
		switch(axis){
		case 0: bl[i] = ti.peekTrackedTile(ttdfc, c1, c2).isVisionBlocking(camDirection); break;
		case 1: bl[i] = ti.peekTrackedTile(c1, ttdfc, c2).isVisionBlocking(camDirection); break;
		case 2: bl[i] = ti.peekTrackedTile(c1, c2, ttdfc).isVisionBlocking(camDirection); break;
		}
	}
}
void RegTexBuilder::infoBl(TileIterator& ti, bool bl[8], int camDirection) { infoBl(ti, bl, 0, camDirection); }


void RegTexBuilder::infoCoSi(bool bl[8], bool co[4], bool si[4])
{
	//Set corners
	if( !bl[0]&&bl[1]&&bl[3] ) co[0] = 1;
	if( !bl[2]&&bl[1]&&bl[4] ) co[1] = 1;
	if( !bl[5]&&bl[3]&&bl[6] ) co[2] = 1;
	if( !bl[7]&&bl[4]&&bl[6] ) co[3] = 1;

	//Set sides
	if( !bl[1] ) si[0] = 1;
	if( !bl[3] ) si[1] = 1;
	if( !bl[4] ) si[2] = 1;
	if( !bl[6] ) si[3] = 1;

	//Anywhere a side intersects the corner of the square, that corner = 1.
	if( si[0] ) { co[0] = 1; co[1] = 1; }
	if( si[1] ) { co[0] = 1; co[2] = 1; }
	if( si[2] ) { co[1] = 1; co[3] = 1; }
	if( si[3] ) { co[2] = 1; co[3] = 1; }
}

void RegTexBuilder::infoSh(bool co[4], bool si[4], bool sh[4])
{
	//Set shadows
	if( si[0] ) { sh[0] = 0; sh[1] = 0; }
	if( si[1] ) { sh[0] = 0; sh[2] = 0; }
	if( si[2] ) { sh[1] = 0; sh[3] = 0; }
	if( si[3] ) { sh[2] = 0; sh[3] = 0; }
	if( co[0] ) { sh[0] = 0; }
	if( co[1] ) { sh[1] = 0; }
	if( co[2] ) { sh[2] = 0; }
	if( co[3] ) { sh[3] = 0; }
}

void RegTexBuilder::infoCoSiSh(bool bl[8], bool co[4], bool si[4], bool sh[4])
{
    infoCoSi(bl, co, si);
    infoSh(co, si, sh);
}

void RegTexBuilder::detailDepth0Tiles(TileIterator& ti, int camDirection)
{
	bool bl[8] = { false };
	infoBl(ti, bl, camDirection);

	bool co[4] = { 0, 0, 0, 0 }; 	//Corners
	bool si[4] = { 0, 0, 0, 0 }; 	//Sides
	bool sh[4] = { 1, 1, 1, 1 };  	//Shadows
	infoCoSiSh(bl, co, si, sh);

	//Get total # of shadows
	int shadows = sh[0]+sh[1]+sh[2]+sh[3];
	bool special2Shadow = false;
	if( shadows==2 ) {
		if( (sh[0]&&sh[1]) || (sh[1]&&sh[3]) || (sh[2]&&sh[3]) || (sh[0]&&sh[2]) ) {
			special2Shadow = true;
		}
	}

	int osx = 0; //Overlay source X
	int osy = 0; //Overlay source Y
	if( sh[0] ) osy+=2;
	if( sh[1] ) osy+=1;
	if( sh[2] ) osx+=2;
	if( sh[3] ) osx+=1;

	drawWallOverlay( osx, osy );

	//If shadows <= 1 || special2Shadow
	//Decides locations of the corner and side texture
	if( shadows==0 || shadows==1 || special2Shadow ) {

		/* Draw corner(s) */
		osx = 0;
		osy = 4;
		if( co[0] ) osy += 2;
		if( co[1] ) osy += 1;
		if( co[2] ) osx += 2;
		if( co[3] ) osx += 1;

		drawWallOverlay( osx, osy );

		/* Draw side(s) */
		osx = 4;
		osy = 4;
		if( si[0] ) osy += 2;
		if( si[1] ) osy += 1;
		if( si[2] ) osx += 2;
		if( si[3] ) osx += 1;

		drawWallOverlay( osx, osy );
	}

}

void RegTexBuilder::drawTiles(TileDict* tileDict, std::vector<std::pair<int64_t, Tile>>& tilesToDraw, int camDirection)
{
	for(int i = 0; i<tilesToDraw.size(); i++) {
		int depth = tilesToDraw[i].first;
		Tile t = tilesToDraw[i].second;

		if(t.skipRendering) {
			continue;
		} else {
			drawTypeA(tileDict, t, camDirection, depth);
		}
	}
}

/**
    Draw a tile image with a given color; Depending on depth, shade it.
*/
void RegTexBuilder::drawTypeA(TileDict* tileDict, Tile t, int camDirection, int depth)
{
	//Get resources
	std::tuple<Texture*, int, int> src = tileDict->getTileSrcFromCamDir(t, camDirection);
	Texture* srcTex = std::get<0>(src);
	int srcX = 32*std::get<1>(src);
	int srcY = 32*std::get<2>(src);


	tex->lock(dstX, dstY, blitScale, blitScale);
	if(srcTex!=nullptr) {
		if(t.id!="null")
			tex->blit(srcTex, srcX, srcY);
	} else {
		tex->rect(dstX, dstY, blitScale, blitScale, 0, 0, 0);
	}


	
	//Depending on blitScale, draw whichever size we need to
	/*
	if( blitScale<=1 ) {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 5), srcX/32, srcY/32 );
	} else if( blitScale<=2 ) {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 4), srcX/16, srcY/16 );
	} else if( blitScale<=4 ) {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 3), srcX/8, srcY/8 );
	} else if( blitScale<=8 ) {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 2), srcX/4, srcY/4 );
	} else if( blitScale<=16 ) {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 1), srcX/2, srcY/2 );
	} else {
		tex->blit( tl->getTexture(TextureLoader::WORLD_TILE_type_a, 0), srcX, srcY );
	}*/

	
	//For a positive depth (capped at 6), make the tile look darker (deeper = darker)
	if( depth!=0 ) {
		if(depth>6) {
			depth = 6;
		}
		
		nch::Color skyColor;
		if( plnt!=nullptr ) {
			skyColor = plnt->getSkyColor();
		}
		
		tex->rect( dstX, dstY, blitScale, blitScale, skyColor.r/5, skyColor.g/5, skyColor.b/5, 40*(depth-1), SDL_BLENDMODE_BLEND);
	}
}

/*
 * Draw an overlay on top of wall tiles.
 * If applicable to the tile, some or all of the tile will be pitch black and some of the tile will be darkened.
 */
void RegTexBuilder::drawWallOverlay(int srcX, int srcY)
{
	int sx32 = (srcX*32);
	int sy32 = (srcY*32);
	
	tex->lock( dstX, dstY, blitScale, blitScale );
	tex->blit( TextureLoader::WORLD_TILE_OVERLAY_wall, sx32, sy32, 32, 32 );
}
