#include "FileHandler.h"
#include <bits/stdc++.h>
#include <codecvt>
#include <dirent.h>
#include <errno.h>
#include <filesystem>
#include <locale>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <SDL2/SDL_image.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "Controls.h"
#include "MainLoop.h"
#include "SDLHandler.h"

FileHandler::FileHandler(){}
FileHandler::~FileHandler(){}

void FileHandler::init( std::string rp, int fsType )
{
    //Resource path (for saving any type of file to disk)
    resourcePath = rp;

    //Filesystem type (for directory creation functions)
    filesystemType = fsType;

    //Set file paths.
    files[Settings::controls] = 	new BTEPath("saved/settings/controls.txt", filesystemType);
    files[Settings::games] =		new BTEPath("saved/settings/games.txt", filesystemType);
    files[Settings::options] =		new BTEPath("saved/settings/options.txt", filesystemType);
    files[Settings::session] =		new BTEPath("saved/settings/session.txt", filesystemType);
    files[Settings::character] =	new BTEPath("saved/settings/character.txt", filesystemType);

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

    int cd;
    do {
        cd = createDir(path);
    } while(cd==0);

    return 1;
}

int FileHandler::createPNGScreenshot(SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf)
{
	/* Generate screenshot file path */
	std::string name = MainLoop::getSystemTimeFilename();
	BTEPath ssPath(resourcePath+"saved/screenshots/"+name, "png", filesystemType);

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
        nch::Log::log(msg);
        //Free surface
        SDL_FreeSurface(surf);
        return 0;
    //Failure
    } else {
        //Message
        std::string msg = "Failed to save screenshot saved at " + ssPath.get();
        nch::Log::error(__PRETTY_FUNCTION__, msg);
        //Free surface
        SDL_FreeSurface(surf);
        return -1;
    }
}

void FileHandler::openUserLocalURL(std::string url)
{
    //Create local URL ('lurl') from 'url'
    std::stringstream ss;
    ss << "file://" << resourcePath << url;
    std::string lurl = ss.str();    //Local URL recognized as such by SDL (must start with "file:///")

    //Attempt to open 'lurl'
    nch::Log::log("Attempting to open local URL \"%s\" in user's file manager...", lurl.c_str());
    if(SDL_OpenURL(lurl.c_str())==-1) {
        nch::Log::errorv(__PRETTY_FUNCTION__, SDL_GetError(), "Failed to open local URL \"%s\"", lurl.c_str());
    }
}

int FileHandler::openFile(std::string path, int openType, bool binary)
{
    /* Prelims */
	//Close any old file
	saveCloseFile();
    //Get BTEPath
    BTEPath fp(path, filesystemType);
    std::string mpath = getModifiedPath(fp);
    //Get whether file already exists
    bool fExists = (fileExists(path));
    if(!fExists) {
        nch::Log::debug("File \"%s\" not found, attempting to create new file...", mpath.c_str());
    }
    //Mode: Binary (b) or Text (t).
    std::string modeArg = "b";
    if(!binary) {
        modeArg = "t";
    }

    /* Open file */
    //Open file for writing, appending, reading, or updating
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
        nch::Log::warn(__PRETTY_FUNCTION__, "Failed to "+action+" file '"+mpath+"' for "+getFileOpenTypeStr(openType));
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
    "(C)lear-Edit": Opens file (contents are cleared) for writing. Simpler to use than openFile()
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
	BTEPath srcFp(srcPath, filesystemType);
	std::string srcMPath = getModifiedPath(srcFp);
	//New path
	BTEPath dstFp(dstPath, filesystemType);
	std::string dstMPath = getModifiedPath(dstFp);
	
	
	if(std::rename(srcMPath.c_str(), dstMPath.c_str())==0) {
		return 0;
	}
	
	if( !fileExists(srcMPath) ) {
		std::stringstream ss;
		ss << "Can't rename file '" << srcMPath << "' which doesn't exist";
		nch::Log::error(__PRETTY_FUNCTION__, ss.str());
		return -1;
	}
	
	std::stringstream ss;
	ss << "Unknown error in renaming file '" << srcMPath;
	nch::Log::error(__PRETTY_FUNCTION__, ss.str());
	return -2;
}

template<typename T> int FileHandler::write(T t)
{
	std::stringstream ss; ss << t;
	
	if( std::fputs(ss.str().c_str(), file)==EOF ) {
		std::string logres = "Unsuccessful fputs() call for string '"+ss.str()+"'";
		nch::Log::error(__PRETTY_FUNCTION__, logres, strerror(errno));
		return -1;
	}
	return 0;
}
int FileHandler::writeByte(uint8_t byte)
{
	if(fwrite(&byte, 1, 1, file)!=1) {
		std::stringstream logres;
		logres << "Unsuccessful fwrite() call for data '" << byte << "'";
		nch::Log::error(__PRETTY_FUNCTION__, logres.str(), strerror(errno));
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

bool FileHandler::fileExists(std::string path)
{
    if(dirExists(path)) {
        return false;
    }

    BTEPath fp(path, filesystemType);
    struct stat buffer;
    std::string btePath = (getModifiedPath(fp));
    return (stat( btePath.c_str(), &buffer) == 0);
}

bool FileHandler::dirExists(std::string path)
{
    BTEPath fp(path, filesystemType);
    std::string mfp = getModifiedPath(fp);

    if(std::filesystem::is_directory(mfp)) {
        return true;
    }
    return false;
}

std::vector<std::string> FileHandler::listDirContents(std::string parentDir, bool listOnlyDirs, bool recursive)
{
    BTEPath fp(parentDir, filesystemType);
    std::string mfp = getModifiedPath(fp);

    std::vector<std::string> res;
    if(dirExists(fp.get())) {

        std::vector<std::filesystem::directory_entry> dev;  //Directory entry vector
        if(recursive) {
            using dritr = std::filesystem::recursive_directory_iterator;
            dritr dirListRec = dritr(mfp);
            for (const auto& dir : dirListRec) { dev.push_back(dir); }
        } else {
            using ditr = std::filesystem::directory_iterator;
            ditr dirList = ditr(mfp);
            for (const auto& dir : dirList) { dev.push_back(dir); }
        }

        for (const auto& dir : dev) {
            std::stringstream ss; ss << dir;
            std::string withoutQuotes = ss.str().substr(1, ss.str().length()-2);
            withoutQuotes = BTEPath::getConvertedPath(withoutQuotes, filesystemType);
            std::string umfp = getUnmodifiedPath(withoutQuotes);

            if(listOnlyDirs) {
                if(dirExists(umfp)) {
                    res.push_back(umfp);
                }
            } else {
                res.push_back(umfp);
            }
        }

    } else {
        nch::Log::warnv(__PRETTY_FUNCTION__, "returning empty vector", "\"%s\" is not a directory", parentDir.c_str());
    }


    return res;
}

std::vector<std::string> FileHandler::listSubDirs(std::string parentDir) { return listDirContents(parentDir, true, false); }

/*
    returns: number of bytes of disk space used by the directory.
*/
uint64_t FileHandler::dirDiskSpaceUsed(std::string parentDir)
{
    saveCloseFile();

    uint64_t res = 0;
    auto dc = listDirContents(parentDir, false, true);
    for(std::string s : dc) {
        std::string umfp = getUnmodifiedPath(s);
        if(fileExists(umfp)) {
            openFile(umfp, FileOpenTypes::READ, true);
            res += getFileLength();
            saveCloseFile();
        }
    }

    

    return res;
}

uint8_t FileHandler::readByte()
{
	uint8_t buffer = 0;
	if( fread(&buffer, sizeof(uint8_t), 1, file)!=1 ) {
		nch::Log::warn(__PRETTY_FUNCTION__, "Failed to read byte from file");
	}
	return buffer;
}

uint8_t FileHandler::readByteStay()
{
	uint8_t byte = readByte();
	seekThru(-1);
	return byte;
}

/**
 * 	Return either the first or second half of readByteStay() (4 bits in total, same as a hex digit)
 * 	If half==true, return the second half.
 */
uint8_t FileHandler::readHexStay(bool half)
{
	DataStream ds;
	ds.putByte( readByteStay() );
	if(half) { ds.seekBitDelta(4); }
	return ds.peekXBits(4);
}

/**
 * 	Return the first 4 bits (first half) of readByteStay()
 */
uint8_t FileHandler::readHex1Stay() { return readHexStay(false); }

/**
 * 	Return the last 4 bits (second half) of readByteStay()
 */
uint8_t FileHandler::readHex2Stay() { return readHexStay(true); }

/**
    t_kvSet: A vector of key-value pairs (both elements of the pair are strings)
    Get contents of a .txt file line by line. Returns empty vector if file loading failed.
    Escape sequences before a newline char or an equals sign char causes those characters to be read in normally.
*/
Settings::t_kvMap FileHandler::readTxtFileKVs(BTEPath fp)
{
    openFile(fp.get(), FileOpenTypes::READ, false);

	//Create an empty t_kvMap
	Settings::t_kvMap contents;

	//Return an empty kvMap if file not found.
	if( !fileExists(fp.get()) ) {
		nch::Log::debug( __PRETTY_FUNCTION__, "File \"%s\" not found", fp.get().c_str());
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

			//In Windows (and others?), end of line is "\r\n". It is "\n" in unix.
			//Thus, we need to remove any \r's (char 13) from currentValue before using it.
			std::string temp = currentValue;
			currentValue = "";
			for( int i = 0; i<temp.size(); i++ ) {
				if( temp[i]!=13 ) {
					currentValue += temp[i];
				}
			}

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

Settings::t_kvMap FileHandler::readTxtFileKVs(std::string path)
{
	return readTxtFileKVs( BTEPath(path, filesystemType) );
}

long FileHandler::tellPos() {
    long res = ftell(file);
    if(res==-1) {
        nch::Log::warnv(__PRETTY_FUNCTION__, "returning 0 instead", "ftell failed");
        return 0;
    }
    return res;
}

long FileHandler::getFileLength()
{
	//Store current pos
    long origPos = tellPos();
	
    //Go to end and store pos of end
    seekToEnd();
	long res = tellPos();

    //Go back to original pos
	seekTo(origPos);

    if(res==-1) {
        nch::Log::warnv(__PRETTY_FUNCTION__, "returning 0 instead", "getFileLength failed");
        return 0;
    }
	return res;
}

int FileHandler::seek(long byte, int seekType)
{
	if( file==nullptr ) {
		nch::Log::error(__PRETTY_FUNCTION__, "File is nullptr");
	}

	if( fseek(file, byte, seekType)!=0 ) {
		std::stringstream logres;
		switch(seekType) {
			case SEEK_SET: logres << "Failed to seek to position " << byte << " in file."; break;
			case SEEK_END: logres << "Failed to seek to end of file."; break;
			case SEEK_CUR: logres << "Failed to seek " << byte << " forward in file."; break;
		}
		nch::Log::warn(__PRETTY_FUNCTION__, logres.str());
		return -1;
	}
	return 0;
}

int FileHandler::seekTo(long byte)
{
	return seek(byte, SEEK_SET);
}
int FileHandler::seekToEnd() { return seek(0, SEEK_END); }
int FileHandler::seekThru(long bytesDelta) { return seek(bytesDelta, SEEK_CUR); }

void FileHandler::saveSettings(Settings::t_kvMap kvMap, std::string path)
{	
	if( kvMap.size()!=0 ) {
	    cEditFile( path );
		for(auto kvp : kvMap) {
            if( kvp.first.size()==0 || kvp.second.size()==0 ) {
                continue;
            }
            
			write(kvp.first);
			write("=");
			write(kvp.second);
			write("\n");
		}

	    saveCloseFile();
	} else {
        nch::Log::warn(__PRETTY_FUNCTION__, "kvMap is empty");
    }
}

void FileHandler::saveSettings(int index)
{
    if( index>=0 && index<Settings::LAST_INDEX ) {
		auto kvMap = settings.getKvMap(index);
		std::string path = (*files[index]).get();
		saveSettings(kvMap, path);

        std::stringstream ss;
        ss << "Successfully saved file with index '" << index << "'.";
    } else {
        std::stringstream ss;
        ss << "Index '" << index << "' doesn't exist.";
        nch::Log::error(__PRETTY_FUNCTION__, ss.str());
    }
}

void FileHandler::saveSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        saveSettings(i);
    }
}

void FileHandler::reloadSettings()
{
    //Load settings and track how much time it takes
    {
        nch::Timer t("reloading settings");
        unloadSettings();
        loadSettings();
    }
}

FILE* FileHandler::getFilePtr() { return file; }
Settings* FileHandler::getSettings() { return &settings; }
std::string FileHandler::getResourcePath() { return resourcePath; }
/**
    Takes a file path and returns it with the base BTE path added at the beginning.
*/
std::string FileHandler::getModifiedPath(BTEPath fp)
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
std::string FileHandler::getUnmodifiedPath(std::string mfp)
{
    int len = resourcePath.length()-1;
    //BTEPath fp(mfp, filesystemType);
    mfp = BTEPath::getConvertedPath(mfp, filesystemType);

    if(mfp.substr(0, len)==resourcePath.substr(0, len)) {
        return mfp.substr(len+1);
    } else {
        //Log::warnv(__PRETTY_FUNCTION__, "returning itself", "Path \"%s\" is not a full path", mfp.c_str());
        return mfp;
    }
}

std::string FileHandler::getFileOpenTypeStr(int fot)
{
	switch(fot) {
	case FileOpenTypes::WRITE:  return "writing";
	case FileOpenTypes::APPEND: return "appending";
	case FileOpenTypes::READ:   return "reading";
	case FileOpenTypes::UPDATE: return "updating";
	}
	return "unknown";
}


/*
    Return numBytes as a string with the format [value]B/KB/MB/GB/TB.
*/
std::string FileHandler::getReadableMemorySize(uint64_t numBytes)
{
    //Get 'display' & 'unitType'
    double display = numBytes;
    int unitType = 0;
    while(display>=1000.0 && unitType<4) {
        unitType++;
        display /= 1000.0;
    }

    //Get 'unitStr'
    std::string unitStr = "";
    switch(unitType) {
        case 0: unitStr = "B"; break;
        case 1: unitStr = "KB"; break;
        case 2: unitStr = "MB"; break;
        case 3: unitStr = "GB"; break;
        case 4: unitStr = "TB"; break;
    }

    //Return display+unitStr
    std::stringstream res;
    res << display << unitStr;
    return res.str();
}

/**
    Create a new directory (folder) at the path.
        - It will also attempt to create parent directories if they don't already exist (only 1 folder per function call).
    Trying to create new directories outside of the main resource path (backtoearth folder) will not work.
        - In most cases just use createBTEDir() instead.
*/
int FileHandler::createDir(std::string path)
{
    // If 'path' is not within the backtoearth folder
    unsigned int minPathSize = resourcePath.length();
    if( path.substr(0, minPathSize)!=resourcePath ) {
        nch::Log::error( __PRETTY_FUNCTION__,
                    "Tried to create a directory outside of the main resource path '"+resourcePath+"'",
                    "If you are seeing this, something is very wrong...");
        nch::Log::throwException();
        return -2;
    }

    // If directory at 'path' isn't found
    if (opendir(path.c_str())==nullptr) {
        if(path==resourcePath) {
            nch::Log::error( __PRETTY_FUNCTION__,
                        "Could not find main directory '"+resourcePath+"'",
                        "Make sure the 'backtoearth' folder is in the same location as the executable");
            nch::Log::throwException();
        }
    }

    // Make the directory...
    // ...for Windows
    #if ( defined(_WIN32) || defined(WIN32) )
        BTEPath fp(path, SDLHandler::WINDOWS);

        int mkdirres = mkdir(fp.get().c_str());
        if(mkdirres==0) {
            return 0;
        }
    // ...for Linux and macOS
    #elif (defined(__unix__) || defined(__unix) || (defined(__APPLE__) && defined(__MACH__)) )
		#if defined(_POSIX_VERSION)
			BTEPath fp(path, SDLHandler::LINUX);
			if(mkdir(fp.get().c_str(), 0775)==0) {
				return 0;
			}
		#else
			nch::Log::error(__PRETTY_FUNCTION__, "POSIX version undefined for this Unix OS, could not create directory");
		#endif
    // ...for something else? -> Error
    #else
		nch::Log::error(__PRETTY_FUNCTION__, "Unknown operating system, could not create directory");
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
