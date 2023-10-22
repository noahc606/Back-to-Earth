#include "FileHandler.h"
#include <bits/stdc++.h>
#include <codecvt>
#include <dirent.h>
#include <errno.h>
#include <locale>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Controls.h"
#include "Log.h"
#include "MainLoop.h"
#include "SDLHandler.h"
#include "Timer.h"

FileHandler::FileHandler(){}
FileHandler::~FileHandler(){}

void FileHandler::init( std::string rp, int fsType )
{
    //Resource path (for saving any type of file to disk)
    resourcePath = rp;

    //Filesystem type (for directory creation functions)
    filesystemType = fsType;

    //Set file paths.
    files[Settings::controls] = new FilePath("saved/settings/controls.txt", filesystemType);
    files[Settings::games] =    new FilePath("saved/settings/games.txt", filesystemType);
    files[Settings::options] =  new FilePath("saved/settings/options.txt", filesystemType);
    files[Settings::version] =  new FilePath("saved/settings/version.txt", filesystemType);
    files[Settings::session] =  new FilePath("saved/settings/session.txt", filesystemType);

    //Create directories
    createDir(resourcePath);
    createBTEDir("dump");
    createBTEDir("saved/games");
    createBTEDir("saved/logs");
    createBTEDir("saved/screenshots");
    createBTEDir("saved/settings");

    //Load and save settings files.
    loadSettings();
    saveSettings();
}

/**
    Create a directory inside of the main resource path (backtoearth folder)
        - Also tries to create parent directories.
*/
int FileHandler::createBTEDir(std::string path)
{
    path = resourcePath+path;

    int x;
    do {
        x = createDir(path);
    } while(x==0);

    return 1;
}

int FileHandler::createPNGScreenshot(SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf)
{
	/* Generate screenshot file path */
	std::string name = MainLoop::getSystemTimeFilename();
	FilePath ssPath(resourcePath+"saved/screenshots/"+name, "png", filesystemType);

    /* Get window width and height */
    int width = 0; int height = 0;
    SDL_GetWindowSize(w, &width, &height);

    /* Create surface that looks like the current screen */
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat( 0, width, height, 32, pf->format );
    SDL_RenderReadPixels( r, NULL, pf->format, surf->pixels, surf->pitch );

    /* Save the newly created surface as an image */
    //Success
    if(IMG_SavePNG(surf, ssPath.get().c_str())==0) {
        //Message
        std::string msg = "Screenshot saved to " + ssPath.get();
        Log::log(msg);
        //Free surface
        SDL_FreeSurface(surf);
        return 0;
    //Failure
    } else {
        //Message
        std::string msg = "Failed to save screenshot saved at " + ssPath.get();
        Log::error(__PRETTY_FUNCTION__, msg);
        //Free surface
        SDL_FreeSurface(surf);
        return -1;
    }
}

int FileHandler::openFile(std::string path, int openType, bool binary)
{
    /* Prelims */
	//Close any old file
	saveCloseFile();
    //Get filepath
    FilePath fp(path, filesystemType);
    std::string mpath = getModifiedPath(fp);
    //Get whether file already exists
    bool fExists = (fileExists(path));
    if(!fExists) {
        Log::debug( __PRETTY_FUNCTION__, "File '"+mpath+"' not found, attempting to create new file..." );
    }
    //Mode: Binary (b) or Text (t).
    std::string modeArg = "b";
    if(!binary) {
        modeArg = "t";
    }

    /* Open file */
    //Open file for writing, appending, or reading
    switch(openType) {
    case FileOpenTypes::WRITE:  file = std::fopen(mpath.c_str(), ("w"+modeArg).c_str()); break;
    case FileOpenTypes::APPEND: file = std::fopen(mpath.c_str(), ("a"+modeArg).c_str()); break;
    case FileOpenTypes::READ:   file = std::fopen(mpath.c_str(), ("r"+modeArg).c_str()); break;
    case FileOpenTypes::UPDATE: file = std::fopen(mpath.c_str(), ("r"+modeArg+"+").c_str()); break;
    }

    /* Return function value */
    //If file failed to create
    if(file==NULL) {
        std::string action = "open";
        if(fExists) { action = "create"; }
        Log::warn("Failed to "+action+" file '"+mpath+"' for "+getFileOpenTypeStr(openType), __PRETTY_FUNCTION__);
        return FileStates::FAILED_ACCESS;
    //If file was successfully created
    } else {
        if(fExists) {
            return FileStates::EXISTING;
        } else {
            return FileStates::NEW;
        }
    }
}
int FileHandler::openFile(std::string path, int openType) { return openFile(path, openType, false); }

/**
    "(C)lear-Edit": Opens file (contents are cleared) for writing. Simpler to use than pOpenFile()
    Returns:
        Whether we fail to access the file (-1), create a new file (0), or access an existing file (1)
*/
int FileHandler::cEditFile(std::string path)
{
    //Save/close last file and set new FILE*. Store result of file-opening.
    int foRes = openFile(path, FileOpenTypes::WRITE);
    
    //Return file-open result
    return foRes;
}

int FileHandler::clearFile(std::string path)
{ 
	if( !fileExists(path) ) {
		return -2;
	}
	return cEditFile(path);
}

/**
 * Moves a file from 'srcPath' to 'dstPath'. Can be used to rename a file if the src and dst have the same directory but different filenames.
 * Basically the same as the mv command in Linux.
 * 
 */
int FileHandler::mvFile(std::string srcPath, std::string dstPath)
{
	//Old path
	FilePath srcFp(srcPath, filesystemType);
	std::string srcMPath = getModifiedPath(srcFp);
	//New path
	FilePath dstFp(dstPath, filesystemType);
	std::string dstMPath = getModifiedPath(dstFp);
	
	
	if(std::rename(srcMPath.c_str(), dstMPath.c_str())==0) {
		return 0;
	}
	
	if( !fileExists(srcMPath) ) {
		std::stringstream ss;
		ss << "Can't rename file '" << srcMPath << "' which doesn't exist";
		Log::error(__PRETTY_FUNCTION__, ss.str());
		return -1;
	}
	
	std::stringstream ss;
	ss << "Unknown error in renaming file '" << srcMPath;
	Log::error(__PRETTY_FUNCTION__, ss.str());
	return -2;
}

template<typename T> int FileHandler::write(T t)
{
	std::stringstream ss; ss << t;
	
	if( std::fputs(ss.str().c_str(), file)==EOF ) {
		std::string logres = "Unsuccessful fputs() call for string '"+ss.str()+"'";
		Log::error(__PRETTY_FUNCTION__, logres, strerror(errno));
		return -1;
	}
	return 0;
}
int FileHandler::writeByte(uint8_t byte)
{
	if(fwrite(&byte, 1, 1, file)!=1) {
		std::stringstream logres;
		logres << "Unsuccessful fwrite() call for data '" << byte << "'";
		Log::error(__PRETTY_FUNCTION__, logres.str(), strerror(errno));
		return -1;
	}
	return 0;
}
int FileHandler::writeChar(char c) { return write(c); }
int FileHandler::writeln(std::string text) { return write(text+"\n"); }
int FileHandler::writeln() { return writeln(""); }

int FileHandler::saveCloseFile()
{ 
	//Check if file is already nullptr
	if(file==nullptr) {
		return -2;
	}
	
	//Close file and set to nullptr
	int fcRes = std::fclose(file);
	file = nullptr;
	if(fcRes==0) {
		return 0;
	}
	return -1;
}

bool FileHandler::fileExists(FilePath fp)
{
    struct stat buffer;
    std::string absolutePath = (getModifiedPath(fp));
    return (stat( absolutePath.c_str(), &buffer) == 0);
}
bool FileHandler::fileExists(std::string path)
{
    FilePath fp(path, filesystemType);
    return fileExists(fp);
}

uint8_t FileHandler::readByte()
{
	uint8_t buffer = 0;
	if( fread(&buffer, sizeof(uint8_t), 1, file)!=1 ) {
		Log::warn(__PRETTY_FUNCTION__, "Failed to read byte from file.");
	}
	return buffer;
}

/**
    t_kvSet: A vector of key-value pairs (both elements of the pair are strings)
    Get contents of a .txt file line by line. Returns empty vector if file loading failed.
    Escape sequences before a newline char or an equals sign char causes those characters to be read in normally.
*/
Settings::t_kvMap FileHandler::readTxtFileKVs(FilePath fp)
{
    openFile(fp.get(), FileOpenTypes::READ, false);

	//Create an empty t_kvMap
	Settings::t_kvMap contents;

	//Return an empty kvMap if file not found.
	if( !fileExists(fp) ) {
		Log::debug( __PRETTY_FUNCTION__, "File '"+fp.get()+"' not found" );
		return contents;
	}
	
	std::string currentKey = "";
    std::string currentValue = "";
    bool doEscape = false;
    bool foundEqualSign = false;
    bool foundNewLine = false;
	
	char c = '_';
	while( (c = std::fgetc(file))!=EOF ) {
        //Reset foundNewLine
        foundNewLine = false;

        //If we find an escape character, go to the next character and add it normally
        if( c==27 && !doEscape ) {
            doEscape = true;
            continue;
			//If we find an equals character that isn't escaped
        } else if( c=='=' && !doEscape ) {
            //Edge case: multiple = signs after one another
            if( foundEqualSign ) {
                currentValue += c;
            }
            //foundEqualsSign set to true
            foundEqualSign = true;
        //If we find a newline character that isn't escaped
        } else if( c=='\n' && !doEscape ) {
            //update foundNewLine and foundEqualSign.
            foundNewLine = true;
            foundEqualSign = false;

            //Add currentKey and currentValue as pairs to the vector
            contents.insert( std::make_pair(currentKey, currentValue) );

            //Reset currentKey and currentValue
            currentKey = ""; currentValue = "";
        //If we find any other character
        } else {
            //If we are past an equals sign already, then add to currentValue.
            //If we have not passed an equals sign yet, then add to currentKey.
            if( foundEqualSign ) {
                currentValue += c;
            } else {
                currentKey += c;
            }
        }

		//Reset doEscape
		doEscape = false;
    }

    //Push back currentKey and currentValue if newline not found
    if( !foundNewLine ) {
        contents.insert( std::make_pair(currentKey, currentValue) );
    }
	
    return contents;
}

long FileHandler::tellPos()
{
	return ftell(file);
}
long FileHandler::seekTo(long byte)
{
	if( fseek(file, byte, SEEK_SET)!=0 ) {
		std::stringstream logres;
		logres << "Failed to seek to position " << byte << " in file.";
		Log::warn(__PRETTY_FUNCTION__, logres.str());
		return -1;
	}
	return 0;
}
long FileHandler::seekThru(long bytesDelta)
{
	if( fseek(file, bytesDelta, SEEK_CUR) !=0 ) {
		std::stringstream logres;
		logres << "Failed to seek " << bytesDelta << " forward in file.";
		Log::warn(__PRETTY_FUNCTION__, logres.str());
		return -1;
	}
	return 0;
}

int FileHandler::saveSettings(int index)
{
    int success = -1;

    if( index>=0 && index<Settings::LAST_INDEX ) {
        cEditFile( (*files[index]).get() );

        auto kvMap = settings.getKvMap(index);
        if( kvMap.size()!=0 ) {
            for(auto kvp : kvMap) {
                write(kvp.first);
                write("=");
                write(kvp.second);
                write("\n");
            }
            success = 1;
        }

        saveCloseFile();
    } else {
        success = -1000;
    }

    if( success==1 ) {
        std::stringstream ss;
        ss << "Successfully saved file with index '" << index << "'.";
        Log::trbshoot(__PRETTY_FUNCTION__, ss.str());
    } else
    if( success==-1000 ) {
        std::stringstream ss;
        ss << "Index '" << index << "' doesn't exist.";
        Log::error(__PRETTY_FUNCTION__, ss.str());
    } else {
        std::stringstream ss;
        ss << "Failed to save file with index '" << index << "': kvMap.size()=0.";
        Log::debug(ss.str());
    }
    return success;
}

int FileHandler::saveSettings()
{
    int fails = 0;
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        if( saveSettings(i)!=1 ) {
            fails++;
        }
    }

    if( fails==0 )
        return 1;

    std::stringstream ss;
    ss << "Failed to save " << fails << " files, something is very wrong";
    Log::error(__PRETTY_FUNCTION__, ss.str());

    return -fails;
}

void FileHandler::reloadSettings()
{
    //Load settings and track how much time it takes
    {
        Timer t("reloading settings");
        unloadSettings();
        loadSettings();
    }
}

Settings* FileHandler::getSettings() { return &settings; }
std::string FileHandler::getResourcePath() { return resourcePath; }
/**
    Takes a file path and returns it with the base BTE path added at the beginning
*/
std::string FileHandler::getModifiedPath(FilePath fp)
{
    //Get file path string
    std::string fps = fp.get();

    //Edge case: File path string already begins with a slash
    if( fps[0]=='/' || fps[0]=='\\' ) {
        fps = fps.substr(1);
    }

    //Build modified path that includes the bte path
    std::string mp;
    mp = resourcePath+fps;
    return mp;
}
std::string FileHandler::getFileOpenTypeStr(int fot)
{
    switch(fot) {
    case FileOpenTypes::WRITE:  return "writing";
    case FileOpenTypes::APPEND: return "appending";
    case FileOpenTypes::READ:   return "reading";
    }
    return "unknown";
}

/**
    Create a new directory (folder) at the path.
        - It will also attempt to create parent directories if they don't already exist (only 1 folder per function call).
    Trying to create new directories outside of the main resource path (backtoearth folder) will not work.
        - In most cases just use createBTEDir() instead.
*/
int FileHandler::createDir(std::string path)
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Attempting to create new directory at: '"+path+"'..." );

    // If 'path' is not within the backtoearth folder
    unsigned int minPathSize = resourcePath.length();
    if( path.substr(0, minPathSize)!=resourcePath ) {
        Log::error( __PRETTY_FUNCTION__,
                    "Tried to create a directory outside of the main resource path '"+resourcePath+"'",
                    "If you are seeing this, something is very wrong...");
        Log::throwException();
        return -2;
    }

    // If directory at 'path' isn't found
    if (opendir(path.c_str())==nullptr) {
        if(path==resourcePath) {
            Log::error( __PRETTY_FUNCTION__,
                        "Could not find main directory '"+resourcePath+"'",
                        "Make sure the 'backtoearth' folder is in the same location as the executable");
            Log::throwException();
        } else {
            Log::trbshoot(__PRETTY_FUNCTION__, "Could not find directory '"+path+"', attempting to create it...");
        }
    }

    // Make the directory...
    // ...for Windows
    #if defined(WIN32)
        FilePath fp(path, SDLHandler::WINDOWS);
        if(mkdir(fp.get().c_str())==0) {
            Log::trbshoot(__PRETTY_FUNCTION__, "Successfully created new directory." );
            return 0;
        } else {
            Log::error(__PRETTY_FUNCTION__, "Couldn't create directory.");
        }
    // ...for Linux and macOS
    #elif (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)) )
		#if defined(_POSIX_VERSION)
			FilePath fp(path, SDLHandler::LINUX);
			if(mkdir(fp.get().c_str(), 0775)==0) {
				Log::trbshoot(__PRETTY_FUNCTION__, "Successfully created new directory." );
				return 0;
			} else {
				Log::trbshoot(__PRETTY_FUNCTION__, "Found directory, doing nothing.");
			}
		#else
			Log::error(__PRETTY_FUNCTION__, "POSIX version undefined for this Unix OS, could not create directory");
		#endif
    // ...for something else? -> Error
    #else
		Log::error(__PRETTY_FUNCTION__, "Unknown operating system, could not create directory");
    #endif

    //If path to be created is invalid
    if(path==resourcePath) {
        return 1;
    }

    std::string subPath = "";
    for( unsigned int i = path.length()-1; i>=minPathSize; i-- ) {
        char c = path.at(i);
        if( c=='\\' || c=='/' ) {
            subPath = path.substr( 0, i );
            break;
        }

        if( i<=minPathSize ) {
            Log::trbshoot(__PRETTY_FUNCTION__,
                "Ignoring creation of a directory which already exists or has an invalid name",
                "unfortunately we can't tell the difference with mkdir()");
            return -1;
        }
    }

    return createDir(subPath);
}

void FileHandler::unloadSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        settings.unload(i);
    }
}

void FileHandler::loadSettings()
{
	//Iterate through all indices
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
		// Build kvm from files[i]
		Settings::t_kvMap kvm = readTxtFileKVs( *(files[i]) );
        settings.load( i, kvm );
		
		// If kvm's size is still 0 for some reason, load default settings
		if( settings.getKvMap(i).size()==0 ) {
			settings.load( i, settings.getDefaultSettings(i) );
		}
    }
}
