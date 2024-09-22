#pragma once
#include <cstdint>
#include <map>
#include <vector>
#include "Noise.h"

class NoiseMap
{
public:
    struct RegHeightMap {
        int64_t avgHeight = -1;
        std::vector<std::vector<int64_t>>* heightMap = nullptr;
    };
    
    typedef std::map< std::pair<int64_t, int64_t>, RegHeightMap*> t_baseTerrainMap;

    NoiseMap();
    ~NoiseMap();
    void init(int64_t seed);
    
    Noise* getNoise();
    t_baseTerrainMap* getBaseTerrainMap();
    RegHeightMap* cachedBaseTerrainHeightMapAt(int64_t rX, int64_t rY);

private:
    
    t_baseTerrainMap baseTerrainMap;

    Noise* noise;
    float vScale0, vScale1;
    float tZoom, mZoom, lZoom, cZoom;
};