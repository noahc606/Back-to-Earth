#pragma once
#include <map>
#include <set>
#include <string>
#include <tuple>
#include "Box3D.h"
#include "BTEObject.h"
#include "FileHandler.h"
#include "Player.h"
#include "Texture.h"
#include "TileRegion.h"

class TileMap : public BTEObject
{
public:
    /**/
    typedef long long                       t_ll;
    typedef int                             t_regState;
    typedef std::tuple<char, char>          t_charP;
    typedef std::tuple<long, long>          t_v2d;
    typedef std::tuple<long, long, long>    t_v3d;
    typedef std::set<std::pair<int, int>>   t_updates;

    typedef std::map<t_v3d, TileRegion>     t_regionMap;
    typedef std::map<t_v3d, t_updates>      t_updatesMap;
    /**/
    void destroy();
    /**/
    void info(std::stringstream& ss, int& tabs, t_ll mouseX, t_ll mouseY, t_ll selZ);
    t_regionMap* getRegionMap();
    t_updatesMap* getUpdatesMap();

    TileType getTile(t_ll x, t_ll y, t_ll z);
    TileRegion* getRegByXYZ (t_ll x, t_ll y, t_ll z);
    TileRegion* getRegByRXYZ(long rX, long rY, long rZ);
    t_updates* getRTUsByRXYz(long rX, long rY, long z);

    static void getRegSubPos(t_ll& x, t_ll& y, t_ll& z);
    static void getRegSubPos(long& x, long& y, long& z);
    static long getRegSubPos(t_ll c);
    static void getRegRXYZ  (t_ll& x, t_ll& y, t_ll& z);
    static void getRegRXYZ  (long& x, long& y, long& z);
    static long getRegRXYZ  (t_ll c);

    bool collides( Box3D &b );

    /** TileMap manipulation */
    //Set tile
    int setTile(t_ll x, t_ll y, t_ll z, TileType tt);
    //Load regions
    int loadRegion(long rX, long rY, long rZ);
    int saveRegion(FileHandler* fileHandler, long rX, long rY, long rZ);
    int unloadRegion(FileHandler* fileHandler, long rX, long rY, long rZ);
    //Add/Remove updates
    int addTileUpdate(t_ll x, t_ll y, t_ll layer);
    int addTileUpdates(t_ll x, t_ll y, t_ll layer);
    int addTileUpdates(t_ll x0, t_ll y0, t_ll layer0, t_ll x1, t_ll y1, t_ll layer1);
    int addRegionUpdate(long rX, long rY, t_ll layer);
    int stopRegionUpdate(long rX, long rY, t_ll layer);
    int stopAllUpdates();

protected:

private:
    //Maps
    t_regionMap regionMap;
    t_updatesMap updatesMap;

    std::vector<Player>* players;
};
