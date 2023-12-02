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

void AudioLoader::querySpecs(int& frequency, uint16_t& format, int& channels)
{
	if( !Mix_QuerySpec(&frequency, &format, &channels) ) {
		Log::warn(__PRETTY_FUNCTION__, "Mix_OpenAudio() was never called");
	}
}

Mix_Chunk* AudioLoader::getMixChunk(int id)
{
	//Try to find audio chunk in the map 'audioChunks'. If non-existent, log warning and return missingChunk.
	auto chunk = audioChunks.find(id);
	if( chunk==audioChunks.end() ) {
		std::stringstream ss;
		ss << "Couldn't find audio chunk with ID '" << id << "'";
		Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning 'missingChunk'" );
		return missingChunk;
	}
	
	//If we found the audio chunk, return it from the map entry.
	return &chunk->second;
}

uint32_t AudioLoader::getMixChunkDurationMS(int id)
{
	//Get audio device specs
	int freq = 0;
	uint16_t fmt = 0;
	int channels = 0;
	querySpecs(freq, fmt, channels);
	
	//Get audio length in MS
	uint32_t points = getMixChunk(id)->alen/((fmt&0xFF)/8);		//bytes/samplesize == points
	uint32_t frames = points/channels;							//points/channels == frames
	return ((frames*1000)/freq);								//(frames*1000)/freq == audio length in MS
}

uint32_t AudioLoader::getMixLastPlayedMS(int index)
{
	auto aclpItr = audioChunksLastPlayed.find(index);
	if( aclpItr!=audioChunksLastPlayed.end() ) {
		return aclpItr->second;
	}
	return -1;
}

void AudioLoader::play(int index, int channel, int loops, int ticks, float volume)
{
	bool music = false;
	if( index>=MUSIC_blender_engine && index<=MUSIC_space_travel ) {
		music = true;
	}
	
	//Add pair(index, [current time in MS]) to map
	setMixLastPlayedMS(index, SDL_GetTicks());
	
	//Try to play audio. If mix chunk doesn't exist, log warning
	Mix_Chunk* pChunk = getMixChunk(index);

	int currentVol = Mix_Volume(channel, -1);
	int newVol = (int)((float)(MIX_MAX_VOLUME)*volume);
	//Mix_Volume(channel, newVol);
	int res = Mix_PlayChannelTimed(channel, pChunk, loops, ticks);

	if(music) {
		musicChannel = res;
	}
	
	if( res==-1 ) {
		Log::error(__PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError());
	}
	
}

void AudioLoader::play(int index, int channel, int loops, int ticks)
{
	play(index, channel, loops, ticks, 1.0f);
}

void AudioLoader::play(int index, float volume)
{
	play(index, -1, 0, -1, volume);
}

void AudioLoader::play(int index)
{
	play(index, -1, 0, -1);
}

bool AudioLoader::playOnce(int index)
{
	uint32_t duration = getMixChunkDurationMS(index);
	uint32_t sinceAudioStarted = SDL_GetTicks()-getMixLastPlayedMS(index); 
	
	//If sound effect is not still playing: play it again.
	if( sinceAudioStarted>duration || getMixLastPlayedMS(index)==-1 ) {
		play(index);
		return true;
	}
	
	//If sound effect IS still playing: do nothing, return false
	return false;
}

bool AudioLoader::playMusicTitleTheme()
{
	switch( std::rand()%3 ) {
		case 0: return playOnce(AudioLoader::MUSIC_space_travel);
		case 1: return playOnce(AudioLoader::MUSIC_cyber_city);
		case 2: return playOnce(AudioLoader::MUSIC_mercury);
	}
	
	return false;
}

bool AudioLoader::stopPlayingMusic()
{
	for(int i = MUSIC_blender_engine; i<=MUSIC_space_travel; i++) {
		setMixLastPlayedMS(i, -1);
	}
	Mix_HaltChannel(-1);
	return true;
}

void AudioLoader::addMixChunks()
{
    soundsLoaded = 0;

    missingChunk = addMixChunk(missing, "missing");

    addMixChunk(TITLE_impact, "title/impact");
    addMixChunk(TITLE_beam, "title/beam", "mp3");
	
	addMixChunk(MUSIC_blender_engine, "music/blender_engine", "mp3");
	addMixChunk(MUSIC_cyber_city, "music/cyber_city", "mp3");
	addMixChunk(MUSIC_entering_orbit, "music/entering_orbit", "mp3");
	addMixChunk(MUSIC_kalliope, "music/kalliope", "mp3");
	addMixChunk(MUSIC_mercury, "music/mercury", "mp3");
	addMixChunk(MUSIC_space_travel, "music/space_travel", "mp3");

    //addMixChunk(WORLD_WATER_flowing_heavy, "world/water/flowing_heavy");
    //addMixChunk(WORLD_WATER_flowing_normal, "world/water/flowing_normal");
    //addMixChunk(WORLD_WATER_splash, "world/water/splash");
    //addMixChunk(WORLD_WATER_submerge, "world/water/submerge");
    //addMixChunk(WORLD_WATER_swimming, "world/water/swimming");
    //addMixChunk(WORLD_WATER_underwater, "world/water/underwater");
    //addMixChunk(WORLD_WATER_underwater_deep, "world/water/underwater_deep");

    //addMixChunk(WORLD_WEATHER_rain_outside_heavy, "world/weather/rain_outside_heavy");
    //addMixChunk(WORLD_WEATHER_rain_roof_generic, "world/weather/rain_roof_generic");
    //addMixChunk(WORLD_WEATHER_rain_roof_metal, "world/weather/rain_roof_metal");
}

Mix_Chunk* AudioLoader::addMixChunk(int index, std::string path, std::string extension)
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
    audioChunks.insert(std::make_pair(index, *chunk));
	soundsLoaded++;
    return chunk;
}

Mix_Chunk* AudioLoader::addMixChunk(int index, std::string path)
{
	return addMixChunk(index, path, "wav");
}

void AudioLoader::setMixLastPlayedMS(int index, uint32_t lastPlayed)
{
	auto aclpItr = audioChunksLastPlayed.find(index);
	if( aclpItr!=audioChunksLastPlayed.end() ) {
		audioChunksLastPlayed.erase(aclpItr);
	}
	audioChunksLastPlayed.insert( std::make_pair(index, lastPlayed) );
}