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

    void init(SDLHandler* sh);

    std::string getInfo();
    Texture* getTexture();

    void setSpriteDimensions(int w, int h);
    void setSpriteColor(const Color& c);

    void addSprite(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteX, int spriteY);
    void addSprite(int imgID, int srcX, int srcY, int spriteX, int spriteY);
    void addSpriteToRow(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteY);
    void addSpriteToRow(int imgID, int srcX, int srcY, int spriteY);
    void addSpritesToRow(int imgID, int num, int srcY, int spriteY);

    void drawSheet();

    SDL_Rect src;
    SDL_Rect dst;

protected:

private:
    int spriteWidth = 0; int spriteHeight = 0;
    Color spriteColor;

    int sheetWidth = 0; int sheetHeight = 0;
    std::vector<int> spritesThisRow;

    Texture sheet;


};
