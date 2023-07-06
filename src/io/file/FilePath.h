#pragma once
#include <string>

class FilePath
{
public:
    FilePath(std::string path, std::string extension, int filesystemType);
    FilePath(std::string path, int filesystemType);
    virtual ~FilePath();

    static std::string getConvertedPath(std::string path, int fileSystemType);
    std::string get();

protected:

private:
    std::string path;
};
