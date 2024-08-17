#pragma once
#include <string>

class BTEPath
{
public:
    BTEPath(std::string path, std::string extension, int filesystemType);
    BTEPath(std::string path, int filesystemType);

    static std::string getConvertedPath(std::string path, int fileSystemType);
    static std::string getExtension(std::string path, int fsType);
    static std::string getDirectory(std::string path, int fsType);
    static char getSeparator(int fsType);
    std::string get();

protected:

private:
    std::string path;
};
