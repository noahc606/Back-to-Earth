#pragma once
#include <string>

class Timer
{
public:
    /**/
    Timer(std::string desc, bool logging);
    Timer(std::string desc);
    virtual ~Timer();
    /**/
    double getElapsedTimeMS();
    /**/
protected:

private:
    /**/
    void updateElapsedTime();
    /**/
    /* Times (in milliseconds) */
    double t0 = 0.0;    //Initial time
    double t1 = 0.0;    //End time
    double dT = -1.0;   //Time elapsed

    /* Peripherals */
    bool logging = true;
    std::string desc = "Description unuset";
    /**/
};
