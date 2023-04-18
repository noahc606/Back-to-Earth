#include "FilePath.h"

FilePath::FilePath(std::string path, std::string extension)
{
    FilePath::path = path+"."+extension; //'extension': can be "txt", "png" or really anything
}

FilePath::FilePath(std::string path):
FilePath::FilePath(path, "txt"){}

std::string FilePath::get() { return path; }

FilePath::~FilePath(){}
