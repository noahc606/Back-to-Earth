#include "RegTexProcessor.h"
#include "RegTexBuilder.h"
#include "RegTexUpdater.h"
#include "Log.h"

void RegTexProcessor::init(SDLHandler* sh, Canvas* cs, TileMapUpdater* tmu)
{
	sdlHandler = sh;
	csTileMap = cs;
	cam = csTileMap->getCamera();
	tileMapUpdater = tmu;
	tileMap = tmu->getTileMap();
}

/*
 * Centered at region (camRX, camRY, camRZ), process a square of regions perpendicular to the camera direction.
 * The loop starts at the center region and works its way outward, depending on loadRadiusH (horizontal load radius).
 */
void RegTexProcessor::processRegions(Camera* cam, int loadRadiusH, int rtUpdatesToDo)
{
	int64_t camRX = cam->getRX();
	int64_t camRY = cam->getRY();
	int64_t camRZ = cam->getRZ();
	int screenSWR = cam->getScreenSemiWidthReg();
	int screenSHR = cam->getScreenSemiHeightReg();

	TileIterator ti(tileMap);
	ti.logWarnings(false);
	ti.setTrackerMode(ti.FULL);

	for( int outline = 0; outline<=loadRadiusH; outline++ ) {
		//Iterate dSRX (delta SRX) from -outline to +outline
		for( int dSRX = -outline; dSRX<=outline; dSRX++ ) {
			int dSRY = outline*2;                                //Set dSRY (delta SRY)
			if( abs(dSRX)==outline ) { dSRY = 1; }
			
			int64_t sRX = dSRX;
			int64_t sRY = -outline;
			int64_t sRZ = 0;

			//Get sRX and sRY depending on camera axis.
			sRX += camRX;	// X: (camRY, camRZ, camRX)
			sRY += camRY;	// Y: (camRX, camRZ, camRY)
			sRZ += camRZ;	// Z: (camRX, camRY, camRZ)
			
			//Get sRY: Iterate from camRY-outline to camRY+outline based on dSRY
			for( ; sRY<=camRY+outline; sRY += dSRY ) {
				//If this region at (sRX, sRY) is onscreen (vertical coordinate doesn't matter)
				if( RegTexUpdater::isRegOnScreen(sdlHandler, cam, camRX, camRY, camRZ) ) {
					//Update regtex at this regionlayer
					processRegionLayer(ti, sRX, sRY, sRZ, cam, rtUpdatesToDo);

					TileRegion* tr = tileMap->getRegByRXYZ(sRX, sRY, sRZ);
					if(tr!=nullptr) {
						int state = tr->getRegTexState();
						if(state==TileRegion::RegTexState::SHOULD_UPDATE) {
							//tr->setRegTexState(TileRegion::RegTexState::UPDATED);
							//tileMapUpdater->addRegionUpdate(sRX, sRY);
						}
					}


					if((sRX+sRY)%2) {
						//colorFillRegionArea(sRX, sRY, 0, 255, 0);
					} else {
						//colorFillRegionArea(sRX, sRY, 0, 0, 255);
					}
				}
			}
		}
	}
}


/*
 * Draw the camera's view of a specifc region. Called from processRegions().
 */
void RegTexProcessor::processRegionLayer(TileIterator& ti, int64_t sRX, int64_t sRY, int64_t sRZ, Camera* cam, int rtUpdatesToDo)
{
	int numUpdates = 0;
    int sl = TileMap::getRegSubPos(cam->getLayer());

	/** Get 'ru' (region updates collection) */
	auto rus = tileMapUpdater->getRegUpdates();
	auto ruXY = rus->find(std::make_pair(sRX, sRY));
	//Build a region marked for update
	if(ruXY!=rus->end()) {
		rus->erase(ruXY);
		buildRegionArea(sRX, sRY);
	}

    /** Get 'tu' (tile updates collection) located within this region */
    auto tu = tileMapUpdater->getTUsByRXY(sRX, sRY);
    //Delete empty or null update collections
    if(tu==nullptr || tu->size()==0) {
		tileMapUpdater->getTileUpdates()->erase(std::make_pair(sRX, sRY));
        return;
    }

    /** Get 'tex' located at this region from csTileMap */
    Texture* tex = csTileMap->getTex(sRX, sRY);
    //If 'tex' is null
    if(tex==nullptr) return;

    /** Iterate through updates */
    //Set bounds of the TileIterator to this region
	switch( cam->getAxis() ) {
		case Camera::X: {
			ti.setBoundsByRXYZ(sRY, sRZ, sRX);
		} break;
		case Camera::Y: {
			ti.setBoundsByRXYZ(sRX, sRZ, sRY);
		} break;
		case Camera::Z: {
			ti.setBoundsByRXYZ(sRX, sRY, sRZ);
		} break;
	}
	
	//Go through each individual update in 'rtu' and perform the regTexUpdate().
	for( TileMapUpdater::t_updates::iterator itr = tu->begin(); itr!=tu->end(); itr=tu->begin() ) {
		//Increment updates.
		numUpdates++;
		//Set TileIterator to the position of the tu
		switch(cam->getAxis()) {
			case Camera::X: {
				ti.setTrackerSub( sl, itr->first, itr->second );
			} break;
			case Camera::Y: {
				ti.setTrackerSub( itr->first, sl, itr->second );
			} break;
			case Camera::Z: {
				ti.setTrackerSub( itr->first, itr->second, sl );
			} break;
		}
		
		//Perform the region texture update.
		processTileArea(ti, tex, cam->getBlitScale());
		//Erase this rtu
		tu->erase(itr);
		//If max number of updates has been reached, stop the method
		if(numUpdates>=rtUpdatesToDo) return;
	}
}

void RegTexProcessor::processTileArea(TileIterator& ti, Texture* tex, int blitScale)
{
	if( cam->getDirection()==Camera::WEST ) {
		std::cout << ti.gtp(0) << ", " << ti.gtp(1) << ", " << ti.gtp(2) << "\n";
	}
	
	//If tex is nullptr for some reason, stop method
	if( tex==nullptr ) return;

	//Get data of the top tile from the camera
	auto tttData = RegTexUpdater::camTrackedTile(ti, cam->getDirection());  //Pair of an int64_t object and a TileType object
	int64_t dZ = std::get<0>(tttData);        								//Relative height of top tile ( topmost==0, one below==-1, etc. )
	TileType topTileFromCam = std::get<1>(tttData); 						//TileType of top tile from camera

	//Create RegTexBuilder
	RegTexBuilder rtb(tex, ti, cam->getDirection(), blitScale, topTileFromCam, dZ);
}

/*
 * Draw an entire region, given the region's horizontal and vertical coordinates from the camera/user's perspective.
 * Instead of placing a bunch of updates, we use this function when drawing a new onscreen region.
 */
void RegTexProcessor::buildRegionArea(int64_t sRX, int64_t sRY)
{
	//Canvas drawing to screen
	int64_t rX = cam->getRX();
	int64_t rY = cam->getRY();
	int64_t rZ = cam->getRZ();
	int64_t cL = cam->getLayer();
	switch( cam->getAxis() ) {
		case Camera::X: rY = sRX; rZ = sRY; break;
		case Camera::Y: rX = sRX; rZ = sRY; break;
		default:		rX = sRX; rY = sRY; break;
	}

	//Get texture
	Texture* tex = csTileMap->getTex(sRX, sRY);
	if(tex==nullptr) {
		return;
	}
	
	if( tileMap==nullptr ) {
		Log::throwException();
		return;
	}
	TileIterator ti(tileMap);
	ti.setTrackerMode(TileIterator::FULL);
	ti.setBoundsByRXYZ(rX, rY, rZ);

	for(int64_t scx = 0; scx<32; scx++) {
		for(int64_t scy = 0; scy<32; scy++) {
			switch(cam->getAxis()) {
				case Camera::X: ti.setTrackerSub( TileMap::getRegSubPos(cL), scx, scy ); break;
				case Camera::Y: ti.setTrackerSub( scx, TileMap::getRegSubPos(cL), scy ); break;
				default: ti.setTrackerSub( scx, scy, TileMap::getRegSubPos(cL) ); break;
			}
			
			auto ctt = RegTexUpdater::camTrackedTile(ti, cam->getDirection());
			int64_t depth = ctt.first;
			TileType tt = ctt.second;
			int blitScale = 32.0*Canvas::getTexLODBasedOnZoom(cam->getZoom());
			RegTexBuilder rtb(tex, ti, cam->getDirection(), blitScale, tt, depth);
		}
	}
}

/*
 * Debugging tool: useful for seeing where regions of interest are on the current screen.
 */
void RegTexProcessor::colorFillRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	/** Get 'tex' located at this region from csTileMap */
	Texture* tex = csTileMap->getTex(sRX, sRY);
	if(tex==nullptr) {
		return;
	}

	SDL_BlendMode oldBlendMode = tex->getBlendMode();

	// Fill the entire texture (AKA region area) with a single color
	int tw = tex->getTexWidth();
	int th = tex->getTexHeight();
	tex->setBlendMode(SDL_BLENDMODE_ADD);
	tex->rect(0, 0, tw, th, r, g, b, a);
	tex->setBlendMode(oldBlendMode);
}

void RegTexProcessor::colorFillRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY, uint8_t r, uint8_t g, uint8_t b) { colorFillRegionArea(csTileMap, sRX, sRY, r, g, b, 255); }

/*
 *  Blacks out regions. Used ingame for regions that aren't loaded or can't be seen anymore.
 */
void RegTexProcessor::blackOutRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY) { colorFillRegionArea(csTileMap, sRX, sRY, 0, 0, 0); }
