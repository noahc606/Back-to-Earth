#include "SpriteSheetBuilder.h"
#include "Player.h"

SpriteSheetBuilder::SpriteSheetBuilder(SDLHandler* sh)
{
	sdlHandler = sh;
}

void SpriteSheetBuilder::buildPlayerSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
	spsh.init(sdlHandler);
	TextureLoader tl;
	PlayerAnimation pa;
	
/* TOP VIEW */
	//Row 1
	spsh.setSpriteColorMod(pal.get("hair"));
	spsh.addSpriteToRow(tl.PLAYER_top_hair, 0, 0, pa.TOP_HAIR);
	//Row 2
	spsh.setSpriteColorMod(pal.get("skin"));
	spsh.addSpriteToRow(tl.PLAYER_top_arms, 0, 0, pa.TOP_ARMS);
	//Row 3
	spsh.setSpriteColorMod(pal.get("shirt"));
	spsh.addSpriteToRow(tl.PLAYER_top_middle_body, 0, 0, pa.TOP_MIDDLE_BODY);
	//Rows 4 and 5
	spsh.setSpriteColorMod(pal.get("leggings"));
	spsh.addSpritesToRow(tl.PLAYER_top_lower_body, 12, 0, pa.TOP_LOWER_BODY);
	spsh.addSpritesToRow(tl.PLAYER_top_lower_body, 12, 32, pa.TOP_LOWER_BODY_B);
	//Row 6
	spsh.setSpriteColorMod(pal.get("hair"));
	spsh.addSpriteToRow(tl.PLAYER_side_hair, 0, 0, pa.SIDE_HAIR);
/* SIDE VIEW */
	//Row 7
	spsh.setSpriteColorMod(pal.get("skin"));
	spsh.addSpriteToRow(tl.PLAYER_side_head, 0, 0, pa.SIDE_HEAD_BASE);
	//Row 8
	spsh.setSpriteColorMod(nch::Color(255, 255, 255));
	spsh.addSpriteToRow(tl.PLAYER_side_head, 0, 32, pa.SIDE_HEAD_EYES);
	//Row 9
	spsh.setSpriteColorMod(pal.get("eyes"));
	spsh.addSpriteToRow(tl.PLAYER_side_head, 0, 64, pa.SIDE_HEAD_PUPILS);
	//Row 10
	spsh.setSpriteColorMod(pal.get("mouth"));
	spsh.addSpriteToRow(tl.PLAYER_side_head, 0, 96, pa.SIDE_HEAD_MOUTH);
	//Row 11
	spsh.setSpriteColorMod(pal.get("skin"));
	spsh.addSpritesToRow(TextureLoader::PLAYER_side_arms, 8, 0, pa.SIDE_ARMS);
	//Row 12
	spsh.setSpriteColorMod(pal.get("shirt"));
	spsh.addSpritesToRow(tl.PLAYER_side_middle_body, 8, 0, pa.SIDE_MB_TURNING);
	spsh.addSpritesToRow(tl.PLAYER_side_middle_body, 8, 32, pa.SIDE_MB_WALKING);
	spsh.addSpritesToRow(tl.PLAYER_side_middle_body, 8, 64, pa.SIDE_MB_WALKING_FRONT);
	spsh.addSpritesToRow(tl.PLAYER_side_middle_body, 8, 96, pa.SIDE_MB_JUMPING);
	//Rows 13-15
	spsh.setSpriteColorMod(pal.get("leggings"));
	spsh.addSpritesToRow(tl.PLAYER_side_lower_body, 12, 0, pa.SIDE_LOWER_BODY);
	spsh.addSpritesToRow(tl.PLAYER_side_lower_body, 12, 32, pa.SIDE_LOWER_BODY_B);
	spsh.addSpritesToRow(tl.PLAYER_side_lower_body, 12, 64, pa.SIDE_LOWER_BODY_C);
	//Row 16
	spsh.setSpriteColorMod(pal.get("shoes"));
	spsh.addSpritesToRow(tl.PLAYER_side_feet, 12, 0, pa.SIDE_FEET);
}

void SpriteSheetBuilder::buildSpriteSheet(int type, SpriteSheet& spsh, ColorPalette& pal)
{
	switch(type) {
		case DEFAULT_PLAYER: buildPlayerSheet(type, spsh, pal); break;
	}
}
