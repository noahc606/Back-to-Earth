#pragma once
#include <SDL_mixer.h>
#include <string>
#include <vector>

class AudioLoader
{
public:
    AudioLoader();
    virtual ~AudioLoader();
    void init(std::string resourcePath);
    void init(int p_frequency, uint16_t p_format, int p_channels);

    void querySpecs(int& frequency, uint16_t& format, int& channels);

    void play(int index);
    void play(int index, int channel, int loops, int ticks);
    std::vector<Mix_Chunk> mixChunks;

    enum Chunks {
        missing,    //Always keep missing first

        TITLE_impact,

        WORLD_WATER_flowing_heavy,
        WORLD_WATER_flowing_normal,
        WORLD_WATER_splash,
        WORLD_WATER_submerge,
        WORLD_WATER_swimming,
        WORLD_WATER_underwater,
        WORLD_WATER_underwater_deep,

        WORLD_WEATHER_rain_outside_heavy,
        WORLD_WEATHER_rain_roof_generic,
        WORLD_WEATHER_rain_roof_metal,

        LAST_INDEX, //Keep this last
    };

protected:

private:

    std::string resourcePath;

    void addMixChunks();
    Mix_Chunk* addMixChunk(std::string path);

    unsigned int soundsLoaded = 0;
    bool initialized = false;
    int frequency = 22050;
    uint16_t format = MIX_DEFAULT_FORMAT;
    int channels = 2;
    static int timesOpened;

    Mix_Chunk* missingChunk;
};
