#include "NoiseOld.h"
#include <math.h>
#include "Texture.h"
#include "TileMap.h"
#include "Timer.h"

NoiseOld::NoiseOld()
{
    //ctor
}

NoiseOld::~NoiseOld()
{
    /**
        void TileMap::getRegSubPos(long& x, long& y, long& z) { x = getRegSubPos(x); y = getRegSubPos(y); z = getRegSubPos(z); }
        long TileMap::getRegSubPos(long c) { c %= 32; if( c<0 ) c+=32; return c; }

    */



    //dtor
}

void NoiseOld::info(std::stringstream& ss, int& tabs)
{
    DebugScreen::indentLine(ss, tabs);
    ss << "noise2DMap.size()=" << noise2DMap.size() << "; ";
    DebugScreen::newLine(ss);
}

TileRegion NoiseOld::getRegion(t_ll seed, long rX, long rY, long rZ)
{
    TileRegion tr;

    TileType tt; tt.init();

    tt.setVisionBlocking(false);
    tt.setTextureXY(0, 0);
    tt.setSolid(false);
    int air = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(3, 5);
    tt.setRGB(51, 255, 187);
    tt.setSolid(true);
    int topsoil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(3, 1);
    tt.setRGB(128, 50, 0);
    tt.setSolid(true);
    int soil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(2, 1);
    tt.setRGB(240, 240, 240);
    tt.setSolid(true);
    int rock = tr.addToPaletteFast(tt);

    t_ll wz = 0;

    t_floats2d terrain = getTerrain2D(seed, rX, rY, rZ);

    for( int sx = 0; sx<32; sx++ ) {
        for( int sy = 0; sy<32; sy++ ) {
            for( int sz = 0; sz<32; sz++ ) {
                wz = 32*rZ+sz;
                int height = wz-terrain[sx][sy];

                if( height<=-5 ) {
                    tr.setTile(sx, sy, sz, rock);
                } else
                if( height<=-1 ) {
                    tr.setTile(sx, sy, sz, soil);
                } else
                if( height<=0 ) {
                    tr.setTile(sx, sy, sz, topsoil);
                } else
                if( wz<=10000 )
                {
                    tr.setTile(sx, sy, sz, air);
                }
            }
        }
    }

    return tr;
}

TileRegion NoiseOld::getRegion(long rZ)
{
    TileRegion tr;

    TileType tt; tt.init();

    tt.setVisionBlocking(false);
    tt.setTextureXY(0, 0);
    tt.setSolid(false);
    int air = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(3, 5);
    tt.setRGB(51, 255, 187);
    tt.setSolid(true);
    int topsoil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(3, 1);
    tt.setRGB(128, 50, 0);
    tt.setSolid(true);
    int soil = tr.addToPaletteFast(tt);

    tt.setVisionBlocking(true);
    tt.setTextureXY(2, 1);
    tt.setRGB(240, 240, 240);
    tt.setSolid(true);
    int rock = tr.addToPaletteFast(tt);

    if( rZ!=-1 ) {
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                for( int sz = 0; sz<32; sz++ ) {
                    tr.setTile(sx, sy, sz, air);
                }
            }
        }
    }



    if( rZ==-1 ) {
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                for( int sz = 0; sz<32; sz++ ) {
                    tr.setTile(sx, sy, sz, rock);
                }
            }
        }
    }
    /*
    if( rZ<-1 ) {
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                for( int sz = 0; sz<32; sz++ ) {
                    tr.setTile(sx, sy, sz, air);
                }
            }
        }
    } else
    if( rZ==-1 ) {
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {

                for( int sz = 5; sz<31; sz++ )
                    tr.setTile(sx, sy, sz, rock);

                for( int sz = 1; sz<5; sz++ )
                    tr.setTile(sx, sy, sz, soil);

                for( int sz = 0; sz<1; sz++ )
                    tr.setTile(sx, sy, sz, topsoil);


            }
        }
    } else
    if( rZ>-1 ) {
        for( int sx = 0; sx<32; sx++ ) {
            for( int sy = 0; sy<32; sy++ ) {
                for( int sz = 0; sz<32; sz++ ) {
                    tr.setTile(sx, sy, sz, rock);
                }
            }
        }
    }
    */

    return tr;
}

/**/

int NoiseOld::allocNoise2DSingle(t_ll seed, long rX, long rY)
{
    //Set seed of rng based on seed, + region (x, y)
    int rZ = 0;
    std::srand( (1218678311*seed)+(1298951191*rX)+(1765186373*rY)+(2102166179*rZ) );

    //Generate a 32x32 grid of random floats between 0 and 1
    t_floats2d noise;
    floatsGrid(noise, 32);
    floatsRandom(noise);

    //Try to insert 32x32 noise grid into the map
    t_noise2DMap::iterator itrN2M = noise2DMap.find(std::make_pair(rX, rY));
    if( itrN2M==noise2DMap.end() ) {
        noise2DMap.insert( std::make_pair(std::make_pair(rX, rY), noise) );
        return 1;
    }
    return -1;
}

int NoiseOld::allocNoise2DNear(t_ll seed, long rX, long rY)
{
    int result = 0;

    for(int irX = rX-1; irX<=rX+1; irX++) {
        for(int irY = rY-1; irY<=rY+1; irY++) {
            if( allocNoise2DSingle(seed, irX, irY)>0 ) {
                result++;
            }
        }
    }

    //Return result
    if(result==0)
        return -1;
    return result;
}

int NoiseOld::freeNoise2DSingle(long rX, long rY)
{
    t_noise2DMap::iterator itrN2M = noise2DMap.find(std::make_pair(rX, rY));
    if( itrN2M!=noise2DMap.end() ) {
        noise2DMap.erase(itrN2M);
        return 1;
    }
    return -1;
}

int NoiseOld::freeNoise2DFar(long rX, long rY)
{
    int result = 0;

    for( t_noise2DMap::iterator itrN2M = noise2DMap.begin(); itrN2M!=noise2DMap.end(); itrN2M++ ) {
        long irX = itrN2M->first.first;
        long irY = itrN2M->first.second;
        if( irX>rX+1 || irX<rX-1 || irY>rY+1 || irY<rY-1 ) {
            freeNoise2DSingle(irX, irY);
            result++;
        }
    }

    //Return result
    if(result==0)
        return -1;
    return result;
}

/**/

NoiseOld::t_floats2d NoiseOld::getNoise2DGrid(long rX, long rY)
{
    t_noise2DMap::iterator itrN2M = noise2DMap.find(std::make_pair(rX, rY));
    if( itrN2M!=noise2DMap.end() ) {
        return itrN2M->second;
    } else {
        t_floats2d result;
        floatsGrid(result, 32);
        return result;
    }
}

float NoiseOld::getNoise2DVal(t_ll wx, t_ll wy)
{
    int rX = TileMap::getRegRXYZ(wx);
    int rY = TileMap::getRegRXYZ(wy);
    int sx = TileMap::getRegSubPos(wx);
    int sy = TileMap::getRegSubPos(wy);

    t_noise2DMap::iterator itrN2M = noise2DMap.find(std::make_pair(rX, rY));
    if( itrN2M!=noise2DMap.end() ) {
        return (itrN2M->second)[sx][sy];
    } else {
        return 0;
    }
}

float NoiseOld::getNoise2DVal(long rX, long rY, int sx, int sy)
{
    if( sx<0 ) {
        rX--;
        sx += 32;
    } else
    if( sx>31) {
        rX++;
        sx -= 32;
    }

    if( sy<0 ) {
        rY--;
        sy += 32;
    } else
    if( sy>31) {
        rY++;
        sy -= 32;
    }

    return getNoise2DGrid(rX, rY)[sx][sy];
}

NoiseOld::t_floats2d NoiseOld::getTerrain2D(t_ll seed, long rX, long rY, long rZ)
{
    /** Prep */
    //Generate noise
    //freeNoise2DFar(rX, rY);         //Free far away noise that won't be needed
    allocNoise2DNear(seed, rX, rY); //Allocate nearby noise that will be needed
    //std::cout << noise2DMap.size() << "\n";

    //Generate a 32x32 grid of floats = 0.0;
    t_floats2d terrain;
    floatsGrid(terrain, 32);

    /** Generate smoothed-out terrain */
    //Parameters
    float minZ = 0;         //Z height value to start at
    float zFactor = 8.0;   //Vertical (Z) stretch
    int xyFactor = 2;  //Horizontal (X, Y) stretch

    t_floats2d noise = getNoise2DGrid(rX, rY);

    //Add gradient in between all points in 'terrain'.
    for( int sx = 0; sx<32; sx++ ) {
        for( int sy = 0; sy<32; sy++ ) {
            float dividend = 0;
            float divisor = 0;

            for( t_ll isx = sx-xyFactor; isx<=sx+xyFactor; isx++ ) {
                for( t_ll isy = sy-xyFactor; isy<=sy+xyFactor; isy++ ) {
                    dividend += getNoise2DVal(32*rX+isx, 32*rY+isy);
                    divisor += 1.0;
                }
            }

            terrain[sx][sy] = dividend*zFactor/divisor+minZ;
            //terrain[x][y] = 55;
        }
    }


    return terrain;
}

void NoiseOld::floatsRandom(t_floats2d& arr)
{
    for( int x = 0; x<(int)arr.size(); x++ ) {
        for( int y = 0; y<(int)arr[0].size(); y++ ) {
             arr[x][y] = (float)std::rand()/(RAND_MAX);
        }
    }
}

void NoiseOld::floatsGrid(t_floats2d& arr, int size)
{
    for( int x = 0; x<size; x++ ) {
        std::vector<float> v;
        for( int y = 0; y<size; y++ ) {
            v.push_back(0.0);
        }
        arr.push_back(v);
    }
}

