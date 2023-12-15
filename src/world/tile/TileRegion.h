#pragma once
#include <map>
#include <vector>
#include <string>
#include "Camera.h"
#include "DataStream.h"
#include "TileType.h"

class TileRegion : public Loggable
{
public:
    typedef std::map<int16_t, TileType> t_palette;

    /**/
    TileRegion();
    virtual ~TileRegion();
    /**/
    void putPaletteInfo(std::stringstream& ss, int& tabs, bool natural);
	void putInfo(std::stringstream& ss, int& tabs);
	void putInfo(std::stringstream& ss, int& tabs, int subX, int subY, int subZ);
    std::string getInfo(int subX, int subY, int subZ);
	void printInfoTileIndices();
	/**/
    //Get palette info
	uint16_t getPaletteSize();
	uint16_t getPaletteSizeNatural();
	uint16_t getArtificialPaletteSize();
	static int getPaletteSizeBucket(int size);
	int getArtificialPaletteSizeBucket();
	TileType getPaletteElement(int16_t key);
	//Get tile info
	int16_t getTileKey(int x, int y, int z);
	TileType getTile(int x, int y, int z);
	//Get Region info
	int getRegTexState();
	int getRegTexPriority();
	
	/**/
	//Check palette
	bool assertDefaultTileExists(t_palette& pal);
	//Add to palette
    int16_t addToPalette(TileType tile, t_palette& pal, bool natural);
    int16_t addToPalette(TileType tile, t_palette& pal);
    int16_t addToPalette(TileType tile, bool natural);
    int16_t addToPalette(TileType tile);
    int16_t addToPaletteFast(TileType tile, bool natural);
    int16_t addToPaletteFast(TileType tile);
    	
	/**/
    //Set tiles
    void setTile         (int x, int y, int z,                       TileType tile);
    void setTile         (int x, int y, int z,                       int16_t paletteIndex);
    void setTiles        (int x1,int y1,int z1,int x2,int y2,int z2, TileType tile);
    void setTiles        (int x1,int y1,int z1,int x2,int y2,int z2, int16_t paletteIndex);
	//Resetting
	void resetPalette();
	void resetArtificialPalette();
	void resetTiles();
    //Set region state/priority
    void setRegTexState(int p_rts);
    void resetRegTexState();
    void setRegTexPriority(int p_rtp);
	//Used in level saving
    void compress();
    void dumpPaletteData(DataStream& ds, uint8_t dataBitsPerTile);
    void dumpTileData(DataStream& ds, uint8_t dataBitsPerTile);
    void save(FileHandler* fh, std::string saveGameName, long rX, long rY, long rZ, bool compress);
    void save(FileHandler* fh, std::string saveGameName, long rX, long rY, long rZ);
	void load(FileHandler* fh, std::string saveGameName, long rX, long rY, long rZ);

    enum RegTexState {
        NONE = 0,
        UNGENERATED=100,
        GENERATING=101,
        FINISHED_GENERATING=102,
        SHOULD_UPDATE = 103,
        UPDATED = 104,
    };

protected:

private:
    t_palette palette;
    int16_t tiles[32][32][32];

    int regTexState = UNGENERATED;
    int regTexPriority = -1;

};
