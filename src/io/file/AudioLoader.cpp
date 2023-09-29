#include "AudioLoader.h"
#include <sstream>
#include "Log.h"
#include "SDLHandler.h"

int AudioLoader::timesOpened = 0;

AudioLoader::AudioLoader(){}

AudioLoader::~AudioLoader()
{
    if(!initialized) return;
    initialized = false;

    Mix_CloseAudio();
    timesOpened = Mix_QuerySpec( &frequency, &format, &channels );

    //Seems to cause a deallocation error in some cases
    //for(Mix_Chunk mc : mixChunks)
        //Mix_FreeChunk(&mc);
    audioChunks.clear();
}

void AudioLoader::init(std::string resourcePath)
{
    AudioLoader::resourcePath = resourcePath;
    //22.050KHz frequency
    //MIX default format = signed 16 bit
    //2 channels (Stereo)
    //1024 byte chunks
    init( 22050, MIX_DEFAULT_FORMAT, 2 );
}

void AudioLoader::init(int p_frequency, uint16_t p_format, int p_channels)
{
    //Initialization check
    if(initialized) return;
    initialized = true;

    //Mix open audio
    if( Mix_OpenAudio(p_frequency, p_format, p_channels, 1024)==-1 ) {
        Log::warn( __PRETTY_FUNCTION__, "SDL_Mixer audio failed to open" );
    }

    //Update times opened
    timesOpened = Mix_QuerySpec( &frequency, &format, &channels );

    //Add all files
    std::stringstream ss1; ss1 << "Beginning to load sounds. SDL ticks=" << SDL_GetTicks();
    Log::trbshoot(__PRETTY_FUNCTION__, ss1.str());
    addMixChunks();
    std::stringstream ss2; ss2 << "Finished loading " << soundsLoaded << " sounds! SDL ticks=" << SDL_GetTicks();
    Log::trbshoot(__PRETTY_FUNCTION__, ss2.str());
}

void AudioLoader::play(int index, int channel, int loops, int ticks)
{
    if( index<missing || index>=LAST_INDEX ) {
        Log::warn(__PRETTY_FUNCTION__, "Invalid index");
        return;
    }
	
    if( Mix_PlayChannelTimed(channel, &(audioChunks[index]), loops, ticks)==-1 ) {
        Log::error(__PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError());
    }
}

void AudioLoader::play(int index)
{
    play(index, -1, 0, -1);
}

void AudioLoader::addMixChunks()
{
    soundsLoaded = 0;

    missingChunk = addMixChunk(missing, "missing");

    addMixChunk(TITLE_impact, "title/impact");
	
	addMixChunk(MUSIC_blender_engine, "music/blender_engine", "mp3");
	addMixChunk(MUSIC_cyber_city, "music/cyber_city", "mp3");
	addMixChunk(MUSIC_entering_orbit, "music/entering_orbit", "mp3");
	addMixChunk(MUSIC_kalliope, "music/kalliope", "mp3");
	addMixChunk(MUSIC_mercury, "music/mercury", "mp3");
	addMixChunk(MUSIC_space_travel, "music/space_travel", "mp3");

    addMixChunk(WORLD_WATER_flowing_heavy, "world/water/flowing_heavy");
    addMixChunk(WORLD_WATER_flowing_normal, "world/water/flowing_normal");
    addMixChunk(WORLD_WATER_splash, "world/water/splash");
    addMixChunk(WORLD_WATER_submerge, "world/water/submerge");
    addMixChunk(WORLD_WATER_swimming, "world/water/swimming");
    addMixChunk(WORLD_WATER_underwater, "world/water/underwater");
    addMixChunk(WORLD_WATER_underwater_deep, "world/water/underwater_deep");

    addMixChunk(WORLD_WEATHER_rain_outside_heavy, "world/weather/rain_outside_heavy");
    addMixChunk(WORLD_WEATHER_rain_roof_generic, "world/weather/rain_roof_generic");
    addMixChunk(WORLD_WEATHER_rain_roof_metal, "world/weather/rain_roof_metal");
}

Mix_Chunk* AudioLoader::addMixChunk(int id, std::string path, std::string extension)
{
	//Get main path
    path = resourcePath + "resources/audio/" + path + "." + extension;
	//Load audio resource
	Mix_Chunk* chunk = Mix_LoadWAV( path.c_str() );
		
	if( chunk==NULL ) {
        //Load the missingChunk
        Log::warn( __PRETTY_FUNCTION__, "Unable to load mix chunk '"+path+"'", "Using missing.wav instead" );
        Log::error( __PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
        chunk = missingChunk;

        //If even the missingChunk is NULL, stop the program as something is very wrong.
        if( chunk==NULL ) {
            Log::error(__PRETTY_FUNCTION__, "Could not find file 'resources/audio/missing.wav'");
            Log::throwException();
        }
	}

	//Insert new element into map
    audioChunks.insert(std::make_pair(id, *chunk));
	soundsLoaded++;
    return chunk;
}

Mix_Chunk* AudioLoader::addMixChunk(int id, std::string path)
{
	return addMixChunk(id, path, "wav");
}