#include "AudioLoader.h"
#include <nch/cpp-utils/log.h>
#include <sstream>
#include "SDLHandler.h"

int AudioLoader::timesOpened = 0;

AudioLoader::AudioLoader(){}

AudioLoader::~AudioLoader()
{
    Mix_CloseAudio();
    timesOpened = Mix_QuerySpec( &frequency, &format, &channels );

    sfxChunks.clear();
}

void AudioLoader::init(std::string resourcePath, int p_frequency, uint16_t p_format, int p_channels)
{
    //Initialization check
    if(initialized) return;
    initialized = true;

    AudioLoader::resourcePath = resourcePath;

    //SDL_Mixer open audio
    if( Mix_OpenAudio(p_frequency, p_format, p_channels, 1024)==-1 ) {
        nch::Log::warn( __PRETTY_FUNCTION__, "SDL_Mixer audio failed to open" );
    }

    //Update times opened
    timesOpened = Mix_QuerySpec( &frequency, &format, &channels );

    //Add all files
    addMixChunks();
}

void AudioLoader::init(std::string resourcePath)
{
    //22.050KHz frequency
    //MIX default format = signed 16 bit
    //2 channels (Stereo)
    //1024 byte chunks
    init(resourcePath, 48000, MIX_DEFAULT_FORMAT, 2);
}

void AudioLoader::tick()
{
	//Set volume of all music (single channel)
	Mix_VolumeMusic(masterVolumeFactor*musicVolumeFactor*MIX_MAX_VOLUME);
}

void AudioLoader::querySpecs(int& frequency, uint16_t& format, int& channels)
{
	if( !Mix_QuerySpec(&frequency, &format, &channels) ) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Mix_OpenAudio() was never called");
	}
}

Mix_Chunk* AudioLoader::getMixSfxChunk(int id)
{
	//Try to find audio chunk in the map 'audioChunks'. If non-existent, log warning and return missingChunk.
	auto chunk = sfxChunks.find(id);
	if( chunk==sfxChunks.end() ) {
		std::stringstream ss;
		
		if( isMusic(id) ) {
			
		} else {
			ss << "Couldn't find audio chunk with ID '" << id << "'";
			nch::Log::warnv(__PRETTY_FUNCTION__, "returning 'missingChunk'", ss.str() );
		}
		return missingChunk;
	}
	
	//If we found the audio chunk, return it from the map entry.
	return chunk->second;
}
Mix_Music* AudioLoader::getMixMusicChunk(int id)
{
	auto chunk = musicChunks.find(id);
	if( chunk==musicChunks.end() ) {
		std::stringstream ss;
		ss << "Couldn't find music chunk with ID '" << id << "'";
		nch::Log::warnv(__PRETTY_FUNCTION__, "returning nullptr", ss.str() );
		return nullptr;
	}
	
	return chunk->second;
}
uint32_t AudioLoader::getAudioDurationMS(int id)
{
	//Get audio device specs
	int freq = 0; uint16_t fmt = 0; int channels = 0;
	querySpecs(freq, fmt, channels);
		
	//If we are dealing with a sound effect...
	if(!isMusic(id) && getMixSfxChunk(id)!=nullptr) {
		//Get audio length in MS
		Mix_Chunk* mc = getMixSfxChunk(id);
		uint32_t points = mc->alen/((fmt&0xFF)/8);	//bytes/samplesize == points
		uint32_t frames = points/channels;			//points/channels == frames
		return ((frames*1000)/freq);				//(frames*1000)/freq == audio length in MS

	//If we are dealing with a music track...
	} else if(isMusic(id) && getMixMusicChunk(id)!=nullptr) {
		//Get music length in MS
		Mix_Music* mm = getMixMusicChunk(id);
		uint32_t mlen = (Mix_MusicDuration(mm)*1000.0);
		return mlen;
	
	//If neither...
	} else {
		nch::Log::warnv(__PRETTY_FUNCTION__, "returning -1", "Bad audio chunk ID %d", id);
		return -1;
	}
}
uint32_t AudioLoader::getAudioLastPlayedMS(int index)
{
	auto aclpItr = audioLastPlayed.find(index);
	if( aclpItr!=audioLastPlayed.end() ) {
		return aclpItr->second;
	}
	return -1;
}

bool AudioLoader::isMusic(int index)
{
	return (index>=MUSIC_kc_50_million_year_trip && index<=MUSIC_space_travel);
}

void AudioLoader::play(int index, int channel, int loops, int ticks)
{
	//Add pair(index, [current time in MS]) to map
	setMixLastPlayedMS(index, SDL_GetTicks());
	
	int res = 0;
	if( isMusic(index) ) {
		//Try to play music
		Mix_Music* pChunk = getMixMusicChunk(index);								//Get music chunk
		res = Mix_PlayMusic(pChunk, loops);											//Play music chunk
	} else {
		//Try to play audio
		Mix_Chunk* pChunk = getMixSfxChunk(index);									//Get SFX chunk
		Mix_VolumeChunk(pChunk, masterVolumeFactor*sfxVolumeFactor*MIX_MAX_VOLUME);	//Set volume of chunk
		res = Mix_PlayChannelTimed(channel, pChunk, loops, ticks);					//Play SFX chunk
	}

	//If something went wrong with playing sfx or music...
	if( res==-1 ) {
		nch::Log::errorv(__PRETTY_FUNCTION__, Mix_GetError(), "SDL_mixer error");
	}
	
}

void AudioLoader::play(int index) { play(index, -1, 0, -1); }

bool AudioLoader::playOnce(int index)
{
	uint32_t duration = getAudioDurationMS(index);
	uint32_t sinceAudioStarted = SDL_GetTicks()-getAudioLastPlayedMS(index); 
	
	//If sound effect is not still playing: play it again.
	if( sinceAudioStarted>duration || getAudioLastPlayedMS(index)==-1 ) {
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

void AudioLoader::setMasterVolumeFactor(double masvf)
{
	if(masvf<0.0){masvf = 0.0;} if(masvf>1.0){masvf = 1.0;} //Truncate value to be in [0, 1]
	masterVolumeFactor = masvf;								//Set masterVolumeFactor
}

void AudioLoader::setMusicVolumeFactor(double musvf)
{
	if(musvf<0.0){musvf = 0.0;} if(musvf>1.0){musvf = 1.0;} //Truncate value to be in [0, 1]
	musicVolumeFactor = musvf;								//Set musicVolumeFactor
}

void AudioLoader::setSfxVolumeFactor(double sfxvf)
{
	if(sfxvf<0.0){sfxvf = 0.0;} if(sfxvf>1.0){sfxvf = 1.0;}	//Truncate value to be in [0, 1]
	sfxVolumeFactor = sfxvf;								//Set sfxVolumeFactor
}

void AudioLoader::test()
{
	//Mix_VolumeChunk(getMixSfxChunk(SFX_TITLE_beam), 100);
	//Mix_PlayChannel(0, getMixSfxChunk(SFX_TITLE_beam), 0);
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

    addMixChunk(SFX_TITLE_beam, "title/beam");
    addMixChunk(SFX_TITLE_button, "title/button");
    addMixChunk(SFX_TITLE_impact, "title/impact");
	
	addMixChunk(MUSIC_kc_50_million_year_trip, "music/kc/50_million_year_trip", "mp3", true);
	addMixChunk(MUSIC_kc_alien_ruins, "music/kc/alien_ruins", "mp3", true);
	addMixChunk(MUSIC_kc_astronomy, "music/kc/astronomy", "mp3", true);
	addMixChunk(MUSIC_kc_digital_sunset, "music/kc/digital_sunset", "mp3", true);
	addMixChunk(MUSIC_kc_distant_planet, "music/kc/distant_planet", "mp3", true);
	addMixChunk(MUSIC_kc_last_stop, "music/kc/last_stop", "mp3", true);
	addMixChunk(MUSIC_kc_nuclear_winter, "music/kc/nuclear_winter", "mp3", true);
	addMixChunk(MUSIC_kc_space_dust, "music/kc/space_dust", "mp3", true);
	addMixChunk(MUSIC_kc_the_witching_hour, "music/kc/the_witching_hour", "mp3", true);
	
	addMixChunk(MUSIC_blender_engine, "music/blender_engine", "mp3", true);
	addMixChunk(MUSIC_cyber_city, "music/cyber_city", "mp3", true);
	addMixChunk(MUSIC_entering_orbit, "music/entering_orbit", "mp3", true);
	addMixChunk(MUSIC_mercury, "music/mercury", "mp3", true);
	addMixChunk(MUSIC_space_travel, "music/space_travel", "mp3", true);
	
	addMixChunk(SFX_WORLD_DESTROY_sand_1, "world/destroy/sand_1", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_sand_2, "world/destroy/sand_2", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_sand_3, "world/destroy/sand_3", "mp3");

	addMixChunk(SFX_WORLD_DESTROY_soil_1, "world/destroy/soil_1", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_soil_2, "world/destroy/soil_2", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_soil_3, "world/destroy/soil_3", "mp3");

	addMixChunk(SFX_WORLD_DESTROY_rock_1, "world/destroy/rock_1", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_rock_2, "world/destroy/rock_2", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_rock_3, "world/destroy/rock_3", "mp3");

	addMixChunk(SFX_WORLD_DESTROY_metal_1, "world/destroy/metal_1", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_metal_2, "world/destroy/metal_2", "mp3");
	addMixChunk(SFX_WORLD_DESTROY_metal_3, "world/destroy/metal_3", "mp3");

	addMixChunk(SFX_WORLD_distant_explosion, "world/distant_explosion", "mp3");
	addMixChunk(SFX_WORLD_heartbeat, "world/heartbeat");
	addMixChunk(SFX_WORLD_implosion, "world/implosion", "mp3");
	addMixChunk(SFX_WORLD_PLACE_solid_1, "world/place/solid_1", "mp3");
	addMixChunk(SFX_WORLD_PLACE_solid_2, "world/place/solid_2", "mp3");
	addMixChunk(SFX_WORLD_PLACE_solid_3, "world/place/solid_3", "mp3");
	addMixChunk(SFX_WORLD_plasma_cannon, "world/plasma_cannon", "mp3");
	addMixChunk(SFX_WORLD_air_release_1, "world/air_release_1", "mp3");
	addMixChunk(SFX_WORLD_antimatter_explosion, "world/antimatter_explosion", "mp3");
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
			nch::Log::error(__PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
		}
		
		//Insert new element into its map
		musicChunks.insert(std::make_pair(index, musicChunk));
		soundsLoaded++;
	} else {
		audioChunk = Mix_LoadWAV( path.c_str() );
		if( audioChunk==NULL ) {
			//Load the missingChunk
			nch::Log::warn( __PRETTY_FUNCTION__, "Unable to load audioChunk from '"+path+"'", "Using missing.wav instead" );
			nch::Log::error( __PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
			audioChunk = missingChunk;
			
			//If even the missingChunk is NULL, stop the program as something is very wrong.
			if( audioChunk==NULL ) {
				nch::Log::error(__PRETTY_FUNCTION__, "Could not find file 'resources/audio/missing.wav'");
				nch::Log::throwException();
			}
		}
		
		//Insert new element into its map
		sfxChunks.insert(std::make_pair(index, audioChunk));
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
	auto aclpItr = audioLastPlayed.find(index);
	if( aclpItr!=audioLastPlayed.end() ) {
		audioLastPlayed.erase(aclpItr);
	}
	audioLastPlayed.insert( std::make_pair(index, lastPlayed) );
}