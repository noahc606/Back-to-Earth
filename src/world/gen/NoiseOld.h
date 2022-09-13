#pragma once
#include <vector>
#include "DebugObject.h"
#include "Texture.h"
#include "TileRegion.h"

class NoiseOld
{
public:
    typedef std::vector<std::vector<bool>> t_bools2d;
    typedef std::vector<std::vector<int>> t_ints2d;
    typedef std::vector<std::vector<float>> t_floats2d;
    typedef std::map<std::pair<long, long>, t_floats2d> t_noise2DMap;
    typedef long long t_ll;
    /**/
    NoiseOld();
    virtual ~NoiseOld();
    /**/

    void info(std::stringstream& ss, int& tabs);
    TileRegion getRegion(t_ll seed, long rX, long rY, long rZ);
    TileRegion getRegion(long rZ);


protected:

private:
    /**/
    int allocNoise2DSingle(t_ll seed, long rX, long rY);
    int allocNoise2DNear(t_ll seed, long rX, long rY);
    int freeNoise2DSingle(long rX, long rY);
    int freeNoise2DFar(long rX, long rY);
    /**/
    t_floats2d getNoise2DGrid(long rX, long rY);
    float getNoise2DVal(t_ll wy, t_ll wx);
    float getNoise2DVal(long rX, long rY, int sx, int sy);

    /**/

    t_floats2d getTerrain2D(t_ll seed, long rX, long rY, long rZ);
    void floatsRandom(t_floats2d& arr);
    void floatsGrid(t_floats2d& arr, int size);

    t_noise2DMap noise2DMap;
};
