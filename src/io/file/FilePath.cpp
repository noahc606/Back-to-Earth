#include "FilePath.h"
#include <sstream>
#include "Log.h"
#include "SDLHandler.h"

FilePath::FilePath(std::string path, std::string extension, int fsType)
{
    FilePath::path = path+"."+extension; //'extension': can be "txt", "png" or really anything

    FilePath::path = getConvertedPath(FilePath::path, fsType);
}

FilePath::FilePath(std::string path, int fsType):
FilePath::FilePath(path, "txt", fsType){}

std::string FilePath::getConvertedPath(std::string path, int fsType)
{
    std::string res = path;

    if(fsType==SDLHandler::WINDOWS) {
        //Convert any type of slash to backslash ('\\')
        for(unsigned int i = 0; i<res.size(); i++) {
            if( res[i]=='\\' || res[i]=='/' ) {
                res[i] = '\\';
            }
        }
    } else
    if(fsType==SDLHandler::LINUX) {
        //Convert any type of slash to frontslash ('/')
        for(unsigned int i = 0; i<res.size(); i++) {
            if( res[i]=='\\' || res[i]=='/' ) {
                res[i] = '/';
            }
        }
    } else {
        std::stringstream ss;
        ss << "Unrecognized filesystem type '" << fsType << "'";
        Log::warn(__PRETTY_FUNCTION__, ss.str(), "getting original string");
    }

    return res;
}

std::string FilePath::get()
{
    return path;
}

FilePath::~FilePath(){}
