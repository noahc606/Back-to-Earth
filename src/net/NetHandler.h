#pragma once
#include <curl/curl.h>
#include <string>

class NetHandler
{
public:
	NetHandler();
	void init();
	void destroy();

	CURLcode cURLAsString(std::string* str, std::string url);
	CURLcode cURLIntoFile(std::string out, std::string url);

private:
	static size_t curlWriteCallbackString(void *contents, size_t size, size_t nmemb, std::string *s);
	static size_t curlWriteCallbackData(void* ptr, size_t size, size_t nmemb, FILE* stream);

	CURL* eHandle = nullptr;
};