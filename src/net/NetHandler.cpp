#include "NetHandler.h"
#include "Log.h"

NetHandler::NetHandler(){}

void NetHandler::init()
{
    eHandle = curl_easy_init();
}

void NetHandler::destroy()
{
    curl_easy_cleanup(eHandle);
    eHandle = nullptr;
}

/*
    Takes in a 'url' (to a web resource) and an easy curl handle ('eHandle'), then populates 'str' with the HTML of that website.
    This is most useful for sites where the entire page is just raw text.
*/
CURLcode NetHandler::cURLAsString(std::string* str, std::string url)
{
    //URL
    curl_easy_setopt(eHandle, CURLOPT_URL, url.c_str());
    
    //Set write function, create string, set writedata to string, then perform
    curl_easy_setopt(eHandle, CURLOPT_WRITEFUNCTION, curlWriteCallbackString);
    std::string res;
    curl_easy_setopt(eHandle, CURLOPT_WRITEDATA, &res);
    CURLcode cc = curl_easy_perform(eHandle);

    //Cleanup
    curl_easy_reset(eHandle);

    //Set transformed str to res and return result
    *str = res;
    return cc;
}

/*
    Takes in a 'url' (to a web resource) and an easy curl handle ('eHandle'), then downloads the file from 'url' into 'out'.
    
    Example: 'out' = "dir1/dir2/icon.png", 'url' = "https://noahc606.github.io/nch/bte/1.1.0a-assets/backtoearth/resources/textures/icon.png"
    
    Note: More likely than not you want to include "https://" at the beginning of the 'url'.
*/
CURLcode NetHandler::cURLIntoFile(std::string out, std::string url)
{
    //Create file and error buffer
    FILE* pFile = fopen(out.c_str(), "w");
    const char* error;

    //Setopt: URL; error buffer; callback function; WRITEDATA into file.
    curl_easy_setopt(eHandle, CURLOPT_ERRORBUFFER, error);
    curl_easy_setopt(eHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(eHandle, CURLOPT_WRITEFUNCTION, curlWriteCallbackData);
    curl_easy_setopt(eHandle, CURLOPT_WRITEDATA, pFile);

    //Perform
    CURLcode cc;
    cc = curl_easy_perform(eHandle);

    //Cleanup
    curl_easy_reset(eHandle);
    if(cc==0) {
        fclose(pFile);
    } else {
        Log::errorv(__PRETTY_FUNCTION__, error, "Nonzero CURLcode %d", cc);
        fclose(pFile);
    }

    //Return CURLcode
    return cc;
}



/*
    Private callback function for writing to a string
*/
size_t NetHandler::curlWriteCallbackString(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    try {
        s->append((char*)contents, newLength);
    }
    catch(std::bad_alloc &e) {
        Log::error(__PRETTY_FUNCTION__, "Bad allocation");
        return 0;
    }
    return newLength;
}

/*
    Private callback function for writing data with fwrite
*/
size_t NetHandler::curlWriteCallbackData(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}