#include "TextureLoader.h"
#include <iostream>
#include <sstream>
#include "Controls.h"
#include "Log.h"
#include "SDLHandler.h"
#include "Timer.h"
#include "Texture.h"

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

SDL_Texture* TextureLoader::getTexture(std::vector<SDL_Texture*>& texCollection, int index)
{
    try {
        return texCollection.at(index);
    } catch (const std::out_of_range &err) {
        std::stringstream ss; ss << "index '" << index << "' out of range [0, " << (texCollection.size()-1) << "]";
        Log::warn(__PRETTY_FUNCTION__, ss.str(), "using 'missing' texture");
        return textures.at(missing);
    }

    Log::warn(__PRETTY_FUNCTION__, "Unknown texture get error", "returning NULL");
    return NULL;
}
SDL_Texture* TextureLoader::getTexture(int index) { return getTexture(textures, index); }
/**
    For certain atlas textures (like for ex: tiles), program keeps smaller scaled copies of it in memory for LOD rendering
    (faster drawing when on a zoomed out map where details are less necessary). You can access these with this function. The
    addScaledTextures() function creates the scaled copies.
*/
SDL_Texture* TextureLoader::getTexture(int index, int scaledSheetIndex)
{
    switch(index) {
        case WORLD_TILE_type_a:         return getTexture(scaledTileTypeA, scaledSheetIndex); break;
        case WORLD_TILE_OVERLAY_wall:   return getTexture(scaledTileOverlayWall, scaledSheetIndex); break;
    }

    return nullptr;
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

        {
            //Main directory
            missingSurf = addSurface("missing");
            addSurface("icon");
            //GUI
            addSurface("gui/font/bte");
            addSurface("gui/font/robot");
            addSurface("gui/button");
            addSurface("gui/cursor");
            addSurface("gui/player_interactions");
            addSurface("gui/window");
            //Player
            addSurface("player/hair");
            addSurface("player/head");
            addSurface("player/arms");
            addSurface("player/middle_body");
            addSurface("player/lower_body");
            addSurface("player/feet");
            //World
            addSurface("world/background/space/interstellar");
            addSurface("world/region0");
            addSurface("world/region1");
            addSurface("world/tile/type_a");
            addSurface("world/tile/overlay/wall");
        }
        //Create textures from all surfaces
        addTextures();
        //Some textures are better to have scaled copies of for faster map rendering (ex: tiles)
        addScaledTextures();
    //If resources have already been loaded
    } else {
        Log::warn(__PRETTY_FUNCTION__, "Resources have already been loaded");
    }

    //Set resourcesLoaded flag
    resourcesLoaded = true;
}

void TextureLoader::addTextures()
{
    for(int i = 0; i<(int)surfaces.size(); i++) {
        //Create 'surf' = to this loop iteration's surface
        SDL_Surface* surf = surfaces[i];
        //Create 'staticTex'
        SDL_Texture* staticTex = SDL_CreateTextureFromSurface(renderer, surf);
        //Create 'targetTex'
        SDL_Texture* targetTex = SDL_CreateTexture(renderer, surf->format->format, SDL_TEXTUREACCESS_TARGET, surf->w, surf->h);
        SDL_SetTextureBlendMode(targetTex, SDL_BLENDMODE_BLEND);

        //Copy 'staticTex' to 'targetTex'
        SDL_Texture* oldTarget = SDL_GetRenderTarget(renderer);
        SDL_SetRenderTarget(renderer, targetTex);
        SDL_RenderCopy(renderer, staticTex, NULL, NULL);
        SDL_SetRenderTarget(renderer, oldTarget);

        //Discard 'staticTex' and add 'targetTex'
        SDL_DestroyTexture(staticTex);
        textures.push_back(targetTex);
    }
}

void TextureLoader::addScaledTextures()
{
    //List of all scalable texture IDs.
    int scalables[] = { WORLD_TILE_type_a, WORLD_TILE_OVERLAY_wall };

    //Store the original render target
    SDL_Texture* rtOld = SDL_GetRenderTarget(renderer);
    for( int i = 0; i<2; i++ ) {
        /**
            For the texture we are on, we create copies of that texture, each successive one being half the size of the last.
            For example: a 1024x1024 texture gives a vector of textures with these sizes:
                1024x1024, 512x512, 256x256, ... ,4x4, 2x2, 1x1 (total of 11 textures).
        */
        int stexW = surfaces[scalables[i]]->w;
        int stexH = surfaces[scalables[i]]->h;
        //While both dimensions are nonzero create copies. Also half 'stexW' and 'stexH' at the end of each loop iteration.
        while( stexW!=0 && stexH!=0 ) {

            //Create stex with proper w/h; Set render target to stex; rcopy texture onto stex for scaled version.
            SDL_Texture* stex = SDL_CreateTexture( renderer, pixelFormat->format, SDL_TEXTUREACCESS_TARGET, stexW, stexH );
            SDL_SetRenderTarget( renderer, stex );
            SDL_RenderCopy( renderer, getTexture(scalables[i]), NULL, NULL );

            //Add stex to the proper vector
            switch( scalables[i] ) {
                case WORLD_TILE_type_a:         scaledTileTypeA.push_back( stex ); break;
                case WORLD_TILE_OVERLAY_wall:   scaledTileOverlayWall.push_back( stex ); break;
            }

            //Halves the size of the texture next time.
            stexW = stexW/2;
            stexH = stexH/2;
        }
    }

    //Set render target to what it was originally
    SDL_SetRenderTarget(renderer, rtOld);
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
	path = resourcePath + "resources/textures/" + path;

	SDL_Surface* img = IMG_Load(path.c_str());
	SDL_Surface* surf = SDL_ConvertSurface(img, pixelFormat, 0);
    SDL_FreeSurface(img);

	if( surf==NULL ) {
	    if(filename=="missing.png") {
            Log::error(__PRETTY_FUNCTION__, "Failed to load image resources", "unable to load default image '"+path+"'");
            Log::error(__PRETTY_FUNCTION__, "IMG_Error: ", IMG_GetError());
            Log::log("Is the \"backtoearth\" folder in the same directory as the main executable file? If not, move file(s) around so that it is.");
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
