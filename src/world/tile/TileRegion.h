#pragma once
#include <map>
#include <vector>
#include <string>
#include "Camera.h"
#include "DataStream.h"
#include "Tile.h"
#include "TileDict.h"
#include "TileEntity.h"
#include "TileRegionArray.h"

class TileRegion : public Loggable
{
public:
    typedef std::map<int16_t, Tile> t_palette;

    enum RegTexState {
        NONE = 0,                   //Unused state - undefined behavior
        UNGENERATED=100,            //Region has not been generated yet
        GENERATING=101,             //Region is generating
        FINISHED_GENERATING=102,    //Region just finished generating
        SHOULD_UPDATE = 103,        //Region needs an update for a 32x32 tile draw
        UPDATED = 104,              //Region has been updated - no need for a 32x32 tile draw
    };

    /* Init */
    TileRegion(TileDict* tileDict, size_t bitsize);
    TileRegion(TileDict* tileDict);
    virtual ~TileRegion();

    //TileRegion info
    void putPaletteInfo(std::stringstream& ss, int& tabs, bool natural);
	void putInfo(std::stringstream& ss, int& tabs);
	void putInfo(std::stringstream& ss, int& tabs, int subX, int subY, int subZ);
    std::string getInfo(int subX, int subY, int subZ);
	bool assertDefaultTileExists(t_palette& pal);
    //Palette info
	uint16_t getPaletteSize();  uint16_t getPaletteSizeNatural();   uint16_t getPaletteSizeArtificial();
	static int getPaletteSizeBucket(int size);                      int getArtificialPaletteSizeBucket();
	Tile getPaletteElement(int16_t key);
	//Get tile info
	int16_t getTileKey(int x, int y, int z);
	Tile getTile(int x, int y, int z);
    std::vector<nch::Vec3<int64_t>> getTileEntityLocs();
	//Get Region info
	bool beenModifiedSinceLoad();
	int getRegTexState();
	
	/* Mutators */
	//Add to palette
    int16_t addToPalette(Tile tile, t_palette& pal, bool natural);
    int16_t addToPalette(Tile tile, t_palette& pal);
    int16_t addToPalette(Tile tile, bool natural);
    int16_t addToPalette(Tile tile);
    int16_t addToPaletteFast(Tile tile, bool natural);
    int16_t addToPaletteFast(Tile tile);
    //Set tiles
    void setTile         (int x, int y, int z,                       Tile tile);
    void setTile         (int x, int y, int z,                       int16_t paletteIndex);
    void setTiles        (int x1,int y1,int z1,int x2,int y2,int z2, Tile tile);
    void setTiles        (int x1,int y1,int z1,int x2,int y2,int z2, int16_t paletteIndex);
	//Resetting
	void resetPalette();
	void resetArtificialPalette();
	void resetTiles();
    //Set region state/priority
    void setRegTexState(int p_rts);
    void resetRegTexState();

    /* SPECIAL - Level saving */
    void compress();
    void dumpPaletteData(DataStream& ds, uint8_t dataBitsPerTile);
    void dumpTileData(DataStream& ds, uint8_t dataBitsPerTile);
    void save(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ, bool compress);
    void save(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ);
	void load(std::string saveGameName, int64_t rX, int64_t rY, int64_t rZ);

    TileEntity* getTileEntity(nch::Vec3<int64_t> subpos);
    void setTileEntity(nch::Vec3<int64_t> subpos, TileEntity* te);

protected:

private:
    TileDict* tileDict;
    t_palette palette;
    TileRegionArray* tiles = nullptr;
    std::map<std::tuple<int64_t, int64_t, int64_t>, TileEntity*> tileEntities;

	bool modifiedSinceLoad = false;

    int regTexState = UNGENERATED;

};
