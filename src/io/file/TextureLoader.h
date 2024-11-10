#pragma once
#include <iostream>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <map>
#include <vector>
#include "Defs.h"
#include "Main.h"

class TextureLoader
{
public:
	/**/
	TextureLoader();
	void init(SDL_Renderer* renderer, SDL_PixelFormat* pixelFormat, std::string resourcePath);
	virtual ~TextureLoader();
	void destroy();
	/**/
	/* Get a SDL_Surface or SDL_Texture from the lists, and their properties */
	SDL_Surface* getSurface(int index);
	SDL_Texture* getTexture(std::vector<SDL_Texture*>& texCollection, int index);
	SDL_Texture* getTexture(int index);
	SDL_Texture* getTexture(int index, int scaledSheetIndex);

	int getWidth(int index);
	int getHeight(int index);
	/**/
	void reload();

	/**/
	enum Textures
	{
		missing, //Always keep missing first
		icon,
		ootws,

		GUI_FONT_bte,
		GUI_FONT_robot,
		GUI_button,
		GUI_cursor,
		GUI_player_camera,
		GUI_player_interactions,
		GUI_window,
		GUI_save_selector,
		GUI_misc,

		PLAYER_modules,
		PLAYER_items,
		PLAYER_side_hair,
		PLAYER_side_head,
		PLAYER_side_arms,
		PLAYER_side_middle_body,
		PLAYER_side_lower_body,
		PLAYER_side_feet,
		PLAYER_top_hair,
		PLAYER_top_arms,
		PLAYER_top_middle_body,
		PLAYER_top_lower_body,

		WORLD_background_space_interstellar,
		WORLD_background_space_intergalactic,
		WORLD_region0,
		WORLD_region1,
		WORLD_TILE_type_a,
		WORLD_TILE_OVERLAY_wall,
	};

protected:

private:
	/**/
	SDL_Surface* loadSurface(std::string path);
	SDL_Surface* addSurface(std::string path);

	/**/

	void addTextures();
	void addScaledTextures();
	void load();
	void unload();
	void errorFailedTexLoad();

	/**/
	SDL_Renderer* renderer;
	/* SDLHandler peripherals */
	SDL_PixelFormat* pixelFormat;
	std::string resourcePath;

	/* List of loaded surfaces + textures */
	SDL_Surface* missingSurf;
	std::vector<SDL_Surface*> surfaces;
	Defs::t_textureAtlasesMap textureAtlasesMap;
	bool resourcesLoaded = false;
};
