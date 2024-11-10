#include "Minimap.h"
#include "Log.h"
#include "RegTexInfo.h"
#include "TextOld.h"
#include "TileIterator.h"

void Minimap::init(SDLHandler* sh, Camera* cam, TileMap* tm)
{
    sdlHandler = sh;
    camera = cam;
    tileMap = tm;

	terrain.init(sdlHandler, 128, 128,  2);
	terrain.lock();
	for(int x = 0; x<128; x++)
	for(int y = 0; y<128; y++) {
		terrain.pixel(x, y, 0, 0, 0);
	}
	terrain.unlock();
}

void Minimap::draw()
{
    //Run whenever there should be a terrain part drawn
    if(SDL_GetTicks64()>lastTerrainPartDrawMS+updateTimeMS/numTerrainPartDraws) {
        lastTerrainPartDrawMS = SDL_GetTicks64();
        
        int stw = 127;
        int pds = currTerrainPartDraw*stw/numTerrainPartDraws; 	//(P)artial (D)raw (S)tart
        int pde = pds+stw/numTerrainPartDraws;           		//(P)artial (D)raw (E)nd

        if(currTerrainPartDraw!=numTerrainPartDraws-1) {
            updateTerrainPartial(pds, pde);
        } else {
            updateTerrainPartial(pds, 127);
        }

		//Increment currTerrainPartDraw or reset it if needed
        currTerrainPartDraw++;
		if(currTerrainPartDraw==numTerrainPartDraws) {

			camLX = camera->getLayer(0);
			camLY = camera->getLayer(1);
			camLZ = camera->getLayer(2);
			currTerrainPartDraw = 0;

			//Update terrain appearance
			terrain.unlock();
			terrain.lock();
		}
    }

	terrain.setDrawPos( sdlHandler->getWidth()-terrain.getTexWidth()*2, 0 );
	terrain.draw();

	//helper text
	
	std::stringstream desc;
	desc << "Camera facing " << camera->getDirectionString() << "\n" << "Press [TAB] to change";
	TextOld::draw(sdlHandler, desc.str(), sdlHandler->getWidth()-terrain.getTexWidth()*2, terrain.getTexHeight()*2, 2);
}

/*
	Update the minimap's terrain with new pixels within the specified area.

	Params:
	- startX: number from 0 to 127 representing the starting pixel column
	- endX: number from 0 to 127 ... ending pixel column
*/
void Minimap::updateTerrainPartial(int64_t startX, int64_t endX)
{
    if(startX<0) startX = 0;
    if(startX>127) startX = 127;
    if(endX<0) endX = 0;
    if(endX>127) endX = 127;
    
	//Create tileiterator
	TileIterator ti(tileMap);
	ti.setTrackerMode(TileIterator::RegionTrackingModes::FULL);

	//Set bounds of tileiterator
	int rad = 64;
	int64_t cMinX = camLX+startX-64;	int64_t cMaxX = camLX+endX-64;
	int64_t cMinY = camLY-rad;			int64_t cMaxY = camLY+rad;
	int64_t tEast = cMinX - (camLX-64);
	int status = ti.setBounds(
		cMinX, cMinY, camLZ,
		cMaxX, cMaxY, camLZ
	);
	if(status==-1) {
		return;
	}

	//Loop through every region within bounds
	for(TileRegion* tr = ti.peekRegion(); !ti.atEnd(); ti.nextRegion()) {
		if(tr==nullptr) {
			continue;
		}

		//Loop through 32x32x1 area, using tracker to get the upmost tile at each pos
		for(int sx = ti.gbs(0); sx<=ti.ges(0); sx++) {
			for(int sy = ti.gbs(1); sy<=ti.ges(1); sy++) {
				ti.setTrackerSub(sx, sy, TileMap::getRegSubPos(camLZ));

				auto ctt = RegTexInfo::camTopVisionBlockingTile(ti, Camera::DOWN, 16);
				int64_t depth = ctt.first;
				nch::Color ttRGB = ctt.second.mapColor;

				//Calculate darkFactor from depth
				double darkFactor = 1.0 + (double)(depth)/2.;

				//Fully darken a depth-zero tile if:
				// 1) it is opaque (from above), AND
				// 2) blocked on all 4 sides and 4 corners with opaque tiles
				if(depth==0) {
					if(
						ctt.second.isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(-1, -1, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(-1, 00, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(-1, 01, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(00, -1, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(00, 01, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(01, -1, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(01, 00, 0).isVisionBlocking(Camera::DOWN) &&
						ti.peekTrackedTile(01, 01, 0).isVisionBlocking(Camera::DOWN)
					) {
						darkFactor = 1000.0;
					}
				}

				terrain.pixel (
					ti.getItrReg(0)*32-cMinX+sx+tEast, ti.getItrReg(1)*32-cMinY+sy, ttRGB.r/darkFactor, ttRGB.g/darkFactor, ttRGB.b/darkFactor
				);
			}
		}
	}
}