#pragma once
#include <fstream>
#include <SDL.h>
#include <string>
#include <vector>
#include "Settings.h"

class FileHandler
{
public:


    FileHandler();
    virtual ~FileHandler();
    void init(std::string p_resourcePath);

    /* File editing */
    int editFile(std::string path, std::string fileFormat);
    int editFile(std::string path);

    template<typename T> void write(T t){ ofs<<t; };
    void writeChar(char c);
    void writeln(std::string text);
    void writeln();
    int saveAndCloseFile();
    int saveSettings(int index);
    int saveSettings();

    Settings::t_kvMap readFileKVs(std::string path, std::string fileFormat);
    Settings::t_kvMap readFileKVs(std::string path);

    //Create new folder
    void createBteDir(std::string path);
    //Take & save screenshot
    void createPNGScreenshot( SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf );
    //Reload settings
    void reload();

    Settings* getSettings();
    std::string getResourcePath();

private:
    /* Set output file stream and input file stream */
    void setOFS( std::string path, std::string fileFormat );
    void setOFS( std::string path );
    void setIFS( std::string path, std::string fileFormat );
    void setIFS( std::string path );

    /* Create a new file/folder in disk */
    //Create new folder
    int createDir( std::string path );

    /* Load and save settings */
    void unloadSettings();
    void loadSettings();

    /* Resource path */
    std::string resourcePath;


    /* Input file stream and Output file stream */
    std::ifstream ifs;
    std::ofstream ofs;

    /* Settings handler and files that hold settings */
    Settings settings;
    std::string files[Settings::LAST_INDEX];
};
