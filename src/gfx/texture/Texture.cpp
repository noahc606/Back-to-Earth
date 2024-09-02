#include "Texture.h"
#include <iostream>
#include <sstream>
#include "Log.h"

Texture::Texture()
{
    lockArea.x = 0; lockArea.y = 0; lockArea.w = 0; lockArea.h = 0;
    colorMod.set(255, 255, 255);
}
Texture::~Texture(){}

void Texture::init(SDLHandler* p_sdlHandler)
{
    //Helper class pointers
    sdlHandler = p_sdlHandler;
    pixelFormat = sdlHandler->getPixelFormat()->format;
    renderer = sdlHandler->getRenderer();
    textureLoader = sdlHandler->getTextureLoader();

    //Initialization
    initialized = true;
    initTex(false);
}

void Texture::init(SDLHandler* p_sdlHandler, int p_texWidth, int p_texHeight)
{
    init(p_sdlHandler);
    setTexDimensions(p_texWidth, p_texHeight);
}

void Texture::init(SDLHandler* p_sdlHandler, int p_texWidth, int p_texHeight, double p_texDrawScale)
{
    init(p_sdlHandler, p_texWidth, p_texHeight);
    setDrawScale(p_texDrawScale);
}

void Texture::destroy()
{
    //Certain variables set back to their default values
    initialized = false;
    texW = 0;
    texH = 0;

    //Set render target to "tex" and clear its pixels
    if(tex!=nullptr) {
        //Empty texture
        clear();

        //Destroy texture, set to nullptr.
        SDL_DestroyTexture(tex);
        tex = nullptr;
    }
}


bool Texture::isInitialized() { return initialized; }
TextureLoader* Texture::getTextureLoader() { return textureLoader; }
/**
    This function returns 'tex' directly which will be destroyed when
    this Texture object is destroyed.

    To get a copy of 'tex', use getSDLTextureCopy().
*/
SDL_Texture* Texture::getSDLTexture() { return tex; }
/**
    This function creates + returns a copy of 'tex' (SDL_Texture*) that will
    live indefinitely until you free it yourself using SDL_DestroyTexture().

    If you just need quick access to 'tex' (no new memory alloc), use getSDLTexture().
*/
SDL_Texture* Texture::getSDLTextureCopy()
{
    return SDL_CreateTextureFromSurface(renderer, createSurfaceFromTexture());
}
SDL_Surface* Texture::createSurfaceFromTexture()
{
    //Save render target
    SDL_Texture* rtOld = SDL_GetRenderTarget(renderer);
    //Change render target to this tex
    SDL_SetRenderTarget(renderer, tex);
    //Create a new surface
    SDL_Surface* surface = SDL_CreateRGBSurface(0, texW, texH, 32, 0, 0, 0, 0);
    //Copy render target's pixels to surface's pixels
    SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
    //Set back render target
    SDL_SetRenderTarget(renderer, rtOld);
    //Return surface
    return surface;
}

int Texture::getTexWidth() { return texW; }
int Texture::getTexHeight() { return texH; }
int Texture::getDrawX() { return drawX; }
int Texture::getDrawY() { return drawY; }
double Texture::getDrawScale() { return drawScale; }
SDL_BlendMode Texture::getBlendMode() { return blendMode; }

void Texture::setLock(int lx, int ly, int lw, int lh)
{
    lockArea.x = lx;
    lockArea.y = ly;
    lockArea.w = lw;
    lockArea.h = lh;
}

void Texture::lock(int x, int y, int w, int h)
{
    //Get the points of two opposite corners of the rectangle defined by the method.
    int x1 = x;
    int y1 = y;
    int x2 = x+w;
    int y2 = y+h;

    //Cut these points off at the texture's edge (texture's rectangle is @ (x,y,w,h)=(0,0,width,height).)
    if(x1<0) {
        x1 = 0;
    } else if( x1>texW) {
        x1 = texW;
    }
    if(x2<0) {
        x2 = 0;
    } else if(x2>texW) {
        x2 = texW;
    }
    if(y1<0) {
        y1 = 0;
    } else if( y1>texH) {
        y1 = texH;
    }
    if(y2<0) {
        y2 = 0;
    } else if( y2>texH) {
        y2 = texH;
    }

    //Set lock width, lock height, lock x, and lock y
    lockArea.w = abs(x1-x2);
    lockArea.h = abs(y1-y2);

    if(x1<x2) {
        lockArea.x = x1;
    } else {
        lockArea.x = x2;
    }

    if(y1<y2) {
        lockArea.y = y1;
    } else {
        lockArea.y = y2;
    }

    if( lockArea.x>=texW||lockArea.y>=texH ) {
        setLock(0, 0, 0, 0);
    }
}
void Texture::lock(SDL_Rect* rect)
{
    lock(rect->x, rect->y, rect->w, rect->h);
}
void Texture::lock()
{
    lock(0, 0, texW, texH);
}
void Texture::unlock() { lock(0, 0, 0, 0); }

void Texture::blitEx(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH, int mod)
{
    /** Don't allow invalid blitting areas */
    if( lockArea.w<=0 || lockArea.h<=0 )
        return;

    /** Set render target to texture */
    SDL_SetRenderTarget(renderer, tex);

    /** Set source and dst */
    //Source and destination rectangles for blit
    SDL_Rect sr; sr.x = srcX; sr.y = srcY; sr.w = srcW; sr.h = srcH;
    SDL_Rect dr; dr.x = lockArea.x; dr.y = lockArea.y; dr.w = lockArea.w; dr.h = lockArea.h;
    //Center of rotation should be center of blitting area
    SDL_Point cor; cor.x = lockArea.w/2; cor.y = lockArea.h/2;

    /** Set color, alpha, blending */
    SDL_SetTextureColorMod(src, colorMod.r, colorMod.g, colorMod.b);
    SDL_SetTextureAlphaMod(src, colorMod.a);
    SDL_SetTextureBlendMode(src, blendMode);

    /** Blit src onto tex. */

    if( mod==-1000 ) {
        SDL_RenderCopy( renderer, src, &sr, &dr );
    } else
    if( mod<360 ) {
        SDL_RenderCopyEx( renderer, src, &sr, &dr, mod, &cor, SDL_RendererFlip::SDL_FLIP_NONE );
    } else {
        switch( mod )
        {
            case Flip::H: {
                SDL_RenderCopyEx( renderer, src, &sr, &dr, 0.0, &cor, SDL_RendererFlip::SDL_FLIP_HORIZONTAL );
            } break;
            case Flip::V: {
                SDL_RenderCopyEx( renderer, src, &sr, &dr, 0.0, &cor, SDL_RendererFlip::SDL_FLIP_VERTICAL );
            } break;
            case Flip::HV: {
                SDL_RendererFlip hv = (SDL_RendererFlip) (SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
                SDL_RenderCopyEx( renderer, src, &sr, &dr, 0.0, &cor, hv );
            }
            default: {
                SDL_RenderCopy( renderer, src, &sr, &dr );
            } break;
        }
    }



    //Set render target back to screen
    SDL_SetRenderTarget(renderer, NULL);
}
void Texture::blitEx(Texture* src, int srcX, int srcY, int srcW, int srcH, int mod)
{
    SDL_Texture* stex = src->getSDLTexture();
    blitEx(stex, srcX, srcY, srcW, srcH, mod);
}
void Texture::blitEx(SDL_Texture* src, int srcX, int srcY, int mod)
{
    blitEx(src, srcX, srcY, lockArea.w, lockArea.h, mod);
}
void Texture::blitEx(Texture* src, int srcX, int srcY, int mod)
{
    SDL_Texture* stex = src->getSDLTexture();
    blitEx(stex, srcX, srcY, mod);
}
void Texture::blitEx(int id, int srcX, int srcY, int mod)
{
    blitEx(textureLoader->getTexture(id), srcX, srcY, mod);
}

void Texture::blit(SDL_Texture* src, int srcX, int srcY, int srcW, int srcH)
{
    blitEx(src, srcX, srcY, srcW, srcH, -1000);
}
void Texture::blit(Texture* src, int srcX, int srcY, int srcW, int srcH)
{
    SDL_Texture* stex = src->getSDLTexture();
    blit(stex, srcX, srcY, srcW, srcH);
}
void Texture::blit(int id, int srcX, int srcY, int srcW, int srcH)
{
    blit((textureLoader->getTexture(id)), srcX, srcY, srcW, srcH);
}
void Texture::blit(SDL_Texture* src, int srcX, int srcY)
{
    blit(src, srcX, srcY, lockArea.w, lockArea.h);
}
void Texture::blit(Texture* src, int srcX, int srcY)
{
    SDL_Texture* stex = src->getSDLTexture();
    blit(stex, srcX, srcY, lockArea.w, lockArea.h);
}
void Texture::blit(int id, int srcX, int srcY)
{
    blit((textureLoader->getTexture(id)), srcX, srcY);
}

void Texture::blit(SDL_Texture* src)
{
    blit(src, 0, 0);
}

void Texture::blit(int id)
{
    blit(textureLoader->getTexture(id));
}

/**
    Fill area based on lock settings and color mod settings.
*/
void Texture::fill(SDL_BlendMode newBlending)
{
    //Pixel's color at missing.png(23, 8) will always = (255, 255, 255, 255), making it perfect for color/alpha modding.
    //1x1 pixel can easily be stretched to a rectangle of any size.

    //Store blend setting
    SDL_BlendMode oldBlending = blendMode;

    //Set blend setting to NONE + blit
    setBlendMode(newBlending);
    blit(TextureLoader::missing, 23, 8, 1, 1);

    //Reset blend setting
    setBlendMode(oldBlending);
}
void Texture::fill()
{
    fill(SDL_BLENDMODE_NONE);
}

void Texture::rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm)
{
    //Store lock and color settings
    int lx = lockArea.x; int ly = lockArea.y; int lw = lockArea.w; int lh = lockArea.h;
    uint8_t lr = colorMod.r; uint8_t lg = colorMod.g; uint8_t lb = colorMod.b; uint8_t la = colorMod.a;

    //Set lock and color settings + blit rectangle
    lock(x, y, w, h);
    setColorMod(r, g, b, a);
    fill(bm);

    //Reset lock, color, and blend settings
    lockArea.x = lx; lockArea.y = ly; lockArea.w = lw; lockArea.h = lh;
    colorMod.r = lr; colorMod.g = lg; colorMod.b = lb;  colorMod.a = la;
}
void Texture::rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    rect(x, y, w, h, r, g, b, a, SDL_BLENDMODE_NONE);
}
void Texture::rect(int x, int y, int w, int h, uint8_t r, uint8_t g, uint8_t b)
{
    rect(x, y, w, h, r, g, b, 255);
}
void Texture::rect(int x, int y, int w, int h, nch::Color& c)
{
	rect(x, y, w, h, c.r, c.g, c.b, c.a);
}

void Texture::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a, SDL_BlendMode bm)
{
    rect(x, y, 1, 1, r, g, b, a, bm);
}
void Texture::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    rect(x, y, 1, 1, r, g, b, a);
}
void Texture::pixel(int x, int y, uint8_t r, uint8_t g, uint8_t b)
{
    pixel(x, y, r, g, b, 255);
}
void Texture::pixel(int x, int y, uint32_t rgba)
{
    nch::Color color(rgba);
    pixel(x, y, color.r, color.g, color.b, color.a);
}

void Texture::clear()
{
    //Set render target back to texture and clear
    SDL_SetRenderTarget(renderer, tex);
    SDL_RenderClear(renderer);
        //Set render target back to main screen
    SDL_SetRenderTarget(renderer, NULL);
}

void Texture::setColorMod()
{
    colorMod.r = 255;
    colorMod.g = 255;
    colorMod.b = 255;
    colorMod.a = 255;
}

void Texture::setColorMod(const nch::Color& c)
{
    colorMod.r = c.r;
    colorMod.g = c.g;
    colorMod.b = c.b;
    colorMod.a = c.a;
}
void Texture::setColorMod(uint8_t r, uint8_t g, uint8_t b) { setColorMod(nch::Color(r, g, b)); }
void Texture::setColorMod(uint8_t r, uint8_t g, uint8_t b, uint8_t a) { setColorMod(nch::Color(r, g, b, a)); }

void Texture::setBlendMode(SDL_BlendMode bm) { blendMode = bm; }

void Texture::setTexDimensions(int w, int h, bool scale)
{
    texW = w;
    texH = h;

    initTex(scale);
}
void Texture::setTexDimensions(int w, int h) { setTexDimensions(w, h, false); }
void Texture::setTexHeight(int h, bool scale) { setTexDimensions(texW, h, scale); }
void Texture::setTexHeight(int h) { setTexHeight(h, false); }
void Texture::setTexWidth(int w, bool scale) { setTexDimensions(w, texH, scale); }
void Texture::setTexWidth(int w) { setTexWidth(w, false); }

void Texture::setTexRect(int x, int y, int w, int h)
{
    drawX = x;
    drawY = y;
    texW = w;
    texH = h;
    initTex(false);
}
void Texture::setTexRect(SDL_Rect* rect)
{
    drawX = rect->x;
    drawY = rect->y;
    texW = rect->w;
    texH = rect->h;
    initTex(false);
}

void Texture::setDrawX(int x) { drawX = x; }
void Texture::setDrawY(int y) { drawY = y; }
void Texture::setDrawPos(int x, int y) { drawX = x; drawY = y; }
void Texture::setDrawScale(double s) { drawScale = s; }
void Texture::translate(int dx, int dy) { drawX += dx; drawY += dy; }
void Texture::dilate(double factor) { drawScale *= factor; }


void Texture::draw()
{
    SDL_SetRenderTarget(renderer, NULL);
    SDL_Rect dst; dst.x = drawX; dst.y = drawY; dst.w = texW*drawScale; dst.h = texH*drawScale;
    SDL_RenderCopy( renderer, tex, NULL, &dst );
}

void Texture::draw(SDL_Rect* src, SDL_Rect* dst)
{
    SDL_RenderCopy( renderer, tex, src, dst );
}

void Texture::savePNG(FileHandler* fh, std::string path)
{
    //Create surface from texture
    SDL_Surface* surf = createSurfaceFromTexture();

    //Save surface
    std::stringstream ss;
    ss << fh->getResourcePath() << path;
    IMG_SavePNG( surf, ss.str().c_str() );

    //Free surface
    SDL_FreeSurface(surf);
}

void Texture::initTex(bool scale)
{
    validateTexSize();

    switch(access) {
        case SDL_TEXTUREACCESS_TARGET: {
            /* Recreate 'tex' with new dimensions. */
            //If texW and texH are both positive
            if( texW>0 && texH>0 ) {
                //Create a 'texCopy' which has the exact same pixel data within a new size.
                SDL_Texture* texCopy = SDL_CreateTexture(renderer, pixelFormat, access, texW, texH);

                //Set render target to 'texCopy'
                SDL_SetRenderTarget(renderer, texCopy);
                //Clear pixels in 'texCopy' (might be some garbage left over within pixel data)
                SDL_RenderClear(renderer);

                if(scale) {
                    //Render the entire surface of 'tex' onto the entire surface of 'texCopy'. (effectively resizing the old image).
                    SDL_RenderCopy(renderer, tex, NULL, NULL);
                } else {
                    //Get the width and height of the last texture.
                    int lastTexW = 0; int lastTexH = 0;
                    SDL_QueryTexture(tex, nullptr, nullptr, &lastTexW, &lastTexH );
                    //Render the entire surface of tex onto part of 'texCopy' (effectively cropping the old image).
                    SDL_Rect lastRect; lastRect.x = 0; lastRect.y = 0; lastRect.w = lastTexW; lastRect.h = lastTexH;
                    SDL_RenderCopy(renderer, tex, &lastRect, &lastRect);
                }

                //Recreate the old texture 'tex'.
                {
                    //Destroy 'tex'
                    SDL_DestroyTexture(tex);
                    //Create 'tex' with new width and height
                    tex = SDL_CreateTexture(renderer, pixelFormat, access, texW, texH );

                    /* Check for errrors */
                    //If 'tex' is NULL
                    if(tex==NULL) {
                        nch::Log::error( __PRETTY_FUNCTION__, "texture is NULL", SDL_GetError() );
                    }
                    //Set BlendMode of 'tex' to SDL_BLENDMODE_BLEND
                    setBlendMode(SDL_BLENDMODE_BLEND);
                    if( SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND)==-1 ) {
                        nch::Log::error( __PRETTY_FUNCTION__, "Blend mode could not be set", SDL_GetError() );
                    }
                }

                //Set rendertarget to 'tex'
                SDL_SetRenderTarget(renderer, tex);
                //Clear pixels in 'tex' (might be some garbage left over within the pixel data)
                SDL_RenderClear(renderer);

                //Copy entire 'texCopy' onto entire tex.
                SDL_Rect rect; rect.x = 0; rect.y = 0; rect.w = texW; rect.h = texH;
                SDL_RenderCopy(renderer, texCopy, NULL, &rect);

                //Set Render Target back to main screen.
                SDL_SetRenderTarget(renderer, NULL);

                //Destroy 'texCopy'
                SDL_DestroyTexture(texCopy);
            }

            //If both texW and texH are nonpositive (invalid dimensions)
            if( texW<0 && texH<0 ) {
                //Destroy 'tex'
                destroy();
            }

            //Set render target back to NULL (main window)
            SDL_SetRenderTarget(renderer, NULL);
        } break;
        case SDL_TEXTUREACCESS_STREAMING: {
            tex = SDL_CreateTexture(sdlHandler->getRenderer(), sdlHandler->getPixelFormat()->format, SDL_TEXTUREACCESS_STREAMING, texW, texH);
            if(tex==NULL) {
                nch::Log::errorv(__PRETTY_FUNCTION__, SDL_GetError(), "Error creating streaming texture");
            }
        } break;
    }

    //Set lock settings to default
    validateLock();
}

void Texture::validateLock()
{
    //Modify the lock dimensions to fit the new texture, in case of shrinking one of the texture's dimensions.
    lock(lockArea.x, lockArea.y, lockArea.w, lockArea.h);
}
void Texture::validateTexSize()
{
    //Limit texture size
    if( limitTexSize ) {
        if( texW>texSizeLimit ) texW = texSizeLimit;
        if( texH>texSizeLimit ) texH = texSizeLimit;
    }
}
