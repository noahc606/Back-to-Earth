#include "SpriteSheetBuilder.h"
#include "Player.h"

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
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 0, Player::Components::HEAD_BASE);
            spsh.setSpriteColor(Color(255, 255, 255));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 32, Player::Components::HEAD_EYES);
            spsh.setSpriteColor(pal.get("eyes"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 64, Player::Components::HEAD_EYES+1);
            spsh.setSpriteColor(pal.get("mouth"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_head, 0, 96, Player::Components::HEAD_MOUTH);
            spsh.setSpriteColor(pal.get("skin"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_arms, 0, 0, Player::Components::ARMS);
            spsh.setSpriteColor(pal.get("shirt"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_middle_body, 0, 0, Player::Components::MIDDLE_BODY);

            spsh.setSpriteColor(pal.get("leggings"));
            spsh.addSpritesToRow(TextureLoader::PLAYER_lower_body, 12, 0, Player::Components::LOWER_BODY);
            spsh.addSpritesToRow(TextureLoader::PLAYER_lower_body, 12, 32, Player::Components::LOWER_BODY+1);
            spsh.addSpritesToRow(TextureLoader::PLAYER_lower_body, 12, 64, Player::Components::LOWER_BODY+2);

            spsh.setSpriteColor(pal.get("shoes"));
            spsh.addSpriteToRow(TextureLoader::PLAYER_feet, 0, 0, Player::Components::FEET);
        } break;
    }



}
