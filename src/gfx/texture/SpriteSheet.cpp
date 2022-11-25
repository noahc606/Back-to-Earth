#include "SpriteSheet.h"
#include <iostream>
#include <sstream>

SpriteSheet::SpriteSheet(){}
SpriteSheet::~SpriteSheet(){}

void SpriteSheet::init(SDLHandler* sh)
{
    BTEObject::init(sh, nullptr, nullptr);
    sheet.init(sdlHandler);
    sheet.setDrawScale(2);

    src.x = 0; src.y = 0; src.w = 0; src.h = 0;
    dst.x = 0; dst.y = 0; dst.w = 0; dst.h = 0;

    setSpriteDimensions(32, 32);
}

void SpriteSheet::destroy()
{
    free(sheetPixels);
}

std::string SpriteSheet::getInfo()
{
    std::stringstream ss;

    ss << "Sheet(width, height)=(" << sheetWidth << ", " << sheetHeight << "); ";
    ss << "Sprites(width, height)=(" << spriteWidth << ", " << spriteHeight << "); ";
    ss << "\n";

    ss << "Number of rows: " << spritesThisRow.size() << "; ";
    ss << "Number of sprites per row: { ";
    for(int i = 0; i<(int)spritesThisRow.size(); i++) {
        ss << spritesThisRow[i] << " ";
    }
    ss << "}; ";
    ss << "\n";

    return ss.str();
}

Texture* SpriteSheet::getTexture() { return &sheet; }

void SpriteSheet::setSpriteDimensions(int w, int h)
{
    spriteWidth = w;
    spriteHeight = h;
}

void SpriteSheet::setSpriteColor(const Color& c)
{
    spriteColor = c;
}

void SpriteSheet::addSprite(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteX, int spriteY)
{
    //Make sure size() of spritesThisRow matches the number of rows
    for(int i = spritesThisRow.size(); i<=spriteY; i++) {
        spritesThisRow.push_back(0);
    }

    //Expand sheet horizontally if needed
    int newWidth = (spriteX+1)*spriteWidth;
    if( newWidth>sheetWidth ) {
        sheet.setTexWidth( newWidth );
    }

    //spritesThisRow equals index of the last sprite in this row.
    //Thus, it can only increase or stay the same
    if( spriteX+1>spritesThisRow[spriteY] ) {
        spritesThisRow[spriteY] = spriteX+1;
    }

    //Expand sheet vertically if needed
    int newHeight = (spriteY+1)*spriteHeight;
    if( newHeight>sheetHeight ) {
        sheet.setTexHeight( newHeight );
    }

    //Update sheetWidth and sheetHeight
    sheetWidth = sheet.getTexWidth();
    sheetHeight = sheet.getTexHeight();

    //Blit image at desired location from desired source.
    sheet.setColorMod(spriteColor);
    sheet.lock( spriteX*spriteWidth, spriteY*spriteHeight, spriteWidth, spriteHeight );
    sheet.blit( imgID, srcX, srcY, srcW, srcH );
}

void SpriteSheet::addSprite(int imgID, int srcX, int srcY, int spriteX, int spriteY)
{
    addSprite(imgID, srcX, srcY, spriteWidth, spriteHeight, spriteX, spriteY);
}

void SpriteSheet::addSpriteToRow(int imgID, int srcX, int srcY, int srcW, int srcH, int spriteY)
{
    for(int i = spritesThisRow.size(); i<=spriteY; i++) {
        spritesThisRow.push_back(0);
    }
    addSprite( imgID, srcX, srcY, srcW, srcH, spritesThisRow[spriteY], spriteY );
}

void SpriteSheet::addSpriteToRow(int imgID, int srcX, int srcY, int spriteY)
{
    for(int i = spritesThisRow.size(); i<=spriteY; i++) {
        spritesThisRow.push_back(0);
    }
    addSprite(imgID, srcX, srcY, spritesThisRow[spriteY], spriteY);
}

void SpriteSheet::addSpritesToRow(int imgID, int num, int srcY, int spriteY)
{
    for( int i = 0; i<num; i++ ) {
        addSpriteToRow( imgID, i*spriteWidth, srcY, spriteY );
    }
}

void SpriteSheet::drawSheet()
{
    sheet.draw();
}

/**
    Get ALL of the pixels in the 'sheet' Texture and store them in the raw pointer 'sheetPixels'
*/
void SpriteSheet::updateSheetPixels()
{
    //Get SDLHandler's SDL_Renderer* object
    SDL_Renderer* r = sdlHandler->getRenderer();

    //Free whatever exists in sheetPixels
    free(sheetPixels);

    //Store the current render target
    SDL_Texture* rtOld = SDL_GetRenderTarget( r );

    //Set render target to the Texture 'sheet' and read the pixels of 'sheet'.
    SDL_SetRenderTarget( r, sheet.getSDLTexture() );
    SDL_RenderReadPixels( r, NULL, sdlHandler->getPixelFormat()->format, (void*)sheetPixels, 4*sheet.getTexWidth() );

    //Set render target back to what it originally was
    SDL_SetRenderTarget( r, rtOld );
}
