#include "FileHandler.h"
#include <codecvt>
#include <dirent.h>
#include <fstream>
#include <io.h>
#include <iostream>
#include <locale>
#include <SDL_image.h>
#include <sstream>
#include <string.h>
#include "Controls.h"
#include "Log.h"
#include "MainLoop.h"
#include "Timer.h"

FileHandler::FileHandler(){}
FileHandler::~FileHandler(){}

void FileHandler::init( std::string rp )
{
    //Resource path (for saving any type of file to disk)
    resourcePath = rp;

    //Set file paths.
    files[Settings::controls] = "saved\\settings\\controls";
    files[Settings::games] = "saved\\settings\\games";
    files[Settings::options] = "saved\\settings\\options";
    files[Settings::version] = "saved\\settings\\version";
    files[Settings::session] = "saved\\settings\\session";

    //Create directories
    createDir(resourcePath);
    createBteDir("saved\\games");
    createBteDir("saved\\screenshots");
    createBteDir("saved\\settings");

    //Load and save settings files.
    loadSettings();
    saveSettings();
}

int FileHandler::editFile(std::string path, std::string fileFormat)
{
    //If there is another file being edited, save and close it.
    saveAndCloseFile();

    //Set ifstream and ofstream.
    setIFS( path, fileFormat );
    setOFS( path, fileFormat );

    //Try to create new file if file is not found.
    if( !ifs ) {
        Log::debug( __PRETTY_FUNCTION__, "File '"+path+"' not found, attempting to create new file..." );
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
    Opens file to write to (all contents are deleted!)
    returns -1 if file not found and could not be created
    returns 0 if file not found and new file is created
    returns 1 if file found

*/
int FileHandler::editFile(std::string path) { return editFile(path, "txt"); }

void FileHandler::writeChar(char c) { write(c); }
void FileHandler::writeln(std::string text) { write(text+"\n"); }
void FileHandler::writeln() { writeln(""); }
int FileHandler::saveAndCloseFile()
{
    //Close ifstream and ofstream.
    ifs.close();
    ofs.close();

    return 0;
}


/**
    t_kvSet: A vector of key-value pairs (both elements of the pair are strings)
    Get contents of a .txt file line by line. Returns empty vector if file loading failed.
    Escape sequences before a newline char or an equals sign char causes those characters to be read in normally.
*/
Settings::t_kvMap FileHandler::readFileKVs(std::string path)
{
    saveAndCloseFile();

    Settings::t_kvMap contents;

    //Set ifstream (will not create file if nonexistent).
    setIFS( path );

    //return empty kvSet if file not found.
    if( !ifs ) {
        Log::debug( __PRETTY_FUNCTION__, "File '"+path+"' not found" );
        return contents;
    }

    std::string currentKey = "";
    std::string currentValue = "";
    bool foundEscape = false;
    bool foundEqualSign = false;
    bool foundNewLine = false;
    for( char c = ifs.get(); ifs.good(); c = ifs.get() ) {

        //Reset foundNewLine
        foundNewLine = false;

        //If we find an escape character, just add the character normally
        if( c==27 ) {
            if( foundEqualSign ) {
                currentValue += c;
            } else {
                currentKey += c;
            }
        //If we find an equals character
        } else
        if( c=='=' )
        {
            //Edge case: multiple = signs after one another
            if( foundEqualSign ) {
                currentValue += c;
            }
            //foundEqualsSign set to true
            foundEqualSign = true;
        //If we find a newline character
        } else
        if( c=='\n' ) {
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
    }

    //Push back currentKey and currentValue if newline not found
    if( !foundNewLine ) {
        contents.insert( std::make_pair(currentKey, currentValue) );
    }

    return contents;
}

/**
    Create a directory inside of the main resource path (backtoearth folder)
*/
void FileHandler::createBteDir(std::string path)
{
    path = resourcePath+path;

    int x;
    do {
        x = createDir(path);
    } while(x==0);
}

/**
    This function tries to create a new directory (folder) at the specified path. It will also attempt to create parent directories if they don't already exist (only 1 folder at a time).
    It should be impossible to create a new directory outside of the main resource path (backtoearth folder)
*/
int FileHandler::createDir(std::string path)
{
    Log::trbshoot( __PRETTY_FUNCTION__, "Attempting to create new directory at: '"+path+"'..." );

    unsigned int minPathSize = resourcePath.length();

    if( path.substr(0, minPathSize)!=resourcePath ) {
        Log::error(__PRETTY_FUNCTION__, "Tried to create a directory outside of the main resource path '"+resourcePath+"'", "If you are seeing this, something is very wrong");
        Log::throwException();
        return -2;
    }

    if ( opendir(path.c_str())==nullptr ) {
        if(path==resourcePath) {
            Log::error(__PRETTY_FUNCTION__, "Could not find main directory '"+resourcePath+"'", "Make sure the 'backtoearth' folder is in the same location as the executable");
            Log::throwException();
        } else {
            Log::trbshoot(__PRETTY_FUNCTION__, "Could not find directory '"+path+"', attempting to create it...");
        }
    }

    if( mkdir(path.c_str())==0 ) {
        Log::trbshoot( __PRETTY_FUNCTION__, "Successfully created new directory." );
        return 0;
    }

    if(path==resourcePath) {
        return 1;
    }

    std::string subPath = "";
    for( unsigned int i = path.length()-1; i>=minPathSize; i-- ) {
        char c = path.at(i);
        if( c=='\\' ) {
            subPath = path.substr( 0, i );
            break;
        }

        if( i<=minPathSize ) {
            Log::trbshoot( __PRETTY_FUNCTION__, "Ignoring creation of a directory which already exists or has an invalid name", "unfortunately we can't tell the difference with mkdir()" );
            return -1;
        }
    }

    return createDir(subPath);
}

void FileHandler::createPNGScreenshot( SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf )
{
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

    std::string path = resourcePath + "saved\\screenshots\\" + date.substr(firstSpace+1, lastSpace-4) + ".png";

    //Get window width and height
    int width = 0; int height = 0;
    SDL_GetWindowSize(w, &width, &height);

    //Source rect
    SDL_Rect src; src.w = width; src.h = height;

    //Create surface that looks like the current screen
    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat( 0, src.w, src.h, 32, pf->format );
    SDL_RenderReadPixels( r, NULL, pf->format, surf->pixels, surf->pitch );

    //Save img
    IMG_SavePNG(surf, path.c_str());
    std::string msg = "Screenshot saved to: " + path;
    Log::log(msg);

    //Free surface
    SDL_FreeSurface(surf);
}

void FileHandler::reload()
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
    Set an output filestream to a specified path.
    Essentially writes data to a file in the filesystem.
    Will create a new file if one is not already available.
*/
void FileHandler::setOFS( std::string path, std::string fileFormat)
{
    //Modify path
    path = resourcePath+path;
    path = path+"."+fileFormat; //fileFormat: can be "txt", "png" or really anything

    //Open the output filestream
    ofs.open(path, std::fstream::out);
}
void FileHandler::setOFS( std::string path ) { setOFS(path, "txt"); }

/**
    Opens an input filestream at a specified path.
    Essentially takes in data from the filesystem.
    Will not create a new file if one is not already available.
*/
void FileHandler::setIFS( std::string path, std::string fileFormat )
{
    //Modify path
    path = resourcePath+path;
    path = path+"."+fileFormat;

    //Open the input filestream.
    ifs.open( path, std::fstream::in );
}
void FileHandler::setIFS( std::string path ) { setIFS(path, "txt"); }

void FileHandler::unloadSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        settings.unload(i);
    }
}

void FileHandler::loadSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        Settings::t_kvMap kvm = readFileKVs( files[i] );
        settings.load( i, kvm );
    }
}

void FileHandler::saveSettings()
{
    for(int i = 0; i<Settings::LAST_INDEX; i++) {
        editFile( files[i] );
        settings.save( &ofs, i );
    }

    saveAndCloseFile();
}
