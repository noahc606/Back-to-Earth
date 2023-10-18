#include "MainLoop.h"
#include <ctime>
#include <iostream>
#include <SDL2/SDL.h>
#include "Log.h"
#include "Main.h"
#include "Timer.h"

bool MainLoop::running = true;
bool MainLoop::initialized = false;

double MainLoop::maxFPS = 120.0;
double MainLoop::msPerTick = 1000.0 / 60.0; double MainLoop::msPerFrame = 1000.0/maxFPS;
uint64_t MainLoop::ticks = 0; uint64_t MainLoop::frames = 0;
int MainLoop::ticksThisSecond = 0; int MainLoop::framesThisSecond = 0;
int MainLoop::currentTPS = 0; int MainLoop::currentFPS = 0;
double MainLoop::currentMSPT = 0; double MainLoop::currentMSPF = 0;
double MainLoop::msThisTick = 0; double MainLoop::msThisFrame = 0;
double MainLoop::msptThisSec = 0; double MainLoop::mspfThisSec = 0;
uint64_t MainLoop::nextSecond = 0;

MainLoop::MainLoop()
{
    setMaxFPS(maxFPS);
    //Create Back to Earth subsystems
    Log::trbshoot(__PRETTY_FUNCTION__, "Creating BTE subsystems");
    //SDL (pre-init): file handler and controls.
    sdlHandler.preinit();
    fileHandler.init( sdlHandler.getResourcePath(), sdlHandler.getFilesystemType() );
	Log::initAll( sdlHandler.getResourcePath(), sdlHandler.getFilesystemType() );
    controls.init( fileHandler.getSettings() );
    //BTE (pre-init): Managing settings
    bte.preinit( &sdlHandler, &fileHandler, &controls );
    //SDL & BTE: init if hard testing is disabled
    if( !bte.isHardTesting() ) {
        //SDL (init): window and asset loading
        sdlHandler.init();
        //BTE (init): GUI and Debug
        bte.init();
    }

    //Start gameLoop
    initialized = true;
    if(!bte.isHardTesting()) {
        Log::log("Running "+Main::VERSION+"...");
        while(running) gameLoop();
    }
}

MainLoop::~MainLoop()
{
    if(!bte.isHardTesting()) {
        sdlHandler.getTextureLoader()->destroy();
        Log::log("Exiting "+Main::VERSION+"...");
    } else {
        Log::log("Finished hard testing "+Main::VERSION+"...");
        Log::log("To enable the BTE window, make sure you have \"debugHardTesting=false\" in 'backtoearth/saved/settings/options.txt'!");
    }
}

int MainLoop::getCurrentTPS() { return currentTPS; }
double MainLoop::getCurrentMSPT() { return currentMSPT; }
int MainLoop::getCurrentFPS() { return currentFPS; }
double MainLoop::getCurrentMSPF() { return currentMSPF; }
uint64_t MainLoop::getFrames() { return frames; }
char* MainLoop::getSystemTime() { time_t now = time(0); return ctime(&now); }
std::string MainLoop::getSystemTimeFilename()
{
	/* Build file name */
	std::string date = MainLoop::getSystemTime();
	int firstSpace = -1;
	int lastSpace = -1;
	for( unsigned int i = 0; i<date.length(); i++ ) {
		if(date[i]==' ') {
			lastSpace = i;
			date[i] = '_';
		}
		
		if(firstSpace<0 && date[i]==' ') {
			firstSpace = i;
			date[i] = '_';
		}
		
		if(date[i]==':') {
			date[i] = '_';
		}
	}
	
	/* Return */
	return date.substr(firstSpace+1, lastSpace-4);
}
uint64_t MainLoop::getNextSecond() { return nextSecond; }



bool MainLoop::isInitialized() { return initialized; }

void MainLoop::setMaxFPS(int maxFPS) {
    std::stringstream ss; ss << "Setting max FPS to '" << maxFPS << "'...";
    Log::log(ss.str());

    MainLoop::maxFPS = maxFPS;
    MainLoop::msPerFrame = 1000.0/MainLoop::maxFPS;
}

void MainLoop::quit() { running = false; }

void MainLoop::gameLoop()
{
    //Track SDL Events
    trackEvents();

    //Tick game objects up to 60 times a second
    if( SDL_GetTicks()%1000>msPerTick*ticksThisSecond ) {
        //Track ticks this second to calculate TPS
        ticksThisSecond++;

        //Tick objects and track milliseconds per tick (mspt)
        uint64_t t0 = SDL_GetPerformanceCounter();
        tick();
        uint64_t t1 = SDL_GetPerformanceCounter();
        msptThisSec += (t1-t0)*1000.0/SDL_GetPerformanceFrequency();
    }

    if( SDL_GetTicks()%1000>msPerFrame*framesThisSecond ) {
        //Track frames this second to calculate FPS
        framesThisSecond++;

        //Draw objects and track milliseconds per frame (mspf)
        uint64_t t0 = SDL_GetPerformanceCounter();
		
		if(running) {
			draw();
		}
        uint64_t t1 = SDL_GetPerformanceCounter();
        mspfThisSec += (t1-t0)*1000.0/SDL_GetPerformanceFrequency();
    }

    //This block runs every second.
    if( SDL_GetTicks()>=nextSecond ) {

        currentMSPF = mspfThisSec / framesThisSecond;
        currentMSPT = msptThisSec / ticksThisSecond;
        mspfThisSec = 0;
        msptThisSec = 0;

        //Update nextSecond to the next second to tell the block when to run again.
        nextSecond += 1000;

        //Get frames and ticks from the last second.
        currentFPS = framesThisSecond;
        currentTPS = ticksThisSecond;

        //Reset frame and tick counter to be ready to track for the next second.
        ticksThisSecond = 0;
        framesThisSecond = 0;
    }
}

void MainLoop::trackEvents()
{
    //Handle SDL_Events
    SDL_Event e;
    while( SDL_PollEvent(&e)!=0 )
    {
        switch( e.type )
        {
            case SDL_QUIT: {
                running = false;
            } break;

            //Window events
            case SDL_WINDOWEVENT: {
                //Track last width and last height
                int lastWidth = sdlHandler.getWidth();
                int lastHeight = sdlHandler.getHeight();

                //Update width and height variables in sdlHandler.
                sdlHandler.trackEvents(e);

                //Tell screen objects that the window changed (for resizing center-aligned buttons/windows with the screen, etc.)
                if( lastWidth!=sdlHandler.getWidth() || lastHeight!=sdlHandler.getHeight() ) {
                    bte.onWindowUpdate();
                }
            } break;

            //Any other event (vast majority will be related to moving mouse, keyboard, etc)
            default: {
                //Track input events in the Controls class
                controls.trackEvents(e);
            } break;
        }

        //Pass special input to GUIHandler
        switch( e.type ) {
            case SDL_TEXTINPUT:
            case SDL_KEYDOWN:
            case SDL_JOYBUTTONDOWN: {
                //Pass special input to GUIHandler
                ControlBinding cbsi = controls.getSpecialInput();
                if( cbsi.getType()!=cbsi.NOTHING ) {
                    bte.getGUIHandler()->passSpecialInput(cbsi);
                    controls.resetSpecialInput(__PRETTY_FUNCTION__);
                }
            } break;
        }
    }
}

void MainLoop::tick()
{
    controls.tick();

    if( controls.isPressed("FUNC_9") ) {
        Log::log("Reloading Back to Earth assets...");

        //Reload all resources and track how much time it takes
        {
            Timer t("reloading all resources");
            sdlHandler.getTextureLoader()->reload();
            fileHandler.reloadSettings();
        }

        controls.stopPress("FUNC_9", __PRETTY_FUNCTION__);
    }

    if( controls.isPressed("FUNC_SCREENSHOT") ) {
        fileHandler.createPNGScreenshot(sdlHandler.getWindow(), sdlHandler.getRenderer(), sdlHandler.getPixelFormat()) ;
        controls.stopPress("FUNC_SCREENSHOT", __PRETTY_FUNCTION__);
    }

    if( controls.isPressed("FUNC_FULLSCREEN") ) {
        sdlHandler.toggleFullScreen();
        controls.stopPress("FUNC_FULLSCREEN", __PRETTY_FUNCTION__);
    }

    bte.tick();
}

void MainLoop::draw()
{
    controls.draw();

    SDL_RenderClear( sdlHandler.getRenderer() );
    SDL_SetRenderDrawColor( sdlHandler.getRenderer(), 0, 0, 0, 0 );
    SDL_SetRenderDrawBlendMode( sdlHandler.getRenderer(), SDL_BLENDMODE_BLEND );

    bte.draw();

    SDL_RenderPresent( sdlHandler.getRenderer() );
}
