#pragma once
#include <string>

class Timer
{
public:
    Timer(std::string desc);
    virtual ~Timer();

protected:

private:
    double t0;
    std::string desc;
};
