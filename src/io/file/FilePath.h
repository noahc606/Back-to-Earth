#pragma once
#include <string>

class FilePath
{
public:
    FilePath(std::string path, std::string extension, int filesystemType);
    FilePath(std::string path, int filesystemType);

    static std::string getConvertedPath(std::string path, int fileSystemType);
    static std::string getFileExtension(std::string path, int fsType);
    std::string get();

protected:

private:
    std::string path;
};
