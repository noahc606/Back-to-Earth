#pragma once
#include <cstdint>
#include <map>
#include <vector>

class NoiseMap
{
public:
    typedef std::map< std::pair<int64_t, int64_t>, std::vector<std::vector<int64_t>>*> t_baseTerrainMap;

    struct NoiseRegion {};
    
private:
    
    t_baseTerrainMap baseTerrainMap;
};