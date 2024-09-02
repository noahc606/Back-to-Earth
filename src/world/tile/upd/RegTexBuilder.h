#pragma once
#include <nch/cpp-utils/gfx/Color.h>
#include <sstream>
#include "Planet.h"
#include "Texture.h"
#include "TileIterator.h"

class RegTexBuilder
{
public:
	RegTexBuilder(Texture* tex, Planet* plnt, TileIterator& ti, int camDirection, int blitScale, TileType ttfc, int ttdfc);
	virtual ~RegTexBuilder();
	
	static void info(std::stringstream& ss, int& tabs, TileIterator& ti, int ttdfc);
	
	/*
		bl = blocking
		co = corners
		si = sides
		sh = shadows
	*/
	static void infoBl(TileIterator& ti, bool bl[8], int ttdfc, int camDirection);	
	static void infoBl(TileIterator& ti, bool bl[8], int camDirection);
	static void infoCoSi(bool bl[4], bool co[4], bool si[4]);
	static void infoSh(bool co[4], bool si[4], bool sh[4]);
	static void infoCoSiSh(bool bl[8], bool co[4], bool si[4], bool sh[4]);

protected:

private:
	int getDstX(TileIterator& ti);
	int getDstY(TileIterator& ti);
	
	void detailDepth0Tiles(TileIterator& ti, int camDirection); //Depth 0
	void detailDepthPTiles(TileIterator& ti); //Depth positive (>0)
	
	void drawTypeA(TileIterator& ti, int srcX, int srcY, nch::Color c);
	void drawTypeA(TileIterator& ti, TileType tt);
	void drawTypeA(TileIterator& ti, int srcX, int srcY);
	void drawWallOverlay(int srcX, int srcY);
	
	Planet* plnt = nullptr;
	Texture* tex = nullptr;
	int dstX = 0;
	int dstY = 0;
	int ttdfc = 0;	//Top Tile's Distance From Camera
	TileType ttfc;	//Top Tile From Camera
	int blitScale = -1;
};
