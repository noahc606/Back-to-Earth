#include "SpriteSheetBuilder.h"

SpriteSheetBuilder::SpriteSheetBuilder(SDLHandler* sh)
{
    sdlHandler = sh;
}

void SpriteSheetBuilder::buildSpriteSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
    switch(type) {
        case DEFAULT_PLAYER: {
            spsh.init(sdlHandler);

            spsh.setSpriteColor(pal.get("hair"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_hair, 0, 0, 0);

            spsh.setSpriteColor(pal.get("skin"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 0, 1);
            spsh.setSpriteColor(Color(255, 255, 255));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 32, 2);
            spsh.setSpriteColor(pal.get("eyes"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 64, 3);
            spsh.setSpriteColor(pal.get("mouth"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 96, 4);
            spsh.setSpriteColor(pal.get("skin"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_arms, 0, 0, 5);
            spsh.setSpriteColor(pal.get("shirt"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_middle_body, 0, 0, 6);

            spsh.setSpriteColor(pal.get("leggings"));
            spsh.addSpritesToRow(TextureLoader::PLAYER_lower_body, 10, 0, 7);

            spsh.setSpriteColor(pal.get("shoes"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_feet, 0, 0, 8);
        } break;
    }



}
