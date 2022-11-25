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

    free(lockedPixels);
    if( SDL_LockTexture(tex, &lockArea, (void**)&lockedPixels, &lockedPitch)<0 ) {
        Log::error(__PRETTY_FUNCTION__, "Texture locking failed", SDL_GetError());
        Log::throwException();
    }

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
void STexture::lock()
{

}

void STexture::unlock()
{
    SDL_UnlockTexture(tex);
}

void STexture::blit()
{
    //Create pixels to update the texture with
    lockedPixels = (uint32_t*)malloc(lockArea.w*lockArea.h*sizeof(*lockedPixels));
    for( int py = 0; py<lockArea.h; py++ ) {
        uint32_t* row = lockedPixels+py*lockArea.w;
        for ( int px = 0; px<lockArea.w; px++ ) {
            if( py%2==0 ) {
                row[px] = 0x0000FFFF;
            } else {
                row[px] = 0xFF0000FF;
            }
        }
    }
}

void STexture::update(SDL_Rect ur)
{
    SDL_UpdateTexture(tex, &ur, lockedPixels, lockArea.w*sizeof(*lockedPixels));
}
void STexture::update()
{
    update(lockArea);
}
