#pragma once
#include <memory>
#include <string>
#include "BTE.h"
#include "Controls.h"
#include "FileHandler.h"
#include "SDLHandler.h"

class MainLoop
{
public:
    /**/
    MainLoop();
    virtual ~MainLoop();
    /**/
    static int getCurrentTPS(); static int getCurrentFPS();			//Get current ticks per second / frames per second.
    static double getCurrentMSPT(); static double getCurrentMSPF();	//Get current milliseconds per tick / milliseconds per frame.
    static int getCurrentPTPS(); static int getCurrentPFPS();		//Get current potential TPS / potential FPS. Refers to what they would theoretically be if there were no cap.
    static uint64_t getFrames();
    static char* getSystemTime();
    static std::string getSystemTimeFilename();
    static uint64_t getNextSecond();
    static bool isInitialized();
    /**/
    static void setMaxFPS(int maxFPS);
    static void setMaxFPS(std::string settingValue);
    static void quit();

private:
    /* State of program */
    static bool running;
    static bool initialized;

    /* Helper classes */
    static SDLHandler sdlHandler;
    static FileHandler fileHandler;
    static Controls controls;
    static BTE bte;

    /* Frames and ticks */
    static double maxFPS;
    static double msPerTick; static uint64_t ticks; static int ticksThisSecond; static int currentTPS; static double currentMSPT;
    static double msPerFrame; static uint64_t frames; static int framesThisSecond; static int currentFPS; static double currentMSPF;

    static double msThisTick; static double msThisFrame;
    static double msptThisSec; static double mspfThisSec;
    static uint64_t nextSecond;

    static void crossSleep(int milliseconds);
    static void gameLoop();    //Main game loop (draw and tick)
    static void trackEvents(); //Track SDL_Events to handle keyboard/mouse input, program exit requests, etc.
    static void tick();        //Update game objects at 60 times a second.
    static void draw();        //Draw game objects at 120 times a second.
};
