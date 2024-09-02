#include "BTEPath.h"
#include <nch/cpp-utils/io/Log.h>
#include <sstream>
#include "SDLHandler.h"

BTEPath::BTEPath(std::string path, std::string extension, int fsType)
{
    BTEPath::path = path+"."+extension; //'extension': can be "txt", "png" or really anything
    BTEPath::path = getConvertedPath(BTEPath::path, fsType);
}

BTEPath::BTEPath(std::string path, int fsType)
{
	BTEPath::path = getConvertedPath(path, fsType);
}

/**
    Return formatted version of a path string.
    Depending on the file system (Windows or Linux), either backslashes or forward slashes may be used by filestream functions.
    This function ensures that any type of slash can be used when working with a path string.
*/
std::string BTEPath::getConvertedPath(std::string path, int fsType)
{
    std::string res = path;

    if(fsType==SDLHandler::WINDOWS) {
		//Convert any type of slash to backslash ('\\')
        for( unsigned int i = 0; i<res.size(); i++ ) {
            if( res[i]=='\\' || res[i]=='/' ) {
                res[i] = '\\';
            }
        }

        //Replace all double-backslashes with a single backslash
        size_t start = 0;
        while((start = res.find("\\\\", start)) != std::string::npos)
        {
            res.replace(start, 2, "\\");
            start += 1;
        }

    } else if(fsType==SDLHandler::LINUX) {
        //Convert any type of slash to frontslash ('/')
        for( unsigned int i = 0; i<res.size(); i++ ) {
            if( res[i]=='\\' || res[i]=='/' ) {
                res[i] = '/';
            }
        }
    } else {
        std::stringstream ss;
        ss << "Unrecognized filesystem type '" << fsType << "'";
        nch::Log::warn(__PRETTY_FUNCTION__, ss.str(), "getting original string");
    }

    return res;
}

std::string BTEPath::getExtension(std::string path, int fsType)
{
    //Get path seperator character
    char pathSeperator = getSeparator(fsType);
    path = getConvertedPath(path, fsType);

    //Find extension by starting at the end and going back until we find "."
    int dotIndex = -1;
    for( int i = (int)path.size()-1; i>=0; i-- ) {
        //If we find a period, break out of the function
        if(path[i]=='.') {
            dotIndex = i;
            break;
        //If we find any type of slash, break out of the function
        } else if(path[i]==pathSeperator) {
            dotIndex = -1;
            break;
        }
    }

    //-1 If we didn't find a file extension
    if( dotIndex==-1 || dotIndex==(int)path.size()-1 ) {
        return "";
    } else {
        return path.substr(dotIndex+1);
    }
}

std::string BTEPath::getDirectory(std::string path, int fsType)
{
    char pathSeperator = getSeparator(fsType);
    path = getConvertedPath(path, fsType);

    //Find directory by starting at the end and going back until we find "/"
    for( int i = (int)path.size()-1; i>=0; i-- ) {
        //If we find a slash, return the path of the directory
        if(path[i]==pathSeperator) {
            return path.substr(0, i);
        }
    }

    return "";
}

char BTEPath::getSeparator(int fsType)
{
    //Get path seperator character
    char res = '/';
    if(fsType==SDLHandler::WINDOWS) {
        res = '\\';
    }
    return res;
}

std::string BTEPath::get()
{
    return path;
}
