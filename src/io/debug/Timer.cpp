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
    t0 = SDL_GetPerformanceCounter()/10000.0;
}
Timer::Timer(std::string p_desc):
Timer::Timer(p_desc, true){
}
Timer::~Timer()
{
    //Update elapsed time
    updateElapsedTime();

    //Log timer message
    if( logging ) {
        std::stringstream ss; ss << dT;
        Log::log("Finished "+desc+" in "+ss.str()+"ms.");
    }
}
/**/

double Timer::getElapsedTimeMS()
{
    //If timer hasn't finished, get elapsed time so far.
    if( dT==-1.0 ) {
        updateElapsedTime();
    }

    return dT;
}

void Timer::updateElapsedTime()
{
    //End timer
    t1 = SDL_GetPerformanceCounter()/10000.0;
    //Calculate time elapsed (end time - initial time)
    dT = t1-t0;
}
