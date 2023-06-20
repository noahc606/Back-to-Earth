#include "Log.h"
#include <SDL.h>
#include "Main.h"

bool Log::trbsOverride = false;

Log::Log(){}
Log::~Log(){}

void Log::log(std::string p_message)
{
    std::cout << "[  Log  ] " << p_message << std::endl;
}

void Log::debug(std::string p_message)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
        std::cout << "[ Debug ] " << p_message << std::endl;
    }
}

void Log::debug(std::string p_method, std::string p_message)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
        std::cout << "[ Debug ] " << p_method << " - " << p_message << std::endl;
    }
}

void Log::coords(std::string object, int x, int y)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
        std::cout << "[ Debug ] (x, y) of object \"" << object << "\" = (" << x << ", " << y << ").\n";
    }
}

void Log::coords(std::string object, int x, int y, int w, int h)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
        std::cout << "[ Debug ] (x, y, w, h) of object \"" << object << "\" = (" << x << ", " << y << ", " << w << ", " << h << ").\n";
    }
}

void Log::trbshoot(std::string p_method, std::string p_message)
{
    if(Main::TROUBLESHOOTING || trbsOverride) {
        std::cout << "[ TrbSh ] " << p_method << " - " << p_message << std::endl;
    }
}

void Log::trbshoot(std::string p_method, std::string p_message, std::string p_resolution)
{
    if(Main::TROUBLESHOOTING || trbsOverride) {
        std::cout << "[ TrbSh ] " << p_method << " - " << p_message << ", " << p_resolution << std::endl;
    }
}

void Log::warn(std::string p_method, std::string p_message, std::string p_resolution)
{
    std::cout << "[Warning] " << p_method << " - " << p_message << ", " << p_resolution << "..." << std::endl;
}

void Log::warn(std::string p_method, std::string p_message)
{
    std::cout << "[Warning] " << p_method << " - " << p_message << ", ignoring issue..." << std::endl;
}

void Log::error(std::string p_method, std::string p_message)
{
    std::cout << "[ ERROR ] " << p_method << " - " << p_message << "!" << std::endl;
}

void Log::error(std::string p_method, std::string p_message, std::string error)
{
    std::cout << "[ ERROR ] " << p_method << " - " << p_message << ": " << error << "!" << std::endl;
}

void Log::error(std::string p_method, std::string p_message, const char* error)
{
    std::cout << "[ ERROR ] " << p_method << " - " << p_message << ": " << error << "!" << std::endl;
}


void Log::throwException()
{
    throw std::exception();
}
