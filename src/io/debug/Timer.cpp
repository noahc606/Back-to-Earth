#include "Timer.h"
#include <SDL.h>
#include <sstream>
#include "Log.h"

/**/
Timer::Timer(std::string p_desc)
{
    //Set description
    desc = p_desc;
    //Start timer
    t0 = SDL_GetPerformanceCounter()/10000.0;
}
Timer::~Timer()
{
    //End timer
    double t1 = SDL_GetPerformanceCounter()/10000.0;
    //Log timer message
    std::stringstream ss;
    ss << (t1-t0);
    Log::log("Finished "+desc+" in "+ss.str()+"ms.");
}
/**/
