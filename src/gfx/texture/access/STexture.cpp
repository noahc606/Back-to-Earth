#include "STexture.h"
#include "Log.h"
#include "Timer.h"

STexture::STexture(){}
STexture::~STexture(){}

void STexture::init(SDLHandler* p_sdlHandler)
{
    access = SDL_TEXTUREACCESS_STREAMING;
    Texture::init(p_sdlHandler);
}

void STexture::lock(SDL_Rect lr)
{
    //Perform lock and validate it
    Texture::lock(lr.x, lr.y, lr.w, lr.h);

    free(dstPixels);
    if( SDL_LockTexture(tex, &lockArea, (void**)&dstPixels, &dstPitch)<0 ) {
        Log::error(__PRETTY_FUNCTION__, "Texture locking failed", SDL_GetError());
        Log::throwException();
    }

    dstPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*dstPixels));

    /*


#include <SDL2/SDL.h>
#include <sys/time.h>

static double time_in_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Pixel formats" , 0, 0, 1920, 1080, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    double last_frame = time_in_ms();
    while (1) {
        int width, height;
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                return 0;
            }
        }
        SDL_GetWindowSize(window, &width, &height);
        SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);


        Uint32 *pixels = malloc(width * height * sizeof(*pixels));
        //fill buffer with blue pixels
        for (int y = 0; y < height; y++) {
            Uint32 *row = pixels + y * width;
            for (int x = 0; x < width; x++) {
                row[x] = 0x0000FFFF;
            }
        }

        double update_begin = time_in_ms();
        SDL_UpdateTexture(texture, NULL, pixels, width * sizeof(*pixels));
        double update_end = time_in_ms();
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_NONE);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
        free(pixels);
        double this_frame = time_in_ms();
        printf("frame took %fms\n", this_frame - last_frame);
        printf(" - update texture: %fms\n", update_end - update_begin);
        last_frame = this_frame;
    }

}


    */

}

void STexture::lock(int lx, int ly, int lw, int lh)
{
    SDL_Rect lr;
    lr.x = lx; lr.y = ly; lr.w = lw; lr.h = lh;

    lock(lr);
}
void STexture::lock() { lock(0, 0, texW, texH); }

void STexture::unlock()
{
    SDL_UnlockTexture(tex);
}

/**
    Scaled blit.

    If srcW==lockArea.w and srcH==lockArea.h:
        This is slower than scaleless blitting to achieve the same result, but you should be fine in most cases.
*/
void STexture::sblit(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH)
{
    switch( blendMode ) {
        case SDL_BLENDMODE_ADD:     { sblitA(src, srcX, srcY, srcW, srcH); } break;
        case SDL_BLENDMODE_BLEND:   { sblitB(src, srcX, srcY, srcW, srcH); } break;
        case SDL_BLENDMODE_MOD:     { sblitM(src, srcX, srcY, srcW, srcH); } break;
        default:                    { sblitN(src, srcX, srcY, srcW, srcH); } break;
    }
}
void STexture::sblitA(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    float scW = (float)srcW/(float)lockArea.w;
    float scH = (float)srcH/(float)lockArea.h;

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+(int)((srcY+py)*scW)*spshW;
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel%dstPixel
            Color srcColor( srcRow[ (int)((srcX+px)*scH) ] );
            srcColor.add(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::sblitB(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    float scW = (float)srcW/(float)lockArea.w;
    float scH = (float)srcH/(float)lockArea.h;

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+(int)((srcY+py)*scW)*spshW;
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel%dstPixel
            Color srcColor( srcRow[ (int)((srcX+px)*scH) ] );
            srcColor.blend(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::sblitM(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    float scW = (float)srcW/(float)lockArea.w;
    float scH = (float)srcH/(float)lockArea.h;

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+(int)((srcY+py)*scW)*spshW;
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel%dstPixel
            Color srcColor( srcRow[ (int)((srcX+px)*scH) ] );
            srcColor.mod(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::sblitN(SpriteSheet* src, int srcX, int srcY, int srcW, int srcH)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    float scW = (float)srcW/(float)lockArea.w;
    float scH = (float)srcH/(float)lockArea.h;

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+(int)((srcY+py)*scW)*spshW;
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel
            dstRow[px] = srcRow[ (int)((srcX+px)*scH) ];
        }
    }
}

/**
    General purpose no scaling blit - has error checking.
    Cannot crash program and only a tiny bit slower than fblit.
*/
void STexture::blit(SpriteSheet* src, int srcX, int srcY)
{
    //Error checking
    //If operation would crash do a normal scaleless blit that is a little slower than fblit
    if( srcX+lockArea.w>src->getSheetTexture()->getTexWidth() || srcY+lockArea.y>src->getSheetTexture()->getTexHeight() ) {
        switch( blendMode ) {
            case SDL_BLENDMODE_ADD:     { blitA(src, srcX, srcY); } break;
            case SDL_BLENDMODE_BLEND:   { blitB(src, srcX, srcY); } break;
            case SDL_BLENDMODE_MOD:     { blitM(src, srcX, srcY); } break;
            default:                    { blitN(src, srcX, srcY); } break;
        }
    } else {
        //Else, do fast blit
        fblit(src, srcX, srcY);
    }
}
void STexture::blit(SpriteSheet* src) { blit(src, 0, 0); }
void STexture::blitA(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();
    int spshH = src->getSheetTexture()->getTexHeight();

    for( int py = 0; py<lockArea.h&&srcY+py<spshH; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((int)(srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w && srcX+px<spshW; px++ ) {
            //FORMULA: dstPixel = srcPixel+dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.add(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::blitB(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();
    int spshH = src->getSheetTexture()->getTexHeight();

    for( int py = 0; py<lockArea.h&&srcY+py<spshH; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((int)(srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w && srcX+px<spshW; px++ ) {
            //FORMULA: dstPixel = srcPixel*dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.blend(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::blitM(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();
    int spshH = src->getSheetTexture()->getTexHeight();

    for( int py = 0; py<lockArea.h&&srcY+py<spshH; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((int)(srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w && srcX+px<spshW; px++ ) {
            //FORMULA: dstPixel = srcPixel%dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.mod(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::blitN(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();
    int spshH = src->getSheetTexture()->getTexHeight();

    for( int py = 0; py<lockArea.h&&srcY+py<spshH; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((int)(srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w && srcX+px<spshW; px++ ) {
            //FORMULA: dstPixel = srcPixel
            dstRow[px] = srcRow[srcX+px];
        }
    }
}

/**
    Fast blit - no scaling math, no error checking.
    Can crash program (invalid array access) if srcX+lockArea.w>srcW || srcY+lockArea.y>srcH.
*/
void STexture::fblit(SpriteSheet* src, int srcX, int srcY)
{
    switch( blendMode ) {
        case SDL_BLENDMODE_ADD:     { fblitA(src, srcX, srcY); } break;
        case SDL_BLENDMODE_BLEND:   { fblitB(src, srcX, srcY); } break;
        case SDL_BLENDMODE_MOD:     { fblitM(src, srcX, srcY); } break;
        default:                    { fblitN(src, srcX, srcY); } break;
    }
}
void STexture::fblit(SpriteSheet* src) { fblit(src, 0, 0); }
void STexture::fblitA(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel+dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.add(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::fblitB(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel*dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.blend(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::fblitM(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel%dstPixel
            Color srcColor( srcRow[srcX+px] );
            srcColor.mod(colorMod);
            dstRow[px] = srcColor.getRGBA();
        }
    }
}
void STexture::fblitN(SpriteSheet* src, int srcX, int srcY)
{
    uint32_t* srcPixels = *src->getSheetPixels();
    int spshW = src->getSheetTexture()->getTexWidth();

    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* dstRow = dstPixels+(py*lockArea.w);
        uint32_t* srcRow = srcPixels+((srcY+py)*spshW);
        for ( int px = 0; px<lockArea.w; px++ ) {
            //FORMULA: dstPixel = srcPixel
            dstRow[px] = srcRow[srcX+px];
        }
    }
}

void STexture::blitRedAndBlue()
{
    //Create pixels to update the texture with
    dstPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*dstPixels));
    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* row = dstPixels+py*lockArea.w;
        for ( int px = 0; px<lockArea.w; px++ ) {
            if( py%2==0 ) {
                row[px] = 0x0000FFFF;
            } else {
                row[px] = 0xFF0000FF;
            }
        }
    }
}



void STexture::fill(uint32_t rgba)
{
    //Create pixels to update the texture with
    dstPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*dstPixels));
    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* row = dstPixels+py*lockArea.w;
        for ( int px = 0; px<lockArea.w; px++ ) {
            row[px] = rgba;
        }
    }
}

void STexture::fill(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    fill( Color::getRGBA(r, g, b, a) );
}

void STexture::fill(uint8_t r, uint8_t g, uint8_t b)
{
    fill( Color::getRGBA(r, g, b) );
}

void STexture::update(SDL_Rect ur)
{
    SDL_UpdateTexture(tex, &ur, dstPixels, lockArea.w*sizeof(*dstPixels));
}
void STexture::update()
{
    update(lockArea);
}

/*
void STexture::allocDestinationPixels()
{
    dstPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*dstPixels));
}
*/
