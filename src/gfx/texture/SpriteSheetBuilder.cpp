#include "SpriteSheetBuilder.h"
#include "Player.h"

SpriteSheetBuilder::SpriteSheetBuilder(SDLHandler* sh)
{
	sdlHandler = sh;
}

void SpriteSheetBuilder::buildPlayerSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
	spsh.init(sdlHandler);
	
	
	/* TOP VIEW */
	//New Row
	spsh.setSpriteColor(pal.get("hair"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_hair, 0, 0, Player::Components::TOP_HAIR);
	
	//New Row
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_arms, 0, 0, Player::Components::TOP_ARMS);
	
	//New Row
	spsh.setSpriteColor(pal.get("shirt"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_middle_body, 0, 0, Player::Components::TOP_MIDDLE_BODY);
	
	//2 New Rows
	spsh.setSpriteColor(pal.get("leggings"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_top_lower_body, 12, 0, Player::Components::TOP_LOWER_BODY);
	spsh.addSpritesToRow(TextureLoader::PLAYER_top_lower_body, 12, 32, Player::Components::TOP_LOWER_BODY+1);

	//New Row
	spsh.setSpriteColor(pal.get("hair"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_hair, 0, 0, Player::Components::SIDE_HAIR);

	/* SIDE VIEW */
	//New Row
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 0, Player::Components::SIDE_HEAD_BASE);
	
	//New Row
	spsh.setSpriteColor(Color(255, 255, 255));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 32, Player::Components::SIDE_HEAD_EYES);
	
	//New Row
	spsh.setSpriteColor(pal.get("eyes"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 64, Player::Components::SIDE_HEAD_PUPILS);
	
	//New Row
	spsh.setSpriteColor(pal.get("mouth"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 96, Player::Components::SIDE_HEAD_MOUTH);
	
	//New Row
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_arms, 0, 0, Player::Components::SIDE_ARMS);
	
	//New Row
	spsh.setSpriteColor(pal.get("shirt"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_middle_body, 0, 0, Player::Components::SIDE_MIDDLE_BODY);

	//3 New Rows
	spsh.setSpriteColor(pal.get("leggings"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 0, Player::Components::SIDE_LOWER_BODY);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 32, Player::Components::SIDE_LOWER_BODY+1);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 64, Player::Components::SIDE_LOWER_BODY+2);

	//New Row
	spsh.setSpriteColor(pal.get("shoes"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_feet, 0, 0, Player::Components::SIDE_FEET);
}

void SpriteSheetBuilder::buildSpriteSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
	switch(type) {
		case DEFAULT_PLAYER: buildPlayerSheet(type, spsh, pal); break;
	}
}
