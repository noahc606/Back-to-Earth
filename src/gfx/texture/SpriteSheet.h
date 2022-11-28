#pragma once
#include <string>
#include <vector>
#include "BTEObject.h"
#include "Canvas.h"
#include "Color.h"
#include "Texture.h"

class SpriteSheet : public BTEObject
{
public:
    SpriteSheet();
    virtual ~SpriteSheet();

    void init(SDLHandler* sh, SDL_Texture* sdlTex, int spriteWidth, int spriteHeight);
    void init(SDLHandler* sh, int texID, int spriteWidth, int spriteHeight);
    void init(SDLHandler* sh, SDL_Texture* sdlTex);
    void init(SDLHandler* sh, int texID);
    void init(SDLHandler* sh);
    void destroy();

    std::string getInfo();
    Texture* getSheetTexture();
    uint32_t** getSheetPixels();

    void setSpriteDimensions(int w, int h);
    void setSpriteColor(const Color& c);

    void addSprite(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteX, int spriteY);
    void addSprite(int imgID, int srcX, int srcY, int spriteX, int spriteY);
    void addSpriteToRow(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteY);
    void addSpriteToRow(int imgID, int srcX, int srcY, int spriteY);
    void addSpritesToRow(int imgID, int num, int srcY, int spriteY);

    void drawSheet();

    void loadTextureAsSpriteSheet(SDL_Texture* sdlTex, int spriteWidth, int spriteHeight);
    void loadTextureAsSpriteSheet(SDL_Texture* sdlTex);

    void updateSheetPixels();

    SDL_Rect src;
    SDL_Rect dst;

protected:

private:
    void restrictingSpriteSize(bool rss);

    int spriteWidth = 0; int spriteHeight = 0;
    Color spriteColor;

    int sheetWidth = 0; int sheetHeight = 0;
    std::vector<int> spritesThisRow;

    bool restrictSpriteSize = true;

    Texture sheet;
    uint32_t* sheetPixels = nullptr;
};
