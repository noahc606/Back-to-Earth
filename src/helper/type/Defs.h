#pragma once
#include <map>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

class Defs
{
public:
    typedef std::map<int, std::vector<SDL_Texture*>> t_textureAtlasesMap;
    typedef std::map<int, Mix_Chunk*> t_audioChunkMap;

protected:

private:
};
