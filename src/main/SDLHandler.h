#pragma once
#include <SDL2/SDL.h>
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
    void preinit();
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
    bool isFullScreen();
	int getDisplayRefreshRate();
    int getWidth();
    int getHeight();
    bool usingBTECursor();
    //Get Resources
    std::string getVideoDriversDesc();
    std::string getSystemRamDesc();
    std::string getResourcePath();
    std::string getDevicePlatform();
    int getFilesystemType();
    TextureLoader* getTextureLoader();
    AudioLoader* getAudioLoader();
    //Operating system types
    enum Platforms {
        UNKNOWN = -10,
        UNSET = 0,
        WINDOWS = 1,
        LINUX = 2,
    };

    /**/
    void toggleFullScreen();
    void toggleBTECursor();
    
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
    void validateDevicePlatform();
    void setVideoDriversDesc();

    /**/
    /* Resources */
    // Video drivers description
    std::string videoDriversDesc;
    // Resource Path
    std::string resourcePath = "";
    // Operating system
    std::string devicePlatform = "";
    int filesystemType = UNSET;
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
    int width = 0; int maxWidth = 640;
    int height = 0; int maxHeight = 480;
    bool bteCursor = false;
    int lastDisplayID = 0;

    /* Game Controller/Joystick */
    SDL_Joystick* joystick = nullptr;

};
