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
	//Row 1
	spsh.setSpriteColor(pal.get("hair"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_hair, 0, 0, PlayerAnimation::TOP_HAIR);	
	//Row 2
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_arms, 0, 0, PlayerAnimation::TOP_ARMS);
	//Row 3
	spsh.setSpriteColor(pal.get("shirt"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_top_middle_body, 0, 0, PlayerAnimation::TOP_MIDDLE_BODY);
	//Rows 4 and 5
	spsh.setSpriteColor(pal.get("leggings"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_top_lower_body, 12, 0, PlayerAnimation::TOP_LOWER_BODY);
	spsh.addSpritesToRow(TextureLoader::PLAYER_top_lower_body, 12, 32, PlayerAnimation::TOP_LOWER_BODY_B);
	//Row 6
	spsh.setSpriteColor(pal.get("hair"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_hair, 0, 0, PlayerAnimation::SIDE_HAIR);
/* SIDE VIEW */
	//Row 7
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 0, PlayerAnimation::SIDE_HEAD_BASE);
	//Row 8
	spsh.setSpriteColor(nch::Color(255, 255, 255));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 32, PlayerAnimation::SIDE_HEAD_EYES);
	//Row 9
	spsh.setSpriteColor(pal.get("eyes"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 64, PlayerAnimation::SIDE_HEAD_PUPILS);
	//Row 10
	spsh.setSpriteColor(pal.get("mouth"));
	spsh.addSpriteToRow(TextureLoader::PLAYER_side_head, 0, 96, PlayerAnimation::SIDE_HEAD_MOUTH);
	//Row 11
	spsh.setSpriteColor(pal.get("skin"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_arms, 8, 0, PlayerAnimation::SIDE_ARMS);
	//Row 12
	spsh.setSpriteColor(pal.get("shirt"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_middle_body, 8, 0, PlayerAnimation::SIDE_MB_TURNING);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_middle_body, 8, 32, PlayerAnimation::SIDE_MB_WALKING);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_middle_body, 8, 64, PlayerAnimation::SIDE_MB_WALKING_FRONT);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_middle_body, 8, 96, PlayerAnimation::SIDE_MB_JUMPING);
	//Rows 13-15
	spsh.setSpriteColor(pal.get("leggings"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 0, PlayerAnimation::SIDE_LOWER_BODY);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 32, PlayerAnimation::SIDE_LOWER_BODY_B);
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_lower_body, 12, 64, PlayerAnimation::SIDE_LOWER_BODY_C);
	//Row 16
	spsh.setSpriteColor(pal.get("shoes"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_feet, 12, 0, PlayerAnimation::SIDE_FEET);
}

void SpriteSheetBuilder::buildSpriteSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
	switch(type) {
		case DEFAULT_PLAYER: buildPlayerSheet(type, spsh, pal); break;
	}
}
