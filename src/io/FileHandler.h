#pragma once
#include <fstream>
#include <SDL.h>
#include <string>
#include <vector>
#include "FilePath.h"
#include "Settings.h"

class FileHandler
{
public:
/**/
    FileHandler();
    virtual ~FileHandler();
    void init(std::string p_resourcePath, int p_filesystemType);
/**/
    /* File creation + editing */
    int createBteDir(std::string path);
    int createPNGScreenshot( SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf );
    int cEditFile(FilePath fp);
    int pEditFile(FilePath fp);

    //Seek thru file
    static int fsSeek(std::fstream* p_fs, int seekPos);
    int seek(int seekPos);
    int seekD(int seekPosDelta);
    int seekNextLine();

    //Save and close file
    int saveAndCloseFile();

    /* Settings */
    int saveSettings(int index);
    int saveSettings();
/**/
    /* File reading, getting seek position */
    Settings::t_kvMap readFileKVs(FilePath fp);
    Settings::t_kvStrings readFileLines(FilePath fp);
    int getSeekPos();
    static int getFsSeekPos(std::fstream* p_fs);
    std::string read(int readLen);
    static std::string fsRead(std::fstream* fs, int readLen);

    /* Get settings and resource path */
    Settings* getSettings();
    std::string getResourcePath();
    std::string getModifiedPath(FilePath fp);
/**/
    template<typename T> void write(T t){ ofs<<t; };
    void writeChar(char c);
    void writeln(std::string text);
    void writeln();

    //Line map
    void discardLineMap();
    int buildLineMap();

    //Reload settings
    void reload();

private:
/**/
    /* Create a new folder in disk */
    //Create new folder
    int createDir( std::string path );
/**/
/**/
    /* Set output file stream and input file stream */
    void setIFS(FilePath fp);
    void setOFS(FilePath fp);
    void setFS(FilePath fp);

    /* Load and save settings */
    void unloadSettings();
    void loadSettings();
/**/
    /* Resource path */
    std::string resourcePath;
    int filesystemType;

    /* file stream types */
    std::ifstream ifs;
    std::ofstream ofs;
    std::fstream fs;

    /* Line mapping for large files */
    bool usingLineMap = false;
    std::map<int, int> lineMap;
    int currentLine = 0;


    /* Settings handler and files that hold settings */
    Settings settings;
    FilePath* files[Settings::LAST_INDEX];
};
