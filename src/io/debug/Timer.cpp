#include "Timer.h"
#include <SDL.h>
#include <sstream>
#include "Log.h"

/**/
Timer::Timer(std::string p_desc, bool p_logging)
{
    //Set description
    desc = p_desc;
    logging = p_logging;

    //Start timer
    t0 = getCurrentTime();
}
Timer::Timer(std::string p_desc): Timer::Timer(p_desc, false){}
Timer::Timer(): Timer::Timer("Generic timer", false){}

Timer::~Timer()
{
    //Update elapsed time
    updateElapsedTime();

    //Log timer message
    if( logging ) {
        debugElapsedTimeMS();
    }
}
/**/

uint64_t Timer::getCurrentTime()
{
    return SDL_GetPerformanceCounter();
}

double Timer::getElapsedTimeMS()
{
    //If timer hasn't finished, get elapsed time so far.
    if( dT==-1.0 ) {
        updateElapsedTime();
    }

    return dT;
}

void Timer::debugElapsedTimeMS()
{
    std::stringstream ss;
    ss << getElapsedTimeMS();
    Log::log("Finished "+desc+" in "+ss.str()+"ms.");
}

void Timer::updateElapsedTime()
{
    //End timer
    t1 = getCurrentTime();
    //Calculate time elapsed (end time - initial time)
    dT = (t1-t0)*1000.0/(double)SDL_GetPerformanceFrequency();
}
