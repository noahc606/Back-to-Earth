#include "RegTexProcessor.h"
#include <nch/cpp-utils/log.h>
#include "RegTexBuilder.h"
#include "RegTexInfo.h"

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
void RegTexProcessor::processRegions(Camera* cam, int loadDist, int rtUpdatesToDo)
{
	int64_t camCsRX = cam->getCsRX();
	int64_t camCsRY = cam->getCsRY();
	int64_t camCsRZ = cam->getCsRZ();
	int screenSWR = cam->getScreenSemiWidthReg();
	int screenSHR = cam->getScreenSemiHeightReg();

	TileIterator ti(tileMap);
	ti.logWarnings(false);
	ti.setTrackerMode(ti.FULL);

	for( int outline = 0; outline<=loadDist; outline++ ) {
		for( int dCsRX = -outline; dCsRX<=outline; dCsRX++ ) {		//Iterate thru diff values of dCsRX (delta canvas region X)
			int dSRY = outline*2;                                	//Set dSRY (delta csRY)
			if( abs(dCsRX)==outline ) { dSRY = 1; }
			
			//Get csRX, csRY, and csRZ
			int64_t csRX = dCsRX+camCsRX;
			int64_t csRY = -outline+camCsRY;
			int64_t csRZ = camCsRZ;

			//Get sRY: Iterate from camRY-outline to camRY+outline based on dSRY
			for( ; csRY<=camCsRY+outline; csRY += dSRY ) {
				//If this region at (csRX, csRY) is onscreen (vertical coordinate doesn't matter)
				if( RegTexInfo::isCsRegOnScreen(cam, camCsRX, camCsRY) ) {
					//Update regtex at this regionlayer
					processRegionLayer(ti, csRX, csRY, csRZ, cam, rtUpdatesToDo);
				}
			}
		}
	}
}


/*
 * Draw the camera's view of a specifc region. Called from processRegions().
 */
void RegTexProcessor::processRegionLayer(TileIterator& ti, int64_t csRX, int64_t csRY, int64_t csRZ, Camera* cam, int rtUpdatesToDo)
{
	int numUpdates = 0;
    int sl = TileMap::getRegSubPos(cam->getLayer());

	/** Get 'ru' (region updates collection) */
	auto rus = tileMapUpdater->getRegUpdates();
	auto ruXY = rus->find(std::make_pair(csRX, csRY));
	//Build a region marked for update
	if(ruXY!=rus->end()) {
		rus->erase(ruXY);
		buildRegionArea(csRX, csRY);
	}

    /** Get 'tu' (tile updates collection) located within this region */
    auto tu = tileMapUpdater->getTUsByRXY(csRX, csRY);
    //Delete empty or null update collections
    if(tu==nullptr || tu->size()==0) {
		tileMapUpdater->getTileUpdates()->erase(std::make_pair(csRX, csRY));
        return;
    }

    /** Get 'tex' located at this region from csTileMap */
    Texture* tex = csTileMap->getTex(csRX, csRY);
    //If 'tex' is null
    if(tex==nullptr) return;

    /** Iterate through updates */
    //Set bounds of the TileIterator to this region
	switch( cam->getAxis() ) {
		case Camera::X: {
			ti.setBoundsByRXYZ(csRZ, csRX, csRY);
		} break;
		case Camera::Y: {
			ti.setBoundsByRXYZ(csRX, csRZ, csRY);
		} break;
		case Camera::Z: {
			ti.setBoundsByRXYZ(csRX, csRY, csRZ);
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
	//If tex is nullptr for some reason, stop method
	if( tex==nullptr ) return;

	//Get data of the top tile from the camera
	auto cttdh = RegTexInfo::camTilesToDrawHere(ti, cam->getDirection(), 32);
	//cttdh is a vector of pairs(int64_t, TileType).
	//int64_t: Relative depth of top tile ( none found==-1, topmost==0, one below==1, etc. )
	//TileType: tile in list which should be drawn


	//Create RegTexBuilder
	RegTexBuilder rtb(tex, tileMap->getPlanet(), ti, cam->getDirection(), blitScale, cttdh);
}

/*
 * Draw an entire region, given the region's horizontal and vertical coordinates from the camera/user's perspective.
 * Instead of placing a bunch of updates, we use this function when drawing a new onscreen region.
 */
void RegTexProcessor::buildRegionArea(int64_t csRX, int64_t csRY)
{
	int64_t cL = cam->getLayer();

	//Get texture
	Texture* tex = csTileMap->getTex(csRX, csRY);
	if(tex==nullptr) {
		std::cout << "Couldn't find " << csRX << ", " << csRY << "...\n";
		return;
	}
	
	if( tileMap==nullptr ) {
		nch::Log::throwException();
		return;
	}
	TileIterator ti(tileMap);
	ti.setTrackerMode(TileIterator::FULL);
	switch(cam->getAxis()) {
		case cam->X: ti.setBoundsByRXYZ(cam->getCsRZ(), csRX, csRY); break;
		case cam->Y: ti.setBoundsByRXYZ(csRX, cam->getCsRZ(), csRY); break;
		case cam->Z: ti.setBoundsByRXYZ(csRX, csRY, cam->getCsRZ()); break;
	}

	for(int64_t scx = 0; scx<32; scx++) {
		for(int64_t scy = 0; scy<32; scy++) {
			switch(cam->getAxis()) {
				case Camera::X: ti.setTrackerSub( TileMap::getRegSubPos(cL), scx, scy ); break;
				case Camera::Y: ti.setTrackerSub( scx, TileMap::getRegSubPos(cL), scy ); break;
				default: ti.setTrackerSub( scx, scy, TileMap::getRegSubPos(cL) ); break;
			}
			
			processTileArea(ti, tex, 32.0*Canvas::getTexLODBasedOnZoom(cam->getZoom()));
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
 *  Clears a texture region. Used ingame for regions that aren't loaded or can't be seen anymore.
 */
void RegTexProcessor::clearRegionArea(Canvas* csTileMap, int64_t sRX, int64_t sRY) { colorFillRegionArea(csTileMap, sRX, sRY, 0, 0, 0, 0); }
