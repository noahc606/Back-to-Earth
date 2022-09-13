#pragma once
#include <iostream>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
#include <vector>
#include "Main.h"

class TextureLoader
{
public:
    /**/
    TextureLoader();
    void init(SDL_Renderer* renderer, SDL_PixelFormat* pixelFormat, std::string resourcePath);
    virtual ~TextureLoader();
    /**/
    /* Get a SDL_Surface or SDL_Texture from the lists, and their properties */
    SDL_Surface* getSurface(int index);
    SDL_Texture* getTexture(int index);
    int getWidth(int index);
    int getHeight(int index);
    /**/
    void reload();

    /**/
    enum Textures
    {
        missing, //Always keep missing first
        icon,

        GUI_bte_font,
        GUI_button,
        GUI_cursor,
        GUI_player_interactions,
        GUI_window,

        PLAYER_hair,
        PLAYER_head,
        PLAYER_arms,
        PLAYER_middle_body,
        PLAYER_lower_body,
        PLAYER_feet,

        WORLD_background_space_interstellar,
        WORLD_region0,
        WORLD_region1,
        WORLD_TILE_type_a,
        WORLD_TILE_OVERLAY_depth,
        WORLD_TILE_OVERLAY_wall,
    };

protected:

private:
    /**/
    SDL_Surface* loadSurface(std::string path);
    SDL_Surface* addSurface(std::string path);

    /**/
    void load();
    void unload();

    /**/
    /* SDLHandler peripherals */
    SDL_Renderer* renderer;
    SDL_PixelFormat* pixelFormat;
    std::string resourcePath;

    /* List of loaded surfaces + textures */
    SDL_Surface* missingSurf;
    std::vector<SDL_Surface*> surfaces;
    std::vector<SDL_Texture*> textures;
    bool resourcesLoaded = false;
};
