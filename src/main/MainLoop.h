#pragma once
#include <memory>
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
    static int getCurrentTPS(); static int getCurrentFPS();
    static double getCurrentMSPT(); static double getCurrentMSPF();
    static uint64_t getFrames();
    static char* getSystemTime();
    static uint64_t getNextSecond();
    static bool isInitialized();
    /**/
    static void setMaxFPS(int maxFPS);
    static void quit();

private:
    /* State of program */
    static bool running;
    static bool initialized;

    /* Helper classes */
    SDLHandler sdlHandler;
    FileHandler fileHandler;
    Controls controls;
    BTE bte;

    /* Frames and ticks */
    static double maxFPS;
    static double msPerTick; static uint64_t ticks; static int ticksThisSecond; static int currentTPS; static double currentMSPT;
    static double msPerFrame; static uint64_t frames; static int framesThisSecond; static int currentFPS; static double currentMSPF;

    static double msThisTick; static double msThisFrame;
    static double msptThisSec; static double mspfThisSec;
    static uint64_t nextSecond;

    void gameLoop();    //Main game loop (draw and tick)
    void trackEvents(); //Track SDL_Events to handle keyboard/mouse input, program exit requests, etc.
    void tick();        //Update game objects at 60 times a second.
    void draw();        //Draw game objects at 120 times a second.
};
