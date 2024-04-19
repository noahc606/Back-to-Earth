#pragma once
#include <curl/curl.h>
#include <string>
#include <vector>
#include "SDLHandler.h"

class CurlHandler
{
public:
	CurlHandler();
	void init(SDLHandler* sh);
	void destroy();

	CURLcode cURLAsString(std::string* str, std::string url);
	CURLcode cURLIntoFile(std::string out, std::string url);
	bool v1NewerThanV2(std::string v1, std::string v2);
 	bool newBTEVersionAvailable(std::string* newVersion);
	std::vector<std::string> getBTEAssetPathList();
	std::vector<std::string> getBTEDirList(std::vector<std::string> assetPathList);

private:
	bool initEHandle();
	static size_t curlWriteCallbackString(void *contents, size_t size, size_t nmemb, std::string *s);
	static size_t curlWriteCallbackData(void* ptr, size_t size, size_t nmemb, FILE* stream);
	CURLcode cURLAsStringTBR(std::string* str, std::string url);
	CURLcode cURLIntoFileTBR(std::string out, std::string url);
 
	CURL* eHandle = nullptr;
	SDLHandler* sdlHandler = nullptr;
	static bool globalInitState;
};