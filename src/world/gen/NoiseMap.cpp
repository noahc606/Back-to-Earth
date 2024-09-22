#include "NoiseMap.h"

NoiseMap::NoiseMap(){}
NoiseMap::~NoiseMap()
{
    for(auto elem : baseTerrainMap) {
        delete elem.second->heightMap;
        delete elem.second;
    }
}


void NoiseMap::init(int64_t seed)
{
    /* Noise generator */
    noise = new Noise(seed);

    /* Noise parameters */
    //Height scale
    vScale0 = 32.0;
    vScale1 = 128.0;
    
    //Sizes of noise
    tZoom = 64.0;           //Tiny noise: Small variations
    mZoom = 256.0;          //Medium noise: Hills, valleys, and ponds
    lZoom = 2048.0;         //Large noise: Mountains, large valleys and seas
    cZoom = 16.0*32768.0;   //Continental Noise: Continents and oceans
}

Noise* NoiseMap::getNoise() { return noise; }
NoiseMap::t_baseTerrainMap* NoiseMap::getBaseTerrainMap() { return &baseTerrainMap; }

NoiseMap::RegHeightMap* NoiseMap::cachedBaseTerrainHeightMapAt(int64_t rX, int64_t rY)
{
    int64_t x = rX*32;
    int64_t y = rY*32;

    //If the noise has been cached at this (rX, rY), use what's in this cache
    if( baseTerrainMap.find(std::make_pair(rX, rY))!=baseTerrainMap.end() ) {
        //Return the regHeightMap (old cached data)
        return baseTerrainMap.find(std::make_pair(rX, rY))->second;
    //If the noise has not been cached for this (rX, rY), create it.
    } else {
        RegHeightMap* rhm = new RegHeightMap();
        
        //Build empty heightmap
        rhm->heightMap = new std::vector<std::vector<int64_t>>();
        for(int sx = 0; sx<32; sx++) {
            rhm->heightMap->push_back(std::vector<int64_t>());
            for(int sy = 0; sy<32; sy++) {
                rhm->heightMap->at(sx).push_back(0);
            }
        }
        
        //Fill empty heightmap with values
        float tNoise = 0.0;
        float mNoise = 0.0;
        float lNoise = 0.0;
        float cNoise = 0.0;

        int64_t heightMapAverage = 0;
        for( char sx = 0; sx<32; sx++ )
        for( char sy = 0; sy<32; sy++ ) {
            //Calculate noise components at this location
            tNoise = noise->clampedNoise2D((x+sx)/tZoom,(y+sy)/tZoom)*vScale0;
            mNoise = noise->clampedNoise2D((x+sx)/mZoom,(y+sy)/mZoom)*vScale0;
            lNoise = noise->noise2D((x+sx)/lZoom,(y+sy)/lZoom)*vScale0;
            cNoise = noise->noise2D((x+sx)/cZoom,(y+sy)/cZoom)*vScale1;
        
            //Populate heightmap element
            int64_t thisHeight = -tNoise-mNoise-lNoise-cNoise;
            rhm->heightMap->at(sx).at(sy) = thisHeight;
            heightMapAverage += thisHeight;
        }
    

        //Store the newly computed heightmap for later use
        rhm->avgHeight = heightMapAverage/(32*32);
        baseTerrainMap.insert(std::make_pair( std::make_pair(rX, rY), rhm ));

        //Return the regHeightMap (newly computed)
        return rhm;
    }
}