#pragma once
#include <nch/cpp-utils/color.h>
#include <string>
#include <vector>
#include "BTEObject.h"
#include "Canvas.h"
#include "Texture.h"

class SpriteSheet : public BTEObject
{
public:
    SpriteSheet();
    virtual ~SpriteSheet();

    void init(SDLHandler* sh, SDL_Texture* sdlTex, int spriteWidth, int spriteHeight);
    void init(SDLHandler* sh, SDL_Texture* sdlTex);
    void init(SDLHandler* sh, int sheetWidth, int sheetHeight);
    void init(SDLHandler* sh);
    void destroy();

    std::string getInfo();
    Texture* getSheetTexture();
    int getSheetWidth();
    int getSheetHeight();

    void setSpriteColorMod(const nch::Color& c);

    void addSprite(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH, int spriteX, int spriteY);
    void addSprite(SDL_Texture* src, int srcX, int srcY, int spriteX, int spriteY);
    void addSpriteToRow(SDL_Texture* src, int srcX, int srcY, int spriteY);
    void addSpriteToRow(int texID, int srcX, int srcY, int spriteY);
    void addSpritesToRow(SDL_Texture* src, int num, int srcY, int spriteY);
    void addSpritesToRow(int texID, int num, int srcY, int spriteY);
    void addSpriteToFixedSizeSheet(SDL_Texture* src, int srcX, int srcY);

    void drawSheet();

    void loadTextureAsSpriteSheet(SDL_Texture* sdlTex, int spriteWidth, int spriteHeight);
    void loadTextureAsSpriteSheet(SDL_Texture* sdlTex);

    SDL_Rect src;
    SDL_Rect dst;

protected:

private:
    void restrictingSpriteSize(bool rss);

    int spriteWidth = 0; int spriteHeight = 0;
    nch::Color spriteColor = nch::Color(255, 255, 255);

    std::vector<int> spritesThisRow;

    bool restrictSpriteSize = true;

    Texture sheet;
};
