#pragma once
#include <fstream>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include "BTEPath.h"
#include "Settings.h"

class FileHandler
{
public:
/**/
    FileHandler();
    virtual ~FileHandler();
    void init(std::string p_resourcePath, int p_filesystemType);
/**/
    /* File creation/opening, editing, and save/closing */
    int createBTEDir(std::string path);
    int createPNGScreenshot( SDL_Window* w, SDL_Renderer* r, SDL_PixelFormat* pf );
    void openUserLocalURL(std::string url);
    int openFile(std::string path, int openType, bool binary);
    int openFile(std::string path, int openType);
    int cEditFile(std::string path);
    int clearFile(std::string path);
	int cpFile(std::string srcPath, std::string dstPath);
	int mvFile(std::string srcPath, std::string dstPath);
    //Write to file
	template<typename T> int write(T t);
    int writeByte(uint8_t byte);
    int writeChar(char c);
    int writeln(std::string text);
    int writeln();
    //Save and close file
    int saveCloseFile();

	/* File reading and seeking */
    //File exists?
    bool fileExists(std::string path);
    bool dirExists(std::string path);
    std::vector<std::string> listDirContents(std::string parentDir, bool listOnlyDirs, bool recursive);
    std::vector<std::string> listSubDirs(std::string parentDir);
    uint64_t dirDiskSpaceUsed(std::string path);

	//Read file types
	uint8_t readByte();
	uint8_t readByteStay();
	uint8_t readHexStay(bool half);
	uint8_t readHex1Stay();
	uint8_t readHex2Stay();
	Settings::t_kvMap readTxtFileKVs(BTEPath fp);
	Settings::t_kvMap readTxtFileKVs(std::string path);
	//Seek thru files
	long tellPos();						//Get current byte position
	long getFileLength();				//Get current file length
	int seek(long byte, int seekType);	//Seek with options
	int seekTo(long byte);				//Seek to specified byte
	int seekToEnd();					//Seek to end of file
	int seekThru(long bytesDelta);		//Seek forward the specified number of bytes

    /* Settings */
    void saveSettings(Settings::t_kvMap kvMap, std::string path);
    void saveSettings(int index);
    void saveSettings();
    void reloadSettings();

    /* Getters */
    FILE* getFilePtr();
    Settings* getSettings();
    std::string getResourcePath();
    std::string getModifiedPath(BTEPath fp);
    std::string getUnmodifiedPath(std::string mfp);
    static std::string getFileOpenTypeStr(int fot);
    static std::string getReadableMemorySize(uint64_t numBytes);

    enum FileStates {
        FAILED_ACCESS = -1,
        NEW,
        EXISTING,
    };

    enum FileOpenTypes {
        WRITE = 1,
        APPEND = 2,
        READ = 3,
        UPDATE = 4,
    };

private:
    /* Resource path */
    std::string resourcePath;
    int filesystemType = -1;

    /* File */
    FILE* file = nullptr;

    /* Settings handler and files that hold settings */
    Settings settings;
    BTEPath* files[Settings::LAST_INDEX];

    /* Create new folder ("unsafe" version of createBteDir()) */
    int createDir(std::string path);
	
    /* Load and save settings */
    void unloadSettings();
    void loadSettings();
};
