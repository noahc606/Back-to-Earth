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
	Mix_Chunk* getMixAudioChunk(int index);
	Mix_Music* getMixMusicChunk(int index);
	uint32_t getMixAudioChunkDurationMS(int index);
	uint32_t getMixLastPlayedMS(int index);
	bool isMusic(int index);

    void play(int index, int channel, int loops, int ticks, float volume);
    void play(int index, int channel, int loops, int ticks);
    void play(int index, float volume);
    void play(int index);
    bool playOnce(int index);
	bool playMusicTitleTheme();
	
	bool stopPlayingMusic();

    enum Chunks {
        missing,    //Always keep missing first

		TITLE_beam,
		TITLE_button,
        TITLE_impact,
		
		MUSIC_kc_50_million_year_trip,
		MUSIC_kc_alien_ruins,
		MUSIC_kc_digital_sunset,
		MUSIC_kc_distant_planet,
		MUSIC_kc_nuclear_winter,
		MUSIC_kc_space_dust,
		MUSIC_kc_the_witching_hour,
		MUSIC_blender_engine,
		MUSIC_cyber_city,
		MUSIC_entering_orbit,
		MUSIC_mercury,
		MUSIC_space_travel,
		
		WORLD_distant_explosion,
		WORLD_heartbeat,
		WORLD_implosion,
		WORLD_plasma_cannon,
		
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
    std::map<int, Mix_Music*> musicChunks;
	
    std::map<int, uint64_t> audioChunksLastPlayed;
    std::map<int, bool> audioChunkStates;

    Mix_Chunk* missingChunk;

    void addMixChunks();
    Mix_Chunk* addMixChunk(int index, std::string path, std::string extension, bool music);
    Mix_Chunk* addMixChunk(int index, std::string path, std::string extension);
    Mix_Chunk* addMixChunk(int index, std::string path);
	void setMixLastPlayedMS(int index, uint32_t lastPlayed);
};
