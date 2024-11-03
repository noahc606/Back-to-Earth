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
    void init(std::string resourcePath, int p_frequency, uint16_t p_format, int p_channels);
    void init(std::string resourcePath);
    void tick();

    void querySpecs(int& frequency, uint16_t& format, int& channels);
	Mix_Chunk* getMixSfxChunk(int index);
	Mix_Music* getMixMusicChunk(int index);
	uint32_t getAudioDurationMS(int index);
	uint32_t getAudioLastPlayedMS(int index);
	bool isMusic(int index);

    void play(int index, int channel, int loops, int ticks);
    void play(int index);
    bool playOnce(int index);
	bool playMusicTitleTheme();

    void setMasterVolumeFactor(double masvf);
    void setMusicVolumeFactor(double musvf);
    void setSfxVolumeFactor(double sfxvf);

    void test();
	
	bool stopPlayingMusic();

    enum Chunks {
        missing,    //Always keep missing first

		SFX_TITLE_beam,
		SFX_TITLE_button,
        SFX_TITLE_impact,
		
		MUSIC_kc_50_million_year_trip,
		MUSIC_kc_alien_ruins,
		MUSIC_kc_digital_sunset,
		MUSIC_kc_distant_planet,
        MUSIC_kc_last_stop,
		MUSIC_kc_nuclear_winter,
		MUSIC_kc_space_dust,
		MUSIC_kc_the_witching_hour,
		MUSIC_blender_engine,
		MUSIC_cyber_city,
		MUSIC_entering_orbit,
		MUSIC_mercury,
		MUSIC_space_travel,
		
		SFX_WORLD_DESTROY_sand_1, SFX_WORLD_DESTROY_sand_2, SFX_WORLD_DESTROY_sand_3,
        SFX_WORLD_DESTROY_soil_1, SFX_WORLD_DESTROY_soil_2, SFX_WORLD_DESTROY_soil_3,
		SFX_WORLD_DESTROY_rock_1, SFX_WORLD_DESTROY_rock_2, SFX_WORLD_DESTROY_rock_3,
		SFX_WORLD_DESTROY_metal_1, SFX_WORLD_DESTROY_metal_2, SFX_WORLD_DESTROY_metal_3,
        SFX_WORLD_distant_explosion,
		SFX_WORLD_heartbeat,
		SFX_WORLD_implosion,
		SFX_WORLD_plasma_cannon,
        SFX_WORLD_air_release_1,

		LAST_INDEX, //Keep this last
	};

protected:

private:

    std::string resourcePath;

    unsigned int soundsLoaded = 0;
    bool initialized = false;
    int frequency = 22050; uint16_t format = MIX_DEFAULT_FORMAT; int channels = 2;
    
    static int timesOpened;
	
    //Store audio within two maps: one for sound, one for music
    std::map<int, Mix_Chunk*> sfxChunks;
    std::map<int, Mix_Music*> musicChunks;
	
    std::map<int, uint64_t> audioLastPlayed;    //Track the last time a particular audio track was played

    double masterVolumeFactor = 0.5;
    double musicVolumeFactor = 0.75;
    double sfxVolumeFactor = 0.25;

    Mix_Chunk* missingChunk;

    void addMixChunks();
    Mix_Chunk* addMixChunk(int index, std::string path, std::string extension, bool music);
    Mix_Chunk* addMixChunk(int index, std::string path, std::string extension);
    Mix_Chunk* addMixChunk(int index, std::string path);
	void setMixLastPlayedMS(int index, uint32_t lastPlayed);
};
