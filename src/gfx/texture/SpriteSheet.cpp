#include "SpriteSheet.h"
#include <iostream>
#include <nch/cpp-utils/log.h>
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

    SpriteSheet::spriteWidth = spriteWidth;
    SpriteSheet::spriteHeight = spriteHeight;
  
    //If sdlTex isn't nullptr
    if(sdlTex!=nullptr) {
        //Query tex info
        int texAccess = 0;
        SDL_QueryTexture(sdlTex, NULL, &texAccess, NULL, NULL);

        //Check texture access
        if( texAccess==SDL_TEXTUREACCESS_TARGET || texAccess==SDL_TEXTUREACCESS_STATIC ) {
            loadTextureAsSpriteSheet(sdlTex);
        //If texAccess is something else...
        } else {
            nch::Log::error(__PRETTY_FUNCTION__, "Cannot init SpriteSheet textures with texture access other than SDL_TEXTUREACCESS_TARGET");
            nch::Log::throwException();
        }
    }
}

void SpriteSheet::init(SDLHandler* sh, SDL_Texture* sdlTex)
{
    init(sh, sdlTex, 32, 32);
}

void SpriteSheet::init(SDLHandler* sh, int sheetWidth, int sheetHeight)
{
    Texture temp;
    temp.init(sh, sheetWidth, sheetHeight);
    init(sh, temp.getSDLTexture());
}

void SpriteSheet::init(SDLHandler* sh)
{
    init(sh, nullptr);
}

void SpriteSheet::destroy()
{
    sheet.destroy();
}

std::string SpriteSheet::getInfo()
{
    std::stringstream ss;

    ss << "Sheet(width, height)=(" << getSheetWidth() << ", " << getSheetHeight() << "); ";
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
int SpriteSheet::getSheetWidth() { return sheet.getTexWidth(); } 
int SpriteSheet::getSheetHeight() { return sheet.getTexHeight(); } 

void SpriteSheet::setSpriteColorMod(const nch::Color& c)
{
    spriteColor = c;
}

void SpriteSheet::addSprite(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH, int spriteX, int spriteY)
{
    //Make sure size() of spritesThisRow matches the number of rows
    for(int i = spritesThisRow.size(); i<=spriteY; i++) {
        spritesThisRow.push_back(0);
    }

    //Expand sheet horizontally if needed
    int newWidth = (spriteX+1)*spriteWidth;
    if( newWidth>getSheetWidth() ) {
        sheet.setTexWidth( newWidth );
    }

    //spritesThisRow equals index of the last sprite in this row.
    //Thus, it can only increase or stay the same
    if( spriteX+1>spritesThisRow[spriteY] ) {
        spritesThisRow[spriteY] = spriteX+1;
    }

    //Expand sheet vertically if needed
    int newHeight = (spriteY+1)*spriteHeight;
    if( newHeight>getSheetHeight() ) {
        sheet.setTexHeight( newHeight );
    }

    //Blit image at desired location from desired source.
    sheet.setColorMod(spriteColor);
    sheet.lock( spriteX*spriteWidth, spriteY*spriteHeight, spriteWidth, spriteHeight );
    sheet.blit( src, srcX, srcY, srcW, srcH );
}

void SpriteSheet::addSprite(SDL_Texture* src, int srcX, int srcY, int spriteX, int spriteY)
{
    addSprite(src, srcX, srcY, spriteWidth, spriteHeight, spriteX, spriteY);
}

void SpriteSheet::addSpriteToRow(SDL_Texture* src, int srcX, int srcY, int spriteY)
{
    for(int i = spritesThisRow.size(); i<=spriteY; i++) {
        spritesThisRow.push_back(0);
    }
    addSprite(src, srcX, srcY, spritesThisRow[spriteY], spriteY);
}
void SpriteSheet::addSpriteToRow(int texID, int srcX, int srcY, int spriteY)
{
    addSpriteToRow(sheet.getTextureLoader()->getTexture(texID), srcX, srcY, spriteY);
}

/*
    Call addSpriteToRow 'num' times, with the image's srcX starting at 0, then 1*spriteWidth, 2*spriteWidth, ..., then finally (num-1)*spriteWidth.
    'srcY' and 'spriteY' stay constant throughout the 'num' calls.
*/
void SpriteSheet::addSpritesToRow(SDL_Texture* src, int num, int srcY, int spriteY)
{
    for( int i = 0; i<num; i++ ) {
        addSpriteToRow(src, i*spriteWidth, srcY, spriteY);
    }
}
void SpriteSheet::addSpritesToRow(int texID, int num, int srcY, int spriteY)
{
    addSpritesToRow(sheet.getTextureLoader()->getTexture(texID), num, srcY, spriteY);
}

void SpriteSheet::addSpriteToFixedSizeSheet(SDL_Texture* src, int srcX, int srcY)
{
    int spritesPerRow = getSheetWidth()/spriteWidth;
    int spritesPerCol = getSheetHeight()/spriteHeight;

    int i = 0;
    if(spritesThisRow.size()==0) {
        spritesThisRow.push_back(0);
    }

    while(spritesThisRow[i]==spritesPerRow) i++;
    if(i>=spritesPerCol) {
        nch::Log::warn(
            __PRETTY_FUNCTION__,
            "Spritesheet completely filled (%dx%d size of sprites & %dx%d size of sheet = %d total sprites)",
            spriteHeight, spriteWidth, getSheetWidth(), getSheetHeight(), spritesPerRow*spritesPerCol
        );
    } else {
        addSpriteToRow(src, srcX, srcY, i);
    }
}

void SpriteSheet::drawSheet()
{
    int mspr = getSheetWidth()/spriteWidth;     //[M]ax [S]prites [P]er [R]ow
    int mspc = getSheetWidth()/spriteHeight;    //... [P]er [C]olumn

    SDL_Rect dr;    //Full draw rect
    dr.x = sheet.getDrawX();
    dr.y = sheet.getDrawY();
    dr.w = sheet.getTexWidth()*sheet.getDrawScale();
    dr.h = sheet.getTexHeight()*sheet.getDrawScale();

    for(int ix = 0; ix<mspr; ix++)
    for(int iy = 0; iy<mspc; iy++) {
        double incX = dr.w/mspr;
        double incY = dr.w/mspc;
        
        if( iy>=spritesThisRow.size() || ix>=spritesThisRow[iy]) {
            if((ix+iy)%2) sdlHandler->setRenderDrawColor(nch::Color(0, 64, 0));
            else          sdlHandler->setRenderDrawColor(nch::Color(0, 0, 64));
        }

        sdlHandler->renderFillRect(dr.x+incX*ix, dr.y+incY*iy, incX, incY);
    }
    
    sheet.lock(0, 0, 16, 16);
    sheet.setColorMod(255, 0, 0);
    sheet.fill();

    sheet.setColorMod(255, 255, 255);
    sheet.draw();
}

void SpriteSheet::loadTextureAsSpriteSheet(SDL_Texture* sdlTex, int spriteWidth, int spriteHeight)
{
    int shw = 0; int shh = 0;
    SDL_QueryTexture(sdlTex, NULL, NULL, &shw, &shh);

    //If sheet dimensions incompatible with sprite dimensions
    if( shw%spriteWidth!=0 || shh%spriteHeight!=0 ) {
        std::stringstream ss;
        ss << "SpriteSheet dimensions (" << shw << ", " << shh << ") ";
        ss << "must be divisible by the dimensions of a single sprite (" << spriteWidth << ", " << spriteHeight << ")";
        nch::Log::warn(__PRETTY_FUNCTION__, ss.str(), "setting restrictSpriteSize to false");
        restrictingSpriteSize(false);
    }

    //Destroy old sheet texture
    sheet.destroy();

    //Build new sheet texture
    sheet.init(sdlHandler);
    sheet.setTexDimensions(shw, shh);
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
