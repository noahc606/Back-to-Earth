#include "FileHandler.h"
#include <bits/stdc++.h>
#include <codecvt>
#include <dirent.h>
#include <fstream>
#include <iostream>
#include <locale>
#include <SDL_image.h>
#include <sstream>
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
    files[Settings::controls] = new FilePath("saved/settings/controls", filesystemType);
    files[Settings::games] =    new FilePath("saved/settings/games", filesystemType);
    files[Settings::options] =  new FilePath("saved/settings/options", filesystemType);
    files[Settings::version] =  new FilePath("saved/settings/version", filesystemType);
    files[Settings::session] =  new FilePath("saved/settings/session", filesystemType);

    //Create directories
    createDir(resourcePath);
    createBteDir("dump");
    createBteDir("saved/games");
    createBteDir("saved/screenshots");
    createBteDir("saved/settings");

    //Load and save settings files.
    loadSettings();
    saveSettings();
}

/**
    Create a directory inside of the main resource path (backtoearth folder)
        - Also tries to create parent directories.
*/
int FileHandler::createBteDir(std::string path)
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
    /* Build final file path */
    std::string date = MainLoop::getSystemTime();
    int firstSpace = -1;
    int lastSpace = -1;
    for(unsigned int i = 0; i<date.length(); i++) {
        if( date[i]==' ' ) {
            lastSpace = i;
        }

        if( firstSpace<0 && date[i]==' ' ) {
            firstSpace = i;
        }

        if( date[i]==':' ) {
            date[i] = '_';
        }
    }

    FilePath ssPath(resourcePath + "saved/screenshots/" + date.substr(firstSpace+1, lastSpace-4), "png", filesystemType);

    /* Get window width and height */
    int width = 0; int height = 0;
    SDL_GetWindowSize(w, &width, &height);

    /* Create surface that looks like the current screen */
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat( 0, width, height, 32, pf->format );
    SDL_RenderReadPixels( r, NULL, pf->format, surf->pixels, surf->pitch );

    /* Save the newly created surface as an image */
    //Success
    if( IMG_SavePNG(surf, ssPath.get().c_str())==0 ) {
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

/**
    Opens file to write to (all contents are deleted!)
    Returns:
        -1 if file not found and could not be created
        0 if file not found and new file is created
        1 if file found
*/
int FileHandler::cEditFile(FilePath fp)
{
    //If there is another file being edited, save and close it.
    saveAndCloseFile();

    //Set ifstream and ofstream.
    setIFS(fp);
    setOFS(fp);

    //Try to create new file if file is not found.
    if( !ifs ) {
        Log::debug( __PRETTY_FUNCTION__, "File '"+fp.get()+"' not found, attempting to create new file..." );
        if( !ofs ) {
            //If file was not found and could not be created, return -1.
            Log::warn( __PRETTY_FUNCTION__, "Could not create ofstream" );
            return -1;
        }
        //If file was created, return 1.
        Log::debug( __PRETTY_FUNCTION__, "Created new file." );
        return 0;
    }

    //If file was found, return 1.
    return 1;
}

/**
    Edit part of a file.
        - Does not delete the entire file's contents but is a little more complex to use than cEditFile()
*/
int FileHandler::pEditFile(FilePath fp)
{
    saveAndCloseFile();
    setFS(fp);

    if( !fs.is_open() ) {
        Log::error(__PRETTY_FUNCTION__, "fstream failed to open");
        return -1;
    }

    return 1;
}

/**
    Edit a mapped file.
*/
int FileHandler::pEditMappedFile(FilePath fp)
{
    if( pEditFile(fp)==-1) {
        return -1;
    }

    int mappedLines = pBuildLineMap();
    if(mappedLines==0) {

    }

    return 0;
}


int FileHandler::pSeek(int seekPos) { fs.seekp(seekPos); return fs.tellp(); }
int FileHandler::pSeekDelta(int seekPosDelta) { return pSeek(getPSeekPos()+seekPosDelta);  }
int FileHandler::pSeekNextLine()
{
    if(usingLineMap) {
        auto kv = lineMap.find(currentLine+1);
        if( kv!=lineMap.end() ) {
            pSeek( kv->first );
            currentLine += 1;
            return currentLine;
        }
        return -1;
    }
    return -2;
}

int FileHandler::saveAndCloseFile()
{
    //Close ifstream, ofstream, and fstream.
    ifs.close();
    ofs.close();
    fs.close();

    return 0;
}

int FileHandler::saveSettings(int index)
{
    int success = -1;

    if( index>=0 && index<Settings::LAST_INDEX ) {
        cEditFile( *files[index] );

        auto kvMap = settings.getKvMap(index);
        if( kvMap.size()!=0 ) {
            for(auto kvp : kvMap) {
                cWrite(kvp.first);
                cWrite("=");
                cWrite(kvp.second);
                cWrite("\n");
            }
            success = 1;
        }

        saveAndCloseFile();
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
    ss << "Failed to save " << fails << " files, something is terribly wrong";
    Log::error(__PRETTY_FUNCTION__, ss.str());

    return -fails;
}

/**
    t_kvSet: A vector of key-value pairs (both elements of the pair are strings)
    Get contents of a .txt file line by line. Returns empty vector if file loading failed.
    Escape sequences before a newline char or an equals sign char causes those characters to be read in normally.
*/
Settings::t_kvMap FileHandler::readFileKVs(FilePath fp)
{
    saveAndCloseFile();

    Settings::t_kvMap contents;

    //Set ifstream (will not create file if nonexistent).
    setIFS(fp);

    //return empty kvSet if file not found.
    if( !ifs ) {
        Log::debug( __PRETTY_FUNCTION__, "File '"+fp.get()+"' not found" );
        return contents;
    }

    std::string currentKey = "";
    std::string currentValue = "";
    bool doEscape = false;
    bool foundEqualSign = false;
    bool foundNewLine = false;
    for( char c = ifs.get(); ifs.good(); c = ifs.get() ) {

        //Reset foundNewLine
        foundNewLine = false;

        //If we find an escape character, go to the next character and add it normally
        if( c==27 && !doEscape ) {
            doEscape = true;
            continue;
        //If we find an equals character that isn't escaped
        } else
        if( c=='=' && !doEscape )
        {
            //Edge case: multiple = signs after one another
            if( foundEqualSign ) {
                currentValue += c;
            }
            //foundEqualsSign set to true
            foundEqualSign = true;
        //If we find a newline character that isn't escaped
        } else
        if( c=='\n' && !doEscape ) {
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

Settings::t_kvStrings FileHandler::readFileLines(FilePath fp)
{
    saveAndCloseFile();

    Settings::t_kvStrings res;
    //Set ifstream (will not create file if nonexistent).
    setIFS(fp);

    for(std::string line; std::getline(ifs, line);) {
        res.push_back(line);
    }

    return res;
}

int FileHandler::getPSeekPos() { return fs.tellp(); }
std::string FileHandler::pRead(int readLen)
{
    std::string buffer(readLen, ' ');
    fs.read(&buffer[0], readLen);
    return buffer;
}
//int FileHandler::getFsSeekPos(std::fstream* p_fs) { return p_fs->tellp(); }
//std::string FileHandler::fsRead(std::fstream* p_fs, int readLen) { std::string buffer(readLen, ' '); p_fs->read(&buffer[0], readLen); return buffer; }

Settings* FileHandler::getSettings() { return &settings; }
std::string FileHandler::getResourcePath() { return resourcePath; }
/**
    Takes a file path and returns it with the base BTE path added at the beginning
*/
std::string FileHandler::getModifiedPath(FilePath fp)
{
    //Build path
    std::string mp;
    mp = resourcePath+fp.get();
    return mp;
}

void FileHandler::cWriteChar(char c) { cWrite(c); }
void FileHandler::cWriteln(std::string text) { cWrite(text+"\n"); }
void FileHandler::cWriteln() { cWriteln(""); }
void FileHandler::pWrite(std::string s) { fs << s; }

void FileHandler::reload()
{
    //Load settings and track how much time it takes
    {
        Timer t("reloading settings");
        unloadSettings();
        loadSettings();
    }
}

/**
    Create a new directory (folder) at the path.
        - It will also attempt to create parent directories if they don't already exist (only 1 folder per function call).
    You should never be creating new directories outside of the main resource path (backtoearth folder)
        - In most cases just use createBteDir() instead.
*/
int FileHandler::createDir(std::string path)
{
    Log::trbshoot(__PRETTY_FUNCTION__, "Attempting to create new directory at: '"+path+"'..." );

    // If 'path' is not within 'resourcePath'
    unsigned int minPathSize = resourcePath.length();
    if( path.substr(0, minPathSize)!=resourcePath ) {
        Log::error( __PRETTY_FUNCTION__,
                    "Tried to create a directory outside of the main resource path '"+resourcePath+"'",
                    "If you are seeing this, something is very wrong...");
        Log::throwException();
        return -2;
    }

    // If directory at 'path' isn't found
    if ( opendir(path.c_str())==nullptr ) {
        if(path==resourcePath) {
            Log::error( __PRETTY_FUNCTION__,
                        "Could not find main directory '"+resourcePath+"'",
                        "Make sure the 'backtoearth' folder is in the same location as the executable");
            Log::throwException();
        } else {
            Log::trbshoot(__PRETTY_FUNCTION__, "Could not find directory '"+path+"', attempting to create it...");
        }
    }

    // If filesystem type is Windows
    if(filesystemType==SDLHandler::WINDOWS) {

    // If filesystem type is Linux
    } else if(filesystemType==SDLHandler::LINUX) {
        // Convert all '\\'s into '/'.
        for(unsigned i = 0; i<path.length(); i++) {
            if( path[i]=='\\' ) {
                path[i] = '/';
            }
        }

        int mkdirResult = mkdir(path.c_str(), 0775);
        if( mkdirResult==0 ) {
            Log::trbshoot( __PRETTY_FUNCTION__, "Successfully created new directory." );
            return 0;
        } else {
            Log::trbshoot( __PRETTY_FUNCTION__, "Found directory, doing nothing.");
        }
    // If filesystem type is unknown
    } else {
    }



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

void FileHandler::pDiscardLineMap()
{
    currentLine = -1;
    usingLineMap = false;

    while(lineMap.begin()!=lineMap.end()) {
        lineMap.erase(lineMap.begin());
    }
}

/**
    A "mapped file" always looks something like this (line by line):
    [000010]! hello
    [000026]! the text portion of this line has 84 characters, including the newline right here->
    [000120]! The number within the 10 char 'prefix' of this line indicates the position of the beginning of the text portion of this line (in this case it is right before "The")
    [000295]! There are 295 characters before the beginning of the text portion of this line.

    This function goes through a mapped file and creates a map<int, int> where int1 = the line # and int2 = the char position stored within that line prefix

*/
int FileHandler::pBuildLineMap()
{
    //If FileStream isn't open
    if( !fs.is_open() ) {
        Log::warn(__PRETTY_FUNCTION__, "FS (filestream) must be used instead of IFS or OFS");
        return -1;
    }

    //Discard any old line maps
    pDiscardLineMap();

    usingLineMap = true;

    //# of first line = 0
    currentLine = 0;
    pSeek(0);
    for( int num = 0;; currentLine++ ) {
        //Read in first 10 chars
        std::string linePrefix = pRead(10);
        //Test if this is a proper line prefix
        if( linePrefix[0]=='[' && linePrefix.substr(7)=="]! " ) {
            lineMap.insert( std::make_pair(currentLine, num+10) );
            num = std::stoi( linePrefix.substr(1, 6) );
            pSeekDelta(num+10);
        } else {
            break;
        }
    }

    int res = currentLine;
    currentLine = 0;
    return res;
}

/**
    Opens an input filestream at a path.
    Will not create a new file if one is not already available.
*/
void FileHandler::setIFS(FilePath fp) { ifs.open(getModifiedPath(fp), std::ios::in); }

/**
    Set an output filestream to a path.
    Will create a new file if one is not already available.
*/
void FileHandler::setOFS(FilePath fp) { ofs.open(getModifiedPath(fp), std::ios::out); }

/**
    Set a filestream to a specified path.
        - More complex to use than ifs/ofs but is is useful in editing a part of a large file.
        - Use this if you really don't want to rewrite the entire contents of a file every time you open it.
    Will create a new file if one is not already available.
*/
void FileHandler::setFS(FilePath fp) { fs.open(getModifiedPath(fp), fs.in | fs.out ); }

void FileHandler::unloadSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        settings.unload(i);
    }
}

void FileHandler::loadSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        Settings::t_kvMap kvm = readFileKVs( *(files[i]) );
        settings.load( i, kvm );
    }
}
