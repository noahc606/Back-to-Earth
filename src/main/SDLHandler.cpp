#include "SDLHandler.h"
#include <nch/cpp-utils/log.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_net.h>
#include "CurlHandler.h"
#include "Log.h"
#include "Main.h"

SDLHandler::SDLHandler(){}

void SDLHandler::preinit()
{
    /* Init SDL Subsystems */
    //Create Subsystems
    createSubsystems();
}

void SDLHandler::init()
{
    /* Init SDL Video and assets */
    //Create window and renderer
    createWindowAndRenderer();
	
    //Store information about video drivers
    setVideoDriversDesc();

	//Create asset loaders
    textureLoader.init(windowRenderer, windowPixelFormat, resourcePath);
    audioLoader.init(resourcePath);
    
	//Set window icon
    SDL_SetWindowIcon(window, textureLoader.getSurface(TextureLoader::icon));
}
SDLHandler::~SDLHandler()
{
    //Delete things
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(windowRenderer);
    SDL_FreeFormat(windowPixelFormat);
	
	//Quit
	IMG_Quit();
	Mix_Quit();
	SDLNet_Quit();
	
	SDL_Quit();
}

void SDLHandler::trackEvents(SDL_Event e)
{

    SDL_GetWindowSize(window, &width, &height);
}


SDL_Window* SDLHandler::getWindow() { return window; }
SDL_PixelFormat* SDLHandler::getPixelFormat() { return windowPixelFormat; }
SDL_Renderer* SDLHandler::getRenderer() { return windowRenderer; }

bool SDLHandler::isFullScreen() { return fullscreen; }
int SDLHandler::getDisplayRefreshRate()
{
	int thisDisplayId = 0;
	int numDisplayModes = SDL_GetNumDisplayModes(thisDisplayId);
	SDL_DisplayMode thisDisplayMode;
	int fallback = 60;
	
	if( numDisplayModes<1 ) {
		nch::Log::error(__PRETTY_FUNCTION__, "SDL_GetNumDisplayModes() failed: ", SDL_GetError());
		return fallback;
	}
	
	for( int i = 0; i<numDisplayModes; i++ ) {
		if( SDL_GetDisplayMode(thisDisplayId, i, &thisDisplayMode)!=0 ) {
			nch::Log::error(__PRETTY_FUNCTION__, "SDL_GetDisplayMode() failed: ", SDL_GetError());
			return fallback;
		}
	}
	
	int res = thisDisplayMode.refresh_rate;
	if( res!=0 ) {
		return res;
	} else {
		return fallback;
	}
}
int SDLHandler::getWidth() { return width; }
int SDLHandler::getHeight() { return height; }
bool SDLHandler::usingBTECursor() { return bteCursor; }

std::string SDLHandler::getVideoDriversDesc() { return videoDriversDesc; }
std::string SDLHandler::getSystemRamDesc() {
	std::stringstream ss;
	ss << "System RAM available: " << SDL_GetSystemRAM() << "MB";
	return ss.str();
}
std::string SDLHandler::getResourcePath() { return resourcePath; }
std::string SDLHandler::getDevicePlatform() { return devicePlatform; }
int SDLHandler::getFilesystemType() { return filesystemType; }
TextureLoader* SDLHandler::getTextureLoader() { return &textureLoader; }
AudioLoader* SDLHandler::getAudioLoader() { return &audioLoader; }

void SDLHandler::toggleFullScreen()
{
    nch::Log::debug(__PRETTY_FUNCTION__, "Toggling fullscreen...");

    if( fullscreen ) {
        fullscreen = false;
	
        if ( SDL_SetWindowFullscreen(window, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)!=0 ) {
            nch::Log::error(__PRETTY_FUNCTION__, "Failed to turn fullscreen off", SDL_GetError());
			SDL_SetWindowPosition(window, maxWidth/2, maxHeight/2 );
			SDL_SetWindowSize(window, SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID), SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID));
        } else {
			SDL_SetWindowPosition(window, maxWidth/4, maxHeight/4 );
			SDL_SetWindowSize(window, maxWidth/2, maxHeight/2);
		}
		
		SDL_SetWindowResizable(window, SDL_TRUE);
    } else {
        fullscreen = true;

        if( SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN)!=0 ) {
            nch::Log::error(__PRETTY_FUNCTION__, "Failed to turn fullscreen on", SDL_GetError());
        }
		SDL_SetWindowResizable(window, SDL_FALSE);
    }

    lastDisplayID = SDL_GetWindowDisplayIndex(window);
}

void SDLHandler::toggleBTECursor()
{
	nch::Log::debug(__PRETTY_FUNCTION__, "Toggling BTE cursor...");
	
	if(bteCursor) {
		SDL_ShowCursor(SDL_ENABLE);
		bteCursor = false;
	} else {
		SDL_ShowCursor(SDL_DISABLE);
		bteCursor = true;
	}
}

void SDLHandler::renderCopy(int id, SDL_Rect* src, SDL_Rect* dst)
{
    SDL_RenderCopy(windowRenderer, textureLoader.getTexture(id), src, dst );
}
void SDLHandler::renderCopy(int id, SDL_Rect* src, SDL_Rect* dst, double angle, SDL_Point* cor)
{
    SDL_RenderCopyEx(windowRenderer, textureLoader.getTexture(id), src, dst, angle, cor, SDL_FLIP_NONE);
}
void SDLHandler::renderFillRect(SDL_Rect* dst)
{
    SDL_RenderFillRect(windowRenderer, dst);
}
void SDLHandler::renderFillRect(int x, int y, int w, int h)
{
    SDL_Rect dst;
    dst.x = x; dst.y = y; dst.w = w; dst.h = h;
    SDL_RenderFillRect(windowRenderer, &dst);
}
void SDLHandler::setColorMod(int id, const nch::Color& c)
{
    SDL_SetTextureColorMod( textureLoader.getTexture(id), c.r, c.g, c.b );
    SDL_SetTextureAlphaMod( textureLoader.getTexture(id), c.a );
}
void SDLHandler::setColorMod(int id, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    setColorMod( id, nch::Color(r, g, b, a) );
}
void SDLHandler::setColorMod(int id, uint8_t r, uint8_t g, uint8_t b)
{
    setColorMod(id, r, g, b, 255);
}
void SDLHandler::setRenderDrawColor(const nch::Color& c)
{
    SDL_SetRenderDrawColor(windowRenderer, c.r, c.g, c.b, c.a);
}

void SDLHandler::createSubsystems()
{
    uint32_t flags = 0;

    /* Init SDL subsystems */
    //Init SDL
    flags = SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER;
    if( SDL_Init(flags)!=0 ) {
        nch::Log::error( __PRETTY_FUNCTION__, "Failed to SDL_Init()!", SDL_GetError() );
    }
	
    //Init SDL_img
    flags = IMG_INIT_PNG;
    if( (IMG_Init(flags)&flags)!=flags ) {
        nch::Log::error( __PRETTY_FUNCTION__, "Failed to IMG_Init()!", IMG_GetError() );
    }
	
    //Init SDL_mixer
    flags = MIX_INIT_MP3 | MIX_INIT_OGG;
    if( (Mix_Init(flags)&flags)!=flags ) {
        nch::Log::error( __PRETTY_FUNCTION__, "Failed to Mix_Init()!", Mix_GetError() );
    }
	
	//Init SDL_net
	if( SDLNet_Init()==-1 ) {
		nch::Log::error( __PRETTY_FUNCTION__, "Failed to SDLNet_Init()!", SDLNet_GetError() );
	}

    //Find game controllers/joysticks
    if( SDL_NumJoysticks()>=1 ) {
        joystick = SDL_JoystickOpen(0);
        if( joystick==NULL ) {
            nch::Log::error( __PRETTY_FUNCTION__, "Failed to open joystick!", SDL_GetError() );
        } else {
            std::stringstream ss;
            ss << "Successfully found joystick '"; ss << SDL_JoystickName(joystick);
            ss << "' with "; ss << SDL_JoystickNumAxes(joystick) << " axes ";
            ss << "and "; ss << SDL_JoystickNumButtons(joystick) << " buttons...";
            nch::Log::log(ss.str());
        }
    }



    /* Set Hints */
    //Render driver should be opengl rather than Windows' default of direct3d.
    //Done to prevent graphical issues when rendering to SDL_TEXTUREACCESS_TARGET textures.
    if(true) {
        if( SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl")==SDL_FALSE ) {
            nch::Log::error( __PRETTY_FUNCTION__, "Failed to set hint", SDL_GetError() );
        }
    }
    SDL_CaptureMouse(SDL_FALSE);

    /* Get resource path of application */
    //If this function somehow fails, the application should not continue running.
    char* path = SDL_GetBasePath();
    if( path==NULL ) {
        nch::Log::error(__PRETTY_FUNCTION__, "Failed to get resource path", "SDL_GetBasePath()==NULL");
        nch::Log::throwException();
    } else {
        resourcePath = (std::string)path+"backtoearth/";
    }
    SDL_free(path);

    /* Get OS of this device and set filesystem type */
    const char* platform = SDL_GetPlatform();
    if( platform==NULL ) {
        nch::Log::error(__PRETTY_FUNCTION__, "Failed to get operating system", "SDL_GetPlatform()==NULL");
        nch::Log::throwException();
    } else {
        devicePlatform = (std::string)platform+"";
    }
    validateDevicePlatform();

	/* Get display information */
	SDL_DisplayMode dm;
	if( SDL_GetCurrentDisplayMode(0, &dm)!=0 ) {
		std::stringstream ss;
		ss << "Failed to get screen resolution, defaulting to " << maxWidth << "x" << maxHeight;
		nch::Log::error(__PRETTY_FUNCTION__, ss.str(), SDL_GetError());
	} else {
		maxWidth = dm.w;
		maxHeight = dm.h;
		
		//print out screen resolution
		std::stringstream ss;
		ss << "Got screen size as: " << maxWidth << "x" << maxHeight << ".";
	}
}

void SDLHandler::createWindowAndRenderer()
{
    /* Attempt to init Window */
    //If window should be fullscreen
    if(fullscreen) {
        //Create fullscreen window that is positioned in the display with id 'lastDisplayID.'
        window = SDL_CreateWindow(
                                  Main::TITLE.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  0, 0,
                                  SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                                  );
    //If window should not be fullscreen
    } else {
        //Create resizable window that is resizable and positioned in the display with id 'lastDisplayID.'
        window = SDL_CreateWindow(
                                  Main::TITLE.c_str(),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  SDL_WINDOWPOS_UNDEFINED_DISPLAY(lastDisplayID),
                                  maxWidth*3/4, maxHeight*3/4,
                                  SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
                                  );
    }

    //If window failed to be created, the program should stop
    if( window==NULL ) {
        //Print error, stop program
        nch::Log::error(__PRETTY_FUNCTION__, "Window is null");
        nch::Log::throwException();
    }

    //Initialize width and height.
    SDL_GetWindowSize(window, &width, &height);

    //Attempt to init renderer
    uint32_t rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    windowRenderer = SDL_CreateRenderer(window, -1, rendererFlags);
    //If renderer failed to be created, the program should stop
    if( windowRenderer==NULL ) {
        //Print error, stop program.
        nch::Log::error(__PRETTY_FUNCTION__, "Renderer is null");
        nch::Log::throwException();
    }


    //Allocate pixel format to be used by most textures
    SDL_FreeFormat(windowPixelFormat);
    windowPixelFormat = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
    //If pixel format failed to be allocated, the program should stop
    if( windowPixelFormat==NULL ) {
        //Print error, stop program.
        nch::Log::error(__PRETTY_FUNCTION__, "Pixel format is null");
        nch::Log::throwException();
    }
}

void SDLHandler::validateDevicePlatform()
{
    // If SDL says "Windows"
    if( devicePlatform=="Windows" ) {
        filesystemType = Platforms::WINDOWS;
    // If SDL says "Linux"
    } else if( devicePlatform=="Linux" ) {
        filesystemType = Platforms::LINUX;
    // If SDL says anything else
    } else {
        nch::Log::warn(__PRETTY_FUNCTION__, "Unknown operating system '"+devicePlatform+"' detected", "using Linux filesystem functions");
        filesystemType = Platforms::UNKNOWN;
    }
}

void SDLHandler::setVideoDriversDesc()
{
    int nvd = SDL_GetNumVideoDrivers();
    const char* cvd = SDL_GetCurrentVideoDriver();

    std::stringstream ss;
    ss << nvd << " video drivers found: { ";
    for( int i = 0; i<nvd; i++ ) {
        const char* vd = SDL_GetVideoDriver(i);
        ss << vd;

        if( cvd==vd ) {
            ss << " [in use]";
        }

        if(i<nvd-1) {
            ss << ", ";
        }
    }
    ss << " }";

    videoDriversDesc = ss.str();
}