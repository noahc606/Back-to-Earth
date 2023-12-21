#include "Log.h"
#include <SDL.h>
#include "Main.h"
#include "MainLoop.h"

bool Log::trbsOverride = false;
FileHandler* Log::fileHandler = nullptr;
bool Log::logToFile = false;
bool Log::fhInit = false;
bool Log::logDestroyed = false;

Log::Log(){}
Log::~Log(){}

void Log::initAll(std::string p_resourcePath, int p_filesystemType)
{
	if(!fhInit) {
		fileHandler = new FileHandler();
		fileHandler->init(p_resourcePath, p_filesystemType);
		fhInit = true;
		
		//Should we log to file?
		std::string ltf = fileHandler->getSettings()->get( Settings::options, "logging" );
		if( ltf=="true" ) {
			logToFile = true;
			fileHandler->cEditFile("saved/logs/latest.log");
		}
	} else {
		Log::warn(__PRETTY_FUNCTION__, "Already initialized fileHandler");
	}
}

void Log::destroyAll()
{
	logDestroyed = true;
	
	Log::log("Destroying log...");
	fileHandler->saveCloseFile();
	fileHandler->mvFile("saved/logs/latest.log", "saved/logs/Log_"+MainLoop::getSystemTimeFilename()+".log" );
	delete fileHandler;
}

void Log::printStringStream(std::stringstream& ss)
{
	if(!logDestroyed) {
		std::cout << ss.str();
		if( logToFile && fileHandler!=nullptr && fileHandler->fileExists("saved/logs/latest.log") ) {
			fileHandler->write(ss.str());
		}
	}
}

void Log::log(std::string p_message)
{
	std::stringstream ss;
	ss << "[  Log  ] " << p_message << "\n";
	printStringStream(ss);
}

void Log::debug(std::string p_message)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
		std::stringstream ss;
		ss << "[ Debug ] " << p_message << "\n";
		printStringStream(ss);
    }
}

void Log::debug(std::string p_method, std::string p_message)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
		std::stringstream ss;
		ss << "[ Debug ] " << p_method << " - " << p_message << "\n";
		printStringStream(ss);
    }
}

void Log::coords(std::string object, int x, int y)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
		std::stringstream ss;
		ss << "[ Debug ] (x, y) of object \"" << object << "\" = (" << x << ", " << y << ").\n";
		printStringStream(ss);
    }
}

void Log::coords(std::string object, int x, int y, int w, int h)
{
    if( Main::DEBUG || Main::TROUBLESHOOTING ) {
		std::stringstream ss;
		ss << "[ Debug ] (x, y, w, h) of object \"" << object << "\" = (" << x << ", " << y << ", " << w << ", " << h << ").\n";
		printStringStream(ss);
    }
}

void Log::trbshoot(std::string p_method, std::string p_message)
{
    if(Main::TROUBLESHOOTING || trbsOverride) {
		std::stringstream ss;
		ss << "[ TrbSh ] " << p_method << " - " << p_message << "\n";
		printStringStream(ss);
    }
}

void Log::trbshoot(std::string p_method, std::string p_message, std::string p_resolution)
{
    if(Main::TROUBLESHOOTING || trbsOverride) {
		std::stringstream ss;
		ss << "[ TrbSh ] " << p_method << " - " << p_message << ", " << p_resolution << "\n";
		printStringStream(ss);
    }
}

void Log::warn(std::string p_method, std::string p_message, std::string p_resolution)
{
	std::stringstream ss;
	ss<< "[Warning] " << p_method << " - " << p_message << ", " << p_resolution << "...\n";
	printStringStream(ss);
}

void Log::warn(std::string p_method, std::string p_message)
{
	std::stringstream ss;
	ss << "[Warning] " << p_method << " - " << p_message << ", ignoring issue...\n";
	printStringStream(ss);
}

void Log::error(std::string p_method, std::string p_message)
{
	std::stringstream ss;
	ss << "[ ERROR ] " << p_method << " - " << p_message << "!\n";
	printStringStream(ss);
}

void Log::error(std::string p_method, std::string p_message, std::string error)
{
	std::stringstream ss;
	ss << "[ ERROR ] " << p_method << " - " << p_message << ": " << error << "!\n";
	printStringStream(ss);
}

void Log::error(std::string p_method, std::string p_message, const char* error)
{
	std::stringstream ss;
	ss << "[ ERROR ] " << p_method << " - " << p_message << ": " << error << "!\n";
	printStringStream(ss);
}


void Log::throwException()
{
    throw std::exception();
}
