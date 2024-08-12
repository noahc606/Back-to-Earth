#pragma once
#include <map>
#include <cstdint>

class NoiseMap
{
public:
    struct NoiseRegion {};
    
private:
    
    std::map<std::tuple<int64_t, int64_t, int64_t>, int> noiseMap;
};