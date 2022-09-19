#include "MainLoop.h"
#include <ctime>
#include <iostream>
#include <SDL.h>
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
    setMaxFPS(120);

    //Create Back to Earth subsystems
    Log::trbshoot(__PRETTY_FUNCTION__, "Creating BTE subsystems");
    sdlHandler.init();
    fileHandler.init( sdlHandler.getResourcePath() );
    controls.init( fileHandler.getSettings() );

    bte.init( &sdlHandler, &fileHandler, &controls );

    initialized = true;

    //Start gameLoop
    Log::log("Running "+Main::VERSION+"...");
    while(running) gameLoop();
}

MainLoop::~MainLoop()
{
    //Stop gameLoop
    Log::log("Exiting "+Main::VERSION+"...");
}

int MainLoop::getCurrentTPS() { return currentTPS; }
double MainLoop::getCurrentMSPT() { return currentMSPT; }
int MainLoop::getCurrentFPS() { return currentFPS; }
double MainLoop::getCurrentMSPF() { return currentMSPF; }
uint64_t MainLoop::getFrames() { return frames; }
char* MainLoop::getSystemTime() { time_t now = time(0); return ctime(&now); }
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
        draw();
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

            default: {
                //Track all keyboard and mouse events
                controls.trackEvents(e);
            } break;
        }

        //Pass normal keyboard input to BTE
        if( e.type==SDL_TEXTINPUT ) {
            std::stringstream ss;
            ss << e.text.text;

            bte.getGUIHandler()->passKeyboardInput(ss.str(), false);
        }

        //Pass special keyboard input to BTE
        Controls::KeyboardInput kbi = controls.getKeyboardInput();
        if( kbi.inputReceived ) {
            bte.getGUIHandler()->passKeyboardInput(kbi.inputString, kbi.inputSpecial);
            controls.resetKBInput(__PRETTY_FUNCTION__);
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
            fileHandler.reload();
        }

        controls.stopPress("FUNC_9", __PRETTY_FUNCTION__);
    }

    if( controls.isPressed("FUNC_SCREENSHOT")) {
        fileHandler.createPNGScreenshot(sdlHandler.getWindow(), sdlHandler.getRenderer(), sdlHandler.getPixelFormat()) ;
        controls.stopPress("FUNC_SCREENSHOT", __PRETTY_FUNCTION__);
    }

    if(controls.isPressed("FUNC_FULLSCREEN")) {
        sdlHandler.toggleFullScreen();
        controls.stopPress("FUNC_FULLSCREEN", __PRETTY_FUNCTION__);
    }

    bte.tick();
}

void MainLoop::draw()
{
    SDL_RenderClear( sdlHandler.getRenderer() );
    SDL_SetRenderDrawColor( sdlHandler.getRenderer(), 0, 0, 0, 0 );
    SDL_SetRenderDrawBlendMode( sdlHandler.getRenderer(), SDL_BLENDMODE_BLEND );

    bte.draw();

    SDL_RenderPresent( sdlHandler.getRenderer() );
}
