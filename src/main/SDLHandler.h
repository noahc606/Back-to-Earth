#pragma once
#include <SDL.h>
#include <string>
#include "AudioLoader.h"
#include "Color.h"
#include "Settings.h"
#include "TextureLoader.h"

class SDLHandler
{
public:
    /**/
    SDLHandler();
    void init();
    virtual ~SDLHandler();

    /**/
    void trackEvents(SDL_Event e);

    /**/
    //Get window and window peripherals
    SDL_Window* getWindow();
    SDL_PixelFormat* getPixelFormat();
    SDL_Renderer* getRenderer();
    //Get window properties
    int getWidth();
    int getHeight();
    //Get Resources
    std::string getResourcePath();
    TextureLoader* getTextureLoader();
    AudioLoader* getAudioLoader();

    /**/
    void toggleFullScreen();
    void renderCopy(int id, SDL_Rect* src, SDL_Rect* dst);
    void renderCopy(int id, SDL_Rect* src, SDL_Rect* dst, double angle, SDL_Point* cor);
    void setColorMod(int id, const Color& c);
    void setColorMod(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void setColorMod(int id, uint8_t r, uint8_t g, uint8_t b);

protected:

private:
    /**/
    void createSubsystems();
    void createWindowAndRenderer();

    /**/
    /* Resources */
    //Resource Path
    std::string resourcePath = "";
    //Texture Loader
    TextureLoader textureLoader;
    //Audio Loader
    AudioLoader audioLoader;

    /* Window */
    //Window, window pixel format, window renderer
    SDL_Window* window = nullptr;
    SDL_PixelFormat* windowPixelFormat = nullptr;
    SDL_Renderer* windowRenderer = nullptr;
    //Window properties
    bool fullscreen = false;
    int lastDisplayID = 0;
    int width = 0; int maxWidth = 640;
    int height = 0; int maxHeight = 480;
};
