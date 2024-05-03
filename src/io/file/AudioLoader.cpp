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

    //Seems to cause a crash in some cases. Not really worth fixing anyway since application already exited by this point.
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

Mix_Chunk* AudioLoader::getMixAudioChunk(int id)
{
	//Try to find audio chunk in the map 'audioChunks'. If non-existent, log warning and return missingChunk.
	auto chunk = audioChunks.find(id);
	if( chunk==audioChunks.end() ) {
		std::stringstream ss;
		
		if( isMusic(id) ) {
			
		} else {
			ss << "Couldn't find audio chunk with ID '" << id << "'";
			Log::warnv(__PRETTY_FUNCTION__, "returning 'missingChunk'", ss.str() );
		}
		return missingChunk;
	}
	
	//If we found the audio chunk, return it from the map entry.
	return &chunk->second;
}

Mix_Music* AudioLoader::getMixMusicChunk(int id)
{
	auto chunk = musicChunks.find(id);
	if( chunk==musicChunks.end() ) {
		std::stringstream ss;
		ss << "Couldn't find music chunk with ID '" << id << "'";
		Log::warnv(__PRETTY_FUNCTION__, "returning nullptr", ss.str() );
		return nullptr;
	}
	
	return chunk->second;
}

uint32_t AudioLoader::getMixAudioChunkDurationMS(int id)
{
	//Get audio device specs
	int freq = 0;
	uint16_t fmt = 0;
	int channels = 0;
	querySpecs(freq, fmt, channels);
	
	//Get audio length in MS
	uint32_t points = getMixAudioChunk(id)->alen/((fmt&0xFF)/8);		//bytes/samplesize == points
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

bool AudioLoader::isMusic(int index)
{
	return (index>=MUSIC_kc_50_million_year_trip && index<=MUSIC_space_travel);
}

void AudioLoader::play(int index, int channel, int loops, int ticks, float volume)
{
	//Add pair(index, [current time in MS]) to map
	setMixLastPlayedMS(index, SDL_GetTicks());
	
	int res = 0;
	if( isMusic(index) ) {
		musicChannel = res;
		
		Mix_Music* pChunk = getMixMusicChunk(index);
		int currentVol = Mix_VolumeMusic(-1);
		int newVol = (int)((float)(MIX_MAX_VOLUME)*volume/9); 
		Mix_VolumeMusic(newVol);
		
		int res = Mix_PlayMusic(pChunk, loops);
		
	} else {
		//Try to play audio. If mix chunk doesn't exist, log warning
		Mix_Chunk* pChunk = getMixAudioChunk(index);
		int currentVol = Mix_Volume(channel, -1);
		int newVol = (int)((float)(MIX_MAX_VOLUME)*volume);
		//Mix_Volume(channel, newVol);
		int res = Mix_PlayChannelTimed(channel, pChunk, loops, ticks);
	}

	if( res==-1 ) {
		Log::errorv(__PRETTY_FUNCTION__, Mix_GetError(), "SDL_mixer error");
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
	uint32_t duration = getMixAudioChunkDurationMS(index);
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
	
	Mix_HaltMusic();
	//Mix_HaltChannel(-1);
	return true;
}

void AudioLoader::addMixChunks()
{
    soundsLoaded = 0;

    missingChunk = addMixChunk(missing, "missing");

    addMixChunk(TITLE_beam, "title/beam");
    addMixChunk(TITLE_button, "title/button");
    addMixChunk(TITLE_impact, "title/impact");
	
	addMixChunk(MUSIC_kc_50_million_year_trip, "music/kc/50_million_year_trip", "mp3", true);
	addMixChunk(MUSIC_kc_alien_ruins, "music/kc/alien_ruins", "mp3", true);
	addMixChunk(MUSIC_kc_digital_sunset, "music/kc/digital_sunset", "mp3", true);
	addMixChunk(MUSIC_kc_distant_planet, "music/kc/distant_planet", "mp3", true);
	addMixChunk(MUSIC_kc_nuclear_winter, "music/kc/nuclear_winter", "mp3", true);
	addMixChunk(MUSIC_kc_space_dust, "music/kc/space_dust", "mp3", true);
	addMixChunk(MUSIC_kc_the_witching_hour, "music/kc/the_witching_hour", "mp3", true);
	
	addMixChunk(MUSIC_blender_engine, "music/blender_engine", "mp3", true);
	addMixChunk(MUSIC_cyber_city, "music/cyber_city", "mp3", true);
	addMixChunk(MUSIC_entering_orbit, "music/entering_orbit", "mp3", true);
	addMixChunk(MUSIC_mercury, "music/mercury", "mp3", true);
	addMixChunk(MUSIC_space_travel, "music/space_travel", "mp3", true);
	
	addMixChunk(WORLD_distant_explosion, "world/distant_explosion", "mp3");
	addMixChunk(WORLD_heartbeat, "world/heartbeat");
	addMixChunk(WORLD_implosion, "world/implosion", "mp3");
	addMixChunk(WORLD_plasma_cannon, "world/plasma_cannon", "mp3");
}

/**
 * Adds either a music chunk to 'musicChunks' or an audio chunk to 'audioChunks'.
 * If an audio chunk is added successfully, then the audio chunk will be returned. If a music chunk is added, nullptr will be returned.
 */
Mix_Chunk* AudioLoader::addMixChunk(int index, std::string path, std::string extension, bool music)
{
	//Get main path
    path = resourcePath + "resources/audio/" + path + "." + extension;
	
	//Load audio resource
	Mix_Chunk* audioChunk = nullptr;
	if( music ) {
		Mix_Music* musicChunk =  Mix_LoadMUS( path.c_str() );
		
		if(musicChunk==NULL) {
			Log::error(__PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
		}
		
		//Insert new element into its map
		musicChunks.insert(std::make_pair(index, musicChunk));
		soundsLoaded++;
	} else {
		audioChunk = Mix_LoadWAV( path.c_str() );
		if( audioChunk==NULL ) {
			//Load the missingChunk
			Log::warn( __PRETTY_FUNCTION__, "Unable to load audioChunk from '"+path+"'", "Using missing.wav instead" );
			Log::error( __PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
			audioChunk = missingChunk;
			
			//If even the missingChunk is NULL, stop the program as something is very wrong.
			if( audioChunk==NULL ) {
				Log::error(__PRETTY_FUNCTION__, "Could not find file 'resources/audio/missing.wav'");
				Log::throwException();
			}
		}
		
		//Insert new element into its map
		audioChunks.insert(std::make_pair(index, *audioChunk));
		soundsLoaded++;
	}



    return audioChunk;
}

Mix_Chunk* AudioLoader::addMixChunk(int index, std::string path, std::string extension)
{
	return addMixChunk(index, path, extension, false);
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