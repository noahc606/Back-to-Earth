#include "SpriteSheet.h"
#include <iostream>
#include <nch/cpp-utils/io/Log.h>
#include <sstream>

SpriteSheet::SpriteSheet(){}
SpriteSheet::~SpriteSheet(){}

/*
    Initializes a SpriteSheet, which is a single texture holding multiple similar textures (usually for animation) in a grid formation.
    These individual textures are "sprites".

    Parameters:
    - sh:               sdlHandler object
    - sdlTex:           The individual Texture which will be used entirely as a spritesheet. If not specified, 'sheet' will start out as a 0x0 texture.
    - spriteWidth:      The width, in pixels, of an individual sprite. Defaults to 32 if not specified.
    - spriteHeight:     The height, in pixels, of an individual sprite. Defaults to 32 if not specified.
*/
void SpriteSheet::init(SDLHandler* sh, SDL_Texture* sdlTex, int spriteWidth, int spriteHeight)
{
    //Init 'sdlHandler' and Texture 'sheet'
    BTEObject::init(sh, nullptr, nullptr);
    sheet.init(sdlHandler);
    sheet.setDrawScale(2);

    //Reset src and dst rectangles to (0, 0, 0, 0)
    src.x = 0; src.y = 0; src.w = 0; src.h = 0;
    dst.x = 0; dst.y = 0; dst.w = 0; dst.h = 0;

    //If sdlTex is nullptr, the spritesheet will be initialized to 0x0.
    if( sdlTex==nullptr ) {
        sheetWidth = 0;
        sheetHeight = 0;
        setSpriteDimensions(spriteWidth, spriteHeight);
    //If sdlTex isn't nullptr
    } else {
        //Query tex info
        int texAccess = 0;
        SDL_QueryTexture(sdlTex, NULL, &texAccess, &sheetWidth, &sheetHeight);

        //Check texture access
        if( texAccess==SDL_TEXTUREACCESS_TARGET || texAccess==SDL_TEXTUREACCESS_STATIC ) {
            loadTextureAsSpriteSheet(sdlTex);
            updateSheetPixels();
        //If texAccess is something else...
        } else {
            NCH_Log::error(__PRETTY_FUNCTION__, "Cannot init SpriteSheet textures with texture access other than SDL_TEXTUREACCESS_TARGET");
            NCH_Log::throwException();
        }
    }
}

void SpriteSheet::init(SDLHandler* sh, int texID, int spriteWidth, int spriteHeight)
{
    BTEObject::init(sh, nullptr, nullptr);
    init(sh, sh->getTextureLoader()->getTexture(texID), spriteWidth, spriteHeight);
}

void SpriteSheet::init(SDLHandler* sh, SDL_Texture* sdlTex)
{
    init(sh, sdlTex, 32, 32);
}

void SpriteSheet::init(SDLHandler* sh, int texID)
{
    BTEObject::init(sh, nullptr, nullptr);
    init(sh, sh->getTextureLoader()->getTexture(texID), 32, 32);
}

void SpriteSheet::init(SDLHandler* sh)
{
    init(sh, nullptr);
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

Texture* SpriteSheet::getSheetTexture() { return &sheet; }

uint32_t** SpriteSheet::getSheetPixels() { return &sheetPixels; }

void SpriteSheet::setSpriteDimensions(int w, int h)
{
    spriteWidth = w;
    spriteHeight = h;
}

void SpriteSheet::setSpriteColor(const NCH_Color& c)
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

/*
    Call addSpriteToRow 'num' times, with the image's srcX starting at 0, then 1*spriteWidth, 2*spriteWidth, ..., then finally (num-1)*spriteWidth.
    'srcY' and 'spriteY' stay constant throughout the 'num' calls.
*/
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
    Get all of the pixels in 'sheet' and store them in the raw pointer 'sheetPixels'.
    Recommended to call this function once at the end of building a spritesheet.
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
    SDL_Rect rect; rect.x = 0; rect.y = 0; rect.w = sheet.getTexWidth(); rect.h = sheet.getTexHeight();

    sheetPixels = (uint32_t*)malloc(rect.w*rect.h*sizeof(*sheetPixels));
    SDL_RenderReadPixels( r, &rect, sdlHandler->getPixelFormat()->format, sheetPixels, 4*rect.w );

    //Set render target back to what it originally was
    SDL_SetRenderTarget( r, rtOld );
}

void SpriteSheet::loadTextureAsSpriteSheet(SDL_Texture* sdlTex, int spriteWidth, int spriteHeight)
{
    //If sheet dimensions incompatible with sprite dimensions
    if( sheetWidth%spriteWidth!=0 || sheetHeight%spriteHeight!=0 ) {
        std::stringstream ss;
        ss << "SpriteSheet dimensions (" << sheetWidth << ", " << sheetHeight << ") ";
        ss << "must be divisible by the dimensions of a single sprite (" << spriteWidth << ", " << spriteHeight << ")";
        NCH_Log::warn(__PRETTY_FUNCTION__, ss.str(), "setting restrictSpriteSize to false");
        restrictingSpriteSize(false);
    }

    //Destroy old sheet texture
    sheet.destroy();

    //Build new sheet texture
    sheet.init(sdlHandler);
    sheet.setTexDimensions(sheetWidth, sheetHeight);
    sheet.lock();
    sheet.blit(sdlTex);
}

void SpriteSheet::loadTextureAsSpriteSheet(SDL_Texture* sdlTex)
{
    loadTextureAsSpriteSheet(sdlTex, 32, 32);
}

void SpriteSheet::restrictingSpriteSize(bool rss)
{
    restrictSpriteSize = rss;
}
