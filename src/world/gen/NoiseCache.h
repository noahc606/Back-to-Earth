#pragma once
#include <map>
#include <cstdint>

class NoiseCache
{
public:
    NoiseCache();
    ~NoiseCache();

    struct NoiseRegion
    {
    };
    
private:
    
    std::map<std::tuple<int64_t, int64_t, int64_t>, int> noiseMap;
};