#include "Log.h"
#include <SDL2/SDL.h>
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

void Log::throwException(std::string funcname, std::string format)
{
	error(funcname, format);
	throwException();
}

void Log::throwException()
{
    throw std::exception();
}

void Log::logString(std::string s)
{
	if(!logDestroyed) {
		std::cout << s;
		if( logToFile && fileHandler!=nullptr && fileHandler->fileExists("saved/logs/latest.log") ) {
			fileHandler->write(s);
		}
	}
}

void Log::logSStream(std::stringstream& ss)
{
	logString(ss.str());
}