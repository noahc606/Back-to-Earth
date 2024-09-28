#pragma once
#include <nch/cpp-utils/color.h>
#include <sstream>
#include "Planet.h"
#include "Texture.h"
#include "TileIterator.h"

class RegTexBuilder
{
public:
	RegTexBuilder(Texture* tex, Planet* plnt, TileIterator& ti, int camDirection, int blitScale, std::vector<std::pair<int64_t, TileType>>& tilesToDraw);
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
	
	void drawTiles(std::vector<std::pair<int64_t, TileType>>& tilesToDraw);
	void drawTypeA(TileType tt, int depth);
	void drawWallOverlay(int srcX, int srcY);
	
	Planet* plnt = nullptr;
	Texture* tex = nullptr;
	int dstX = 0;
	int dstY = 0;
	int blitScale = -1;
};
