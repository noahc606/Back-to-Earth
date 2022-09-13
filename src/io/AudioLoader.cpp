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

    for(Mix_Chunk mc : mixChunks)
        Mix_FreeChunk(&mc);
    mixChunks.clear();
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

void AudioLoader::play(int index)
{
    play(index, -1, 0, -1);
}

void AudioLoader::play(int index, int channel, int loops, int ticks)
{
    if( index<missing || index>=LAST_INDEX ) {
        Log::warn(__PRETTY_FUNCTION__, "Invalid index");
        return;
    }

    if( Mix_PlayChannelTimed(channel, &(mixChunks[index]), loops, ticks)==-1 ) {
        Log::error(__PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError());
    }
}

void AudioLoader::addMixChunks()
{
    soundsLoaded = 0;

    missingChunk = addMixChunk("missing");

    addMixChunk("title\\impact");

    addMixChunk("world\\water\\flowing_heavy");
    addMixChunk("world\\water\\flowing_normal");
    addMixChunk("world\\water\\splash");
    addMixChunk("world\\water\\submerge");
    addMixChunk("world\\water\\swimming");
    addMixChunk("world\\water\\underwater");
    addMixChunk("world\\water\\underwater_deep");

    addMixChunk("world\\weather\\rain_outside_heavy");
    addMixChunk("world\\weather\\rain_roof_generic");
    addMixChunk("world\\weather\\rain_roof_metal");
}

Mix_Chunk* AudioLoader::addMixChunk(std::string path)
{
    soundsLoaded++;

    path = resourcePath + "resources\\audio\\" + path + ".wav";
	Mix_Chunk* chunk = Mix_LoadWAV( path.c_str() );

	if( chunk==NULL ) {
        //Load the missingChunk
        Log::warn( __PRETTY_FUNCTION__, "Unable to load mix chunk '"+path+"'", "Using missing.wav instead" );
        Log::error( __PRETTY_FUNCTION__, "SDL_mixer error", Mix_GetError() );
        chunk = missingChunk;

        //If even the missingChunk is NULL, stop the program as something is very wrong.
        if( chunk==NULL ) {
            Log::error(__PRETTY_FUNCTION__, "Could not find file 'resources\\audio\\missing.wav'");
            Log::throwException();
        }
	}

    mixChunks.push_back(*chunk);
    return chunk;
}
