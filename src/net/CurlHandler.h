#pragma once
#include <curl/curl.h>
#include <string>
#include <vector>

class CurlHandler
{
public:
	CurlHandler();
	void init();
	void destroy();

	CURLcode cURLAsString(std::string* str, std::string url);
	CURLcode cURLIntoFile(std::string out, std::string url);
	bool newBTEVersionAvailable();
	std::vector<std::string> getBTEDirList();
	std::vector<std::string> getBTEAssetPathList();

private:
	static size_t curlWriteCallbackString(void *contents, size_t size, size_t nmemb, std::string *s);
	static size_t curlWriteCallbackData(void* ptr, size_t size, size_t nmemb, FILE* stream);

	CURL* eHandle = nullptr;
};