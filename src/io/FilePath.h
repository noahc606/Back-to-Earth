#pragma once
#include <string>

class FilePath
{
public:
    FilePath(std::string path, std::string extension);
    FilePath(std::string path);
    virtual ~FilePath();

    std::string get();

protected:

private:
    std::string path;
};
