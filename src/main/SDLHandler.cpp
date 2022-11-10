#include "SDLHandler.h"
#include <SDL_image.h>
#include <SDL_mixer.h>
#include "Log.h"
#include "Main.h"

SDLHandler::SDLHandler(){}
void SDLHandler::init()
{
    /* Init SDL */
    //Create Subsystems
    createSubsystems();
    //Create window and renderer
    createWindowAndRenderer();

    /* Create resource loaders and set window icon */
    textureLoader.init(windowRenderer, windowPixelFormat, resourcePath);
    audioLoader.init(resourcePath);
    SDL_SetWindowIcon(window, textureLoader.getSurface(TextureLoader::icon));



    /*
    std::stringstream ss;
    ss << SDL_GetCurrentVideoDriver() << "\n";
    ss << SDL_GetNumVideoDrivers() << "\n";
    ss << SDL_GetVideoDriver(0) << ", " << SDL_GetVideoDriver(1) << "\n";
    std::cout << ss.str() << "\n";
    */
}
SDLHandler::~SDLHandler()
{
    //Delete things
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(windowRenderer);
    SDL_FreeFormat(windowPixelFormat);
}

void SDLHandler::trackEvents(SDL_Event e)
{

    SDL_GetWindowSize(window, &width, &height);
}

SDL_Window* SDLHandler::getWindow() { return window; }
SDL_PixelFormat* SDLHandler::getPixelFormat() { return windowPixelFormat; }
SDL_Renderer* SDLHandler::getRenderer() { return windowRenderer; }
int SDLHandler::getWidth() { return width; }
int SDLHandler::getHeight() { return height; }
std::string SDLHandler::getResourcePath() { return resourcePath; }
TextureLoader* SDLHandler::getTextureLoader() { return &textureLoader; }
AudioLoader* SDLHandler::getAudioLoader() { return &audioLoader; }

void SDLHandler::toggleFullScreen()
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Toggling fullscreen");

    if(fullscreen) {
        fullscreen = false;

        if ( SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)!=0 ) {
            SDL_SetWindowPosition(window, maxWidth/2, maxHeight/2 );
            SDL_SetWindowSize(window, SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID), SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID));
            Log::error(__PRETTY_FUNCTION__, "Failed to turn fullscreen off", SDL_GetError());
        }
    } else {
        fullscreen = true;

        if( SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)!=0 ) {
            Log::error(__PRETTY_FUNCTION__, "Failed to turn fullscreen on", SDL_GetError());
        }
    }

    lastDisplayID = SDL_GetWindowDisplayIndex(window);
}

void SDLHandler::renderCopy(int id, SDL_Rect* src, SDL_Rect* dst)
{
    SDL_RenderCopy(windowRenderer, textureLoader.getTexture(id), src, dst );
}

void SDLHandler::renderCopy(int id, SDL_Rect* src, SDL_Rect* dst, double angle, SDL_Point* cor)
{
    SDL_RenderCopyEx(windowRenderer, textureLoader.getTexture(id), src, dst, angle, cor, SDL_FLIP_NONE);
}

void SDLHandler::setColorMod(int id, const Color& c)
{
    SDL_SetTextureColorMod( textureLoader.getTexture(id), c.r, c.g, c.b );
    SDL_SetTextureAlphaMod( textureLoader.getTexture(id), c.a );
}

void SDLHandler::setColorMod(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    setColorMod( id, Color(r, g, b, a) );
}

void SDLHandler::setColorMod(int id, uint8_t r, uint8_t g, uint8_t b)
{
    setColorMod(id, r, g, b, 255);
}

void SDLHandler::createSubsystems()
{
    uint32_t flags = 0;

    /* Init SDL subsystems */
    //Init SDL
    flags = SDL_INIT_AUDIO|SDL_INIT_VIDEO;
    if( SDL_Init(flags)!=0 ) {
        Log::error( __PRETTY_FUNCTION__, "Failed to SDL_Init()!", SDL_GetError() );
    }
    //Init SDL_img
    flags = IMG_INIT_PNG;
    if( (IMG_Init(flags)&flags)!=flags ) {
        Log::error( __PRETTY_FUNCTION__, "Failed to IMG_Init()!", IMG_GetError() );
    }
    //Init SDL_mixer
    flags = MIX_INIT_MP3;
    if( (Mix_Init(flags)&flags)!=flags ) {
        Log::error( __PRETTY_FUNCTION__, "Failed to Mix_Init()!", Mix_GetError() );
    }

    /* Set Hints */
    //Render driver must be opengl rather than Windows' default of direct3d.
    //This is done to prevent graphical issues when rendering to SDL_TEXTUREACCESS_TARGET textures.
    if(true) {
        if( SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl")==SDL_FALSE ) {
            Log::error( __PRETTY_FUNCTION__, "Failed to set hint", SDL_GetError() );
        }
    }

    /* Get resource path of application */
    //If this function somehow fails, the application should not continue running.
    const char* path = SDL_GetBasePath();
    if( path==NULL ) {
        Log::error(__PRETTY_FUNCTION__, "Failed to get resource path", "SDL_GetBasePath()==NULL");
        Log::throwException();
    } else {
        resourcePath = (std::string)path+"backtoearth\\";
        Log::trbshoot(__PRETTY_FUNCTION__, "Got path as: "+resourcePath);
    }
    delete path;

    /* Get display information */
    SDL_DisplayMode dm;
    if( SDL_GetCurrentDisplayMode(0, &dm)!=0 ) {
        std::stringstream ss;
        ss << "Failed to get screen resolution, defaulting to " << maxWidth << "x" << maxHeight;
        Log::error(__PRETTY_FUNCTION__, ss.str(), SDL_GetError());
    } else {
        maxWidth = dm.w;
        maxHeight = dm.h;

        //print out screen resolution
        std::stringstream ss;
        ss << "Got screen size as: " << maxWidth << "x" << maxHeight << ".";
        Log::trbshoot(__PRETTY_FUNCTION__, ss.str());
    }
}

void SDLHandler::createWindowAndRenderer()
{
    /* Attempt to init Window */
    //If window should be fullscreen
    if(fullscreen) {
        //Create fullscreen window that is positioned in the display with id 'lastDisplayID.'
        window = SDL_CreateWindow(
                                  Main::VERSION.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  0, 0,
                                  SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                                  );
    //If window should not be fullscreen
    } else {
        //Create resizable window that is resizable and positioned in the display with id 'lastDisplayID.'
        window = SDL_CreateWindow(
                                  Main::VERSION.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  maxWidth/2, maxHeight/2,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                                  );
    }

    //If window failed to be created, the program should stop
    if( window==NULL ) {
        //Print error, stop program
        Log::error(__PRETTY_FUNCTION__, "Window is null");
        Log::throwException();
    }

    //Initialize width and height.
    SDL_GetWindowSize(window, &width, &height);

    //Attempt to init renderer
    uint32_t rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    windowRenderer = SDL_CreateRenderer(window, -1, rendererFlags);
    //If renderer failed to be created, the program should stop
    if( windowRenderer==NULL ) {
        //Print error, stop program.
        Log::error(__PRETTY_FUNCTION__, "Renderer is null");
        Log::throwException();
    }


    //Allocate pixel format to be used by most textures
    SDL_FreeFormat(windowPixelFormat);
    windowPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    //If pixel format failed to be allocated, the program should stop
    if( windowPixelFormat==NULL ) {
        //Print error, stop program.
        Log::error(__PRETTY_FUNCTION__, "Pixel format is null");
        Log::throwException();
    }
}
