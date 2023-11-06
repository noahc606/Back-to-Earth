#pragma once
#include "ColorPalette.h"
#include "SDLHandler.h"
#include "SpriteSheet.h"

class SpriteSheetBuilder
{
public:
    SpriteSheetBuilder(SDLHandler* sh);

    void buildPlayerSheet(int type, SpriteSheet& spsh, ColorPalette& colors);
    void buildSpriteSheet(int type, SpriteSheet& spsh, ColorPalette& colors);

    enum SpriteSheets {
        DEFAULT_PLAYER,
    };
protected:

private:
    SDLHandler* sdlHandler;



};
