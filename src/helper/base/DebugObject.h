#pragma once
#include <sstream>
#include "DebugScreen.h"

class DebugObject
{
public:
    virtual void info(std::stringstream& ss, int& tabs);

protected:

    std::string name = "unnamed object";

private:
};
