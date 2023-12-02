#pragma once
#include <map>
#include <SDL2/SDL_mixer.h>
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
	Mix_Chunk* getMixChunk(int index);
	uint32_t getMixChunkDurationMS(int index);
	uint32_t getMixLastPlayedMS(int index);

    void play(int index, int channel, int loops, int ticks, float volume);
    void play(int index, int channel, int loops, int ticks);
    void play(int index, float volume);
    void play(int index);
    bool playOnce(int index);
	bool playMusicTitleTheme();
	
	bool stopPlayingMusic();

    enum Chunks {
        missing,    //Always keep missing first

        TITLE_impact,
		TITLE_beam,
		
		MUSIC_blender_engine,
		MUSIC_cyber_city,
		MUSIC_entering_orbit,
		MUSIC_kalliope,
		MUSIC_mercury,
		MUSIC_space_travel,

        LAST_INDEX, //Keep this last
    };

protected:

private:

    std::string resourcePath;

    unsigned int soundsLoaded = 0;
    bool initialized = false;
    int frequency = 22050;
    uint16_t format = MIX_DEFAULT_FORMAT;
    int channels = 2;
    static int timesOpened;
	int musicChannel = 0;
	
    std::map<int, Mix_Chunk> audioChunks;
    std::map<int, uint64_t> audioChunksLastPlayed;
    std::map<int, bool> audioChunkStates;

    Mix_Chunk* missingChunk;

    void addMixChunks();
    Mix_Chunk* addMixChunk(int index, std::string path, std::string extension);
    Mix_Chunk* addMixChunk(int index, std::string path);
	void setMixLastPlayedMS(int index, uint32_t lastPlayed);
};
