#pragma once
#include <iostream>
#include <sstream>

class Log
{
public:
    /**/
    Log();
    virtual ~Log();
    /**/
    static void log(std::string message);

    static void debug(std::string message);
    static void debug(std::string method, std::string message);

    static void coords(std::string object, int x, int y);
    static void coords(std::string object, int x, int y, int w, int h);

    static void trbshoot(std::string method, std::string message);
    static void trbshoot(std::string method, std::string message, std::string resolution);

    static void warn (std::string method, std::string message, std::string resolution);
    static void warn (std::string method, std::string message);
    static void error(std::string method, std::string message);
    static void error(std::string method, std::string message, std::string error);
    static void error(std::string method, std::string message, const char *error);

    static void throwException();
    /**/

    static bool trbsOverride;
    //static std::stringstream log;
    //static std::stringstream debug;
    //static std::stringstream trbsh;
    //static std::stringstream error;


protected:

private:
};
