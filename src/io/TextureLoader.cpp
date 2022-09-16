#include "TextureLoader.h"
#include <iostream>
#include <sstream>
#include "Controls.h"
#include "Log.h"
#include "SDLHandler.h"
#include "Timer.h"

TextureLoader::TextureLoader(){}
void TextureLoader::init(SDL_Renderer* p_renderer, SDL_PixelFormat* p_pixelFormat, std::string p_resourcePath)
{
    renderer = p_renderer;
    pixelFormat = p_pixelFormat;
    resourcePath = p_resourcePath;

    load();
}
TextureLoader::~TextureLoader()
{
    unload();
}

void TextureLoader::reload()
{
    //Reload images and track how much time it takes
    {
        Timer t("reloading images");
        unload();
        load();
    }
}

SDL_Surface* TextureLoader::getSurface(int index)
{
    try {
        return surfaces.at(index);
    }
    catch (const std::out_of_range &err) {
        std::stringstream ss; ss << "index '" << index << "' out of range";
        Log::warn(__PRETTY_FUNCTION__, ss.str(), "using 'missing' surface");
        return surfaces.at(Textures::missing);
    }

    return NULL;
}

SDL_Texture* TextureLoader::getTexture(int index)
{
    try {
        return textures.at(index);
    }
    catch (const std::out_of_range &err) {
        std::stringstream ss; ss << "index '" << index << "' out of range";
        Log::warn(__PRETTY_FUNCTION__, "index out of range", "using 'missing' texture");
        return textures.at(missing);
    }

    return NULL;
}

int TextureLoader::getWidth(int index)
{
    SDL_Surface* s = getSurface(index);
    return s->w;
}

int TextureLoader::getHeight(int index)
{
    SDL_Surface* s = getSurface(index);
    return s->h;
}

//SDL_Renderer* TextureLoader::getRenderer() { return renderer; }

void TextureLoader::load()
{
    //If resources have not been loaded yet
    if(!resourcesLoaded) {
        /* Add surfaces from... */

        //Main directory
        missingSurf = addSurface("missing");
        addSurface("icon");
        //GUI
        addSurface("gui\\font\\bte");
        addSurface("gui\\font\\robot");
        addSurface("gui\\button");
        addSurface("gui\\cursor");
        addSurface("gui\\player_interactions");
        addSurface("gui\\window");
        //Player
        addSurface("player\\hair");
        addSurface("player\\head");
        addSurface("player\\arms");
        addSurface("player\\middle_body");
        addSurface("player\\lower_body");
        addSurface("player\\feet");
        //World
        addSurface("world\\background\\space\\interstellar");
        addSurface("world\\region0");
        addSurface("world\\region1");
        addSurface("world\\tile\\type_a");
        addSurface("world\\tile\\overlay\\depth");
        addSurface("world\\tile\\overlay\\wall");

        //Create textures from all surfaces
        for(int i = 0; i<(int)surfaces.size(); i++) {
            textures.push_back(SDL_CreateTextureFromSurface(renderer, surfaces[i]));
        }
    //If resources have already been loaded
    } else {
        Log::warn(__PRETTY_FUNCTION__, "Resources have already been loaded");
    }

    //Set resourcesLoaded flag
    resourcesLoaded = true;
}

void TextureLoader::unload()
{
    //If resources have already been loaded
    if(resourcesLoaded) {
        //Clear all surfaces
        for(SDL_Surface* s : surfaces)
            SDL_FreeSurface(s);
        surfaces.clear();
        //Clear all textures
        for(SDL_Texture* t : textures)
            SDL_DestroyTexture(t);
        textures.clear();
    //If resources have not been loaded yet
    } else {
        Log::warn(__PRETTY_FUNCTION__, "Resources haven't been loaded yet");
    }

    //Set resourcesLoaded flag
    resourcesLoaded = false;
}

SDL_Surface* TextureLoader::loadSurface(std::string path)
{
    std::string filename = path;
	//Load image at specified path
	path = resourcePath + "resources\\textures\\" + path;
	SDL_Surface* img = IMG_Load(path.c_str());
	SDL_Surface* surf = SDL_ConvertSurface(img, pixelFormat, 0);
    SDL_FreeSurface(img);

	if( surf==NULL ) {
	    if(filename=="missing.png") {
            Log::error(__PRETTY_FUNCTION__, "Failed to load image resources", "unable to load default image '"+path+"'");
            Log::error(__PRETTY_FUNCTION__, "IMG_Error: ", IMG_GetError());
            Log::throwException();
            return nullptr;
	    } else {
            Log::warn(__PRETTY_FUNCTION__, "Unable to load image '"+path+"'", "using default 'missing.png' image");
            Log::error(__PRETTY_FUNCTION__, "IMG_Error: ", IMG_GetError());
            return missingSurf;
	    }
	}

	return surf;
}

SDL_Surface* TextureLoader::addSurface(std::string path)
{
    SDL_Surface* surf = loadSurface(path+".png");
    surfaces.push_back(surf);
    return surf;
}
