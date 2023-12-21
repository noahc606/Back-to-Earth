#pragma once
#include <iostream>
#include <sstream>
#include "FileHandler.h"

class Log
{
public:
    /**/
    Log();
    virtual ~Log();
    /**/
	static void initAll(std::string p_resourcePath, int p_filesystemType);
	static void destroyAll();
	
	static void printStringStream(std::stringstream& ss);
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

protected:

private:

	static FileHandler* fileHandler;
	static bool logToFile;
	static bool fhInit;
	static bool logDestroyed;
};
