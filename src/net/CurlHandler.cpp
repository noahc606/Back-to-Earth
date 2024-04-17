#include "CurlHandler.h"
#include "Log.h"

CurlHandler::CurlHandler(){}

void CurlHandler::init(SDLHandler* sh)
{
    eHandle = curl_easy_init();
    sdlHandler = sh;
}

void CurlHandler::destroy()
{
    curl_easy_cleanup(eHandle);
    eHandle = nullptr;
}

/*
    Takes in a 'url' (to a web resource) and an easy curl handle ('eHandle'), then populates 'str' with the HTML of that website.
    This is most useful for sites where the entire page is just raw text.
    
    Note: You should include "https://" at the beginning of the 'url'.
*/
CURLcode CurlHandler::cURLAsString(std::string* str, std::string url)
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
    Note: You should include "https://" at the beginning of the 'url'.
*/
CURLcode CurlHandler::cURLIntoFile(std::string out, std::string url)
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
    Is version 1 (v1) newer than version 2 (v2)?
    Version format must be: [supermajor].[major].[minor]["a"/"b"/nothing].
    [supermajor] will == "1" for... forever probably. a/b/nothing refers to alpha/beta/release.

    Note that input validation doesn't happen in this function. 
*/
bool CurlHandler::v1NewerThanV2(std::string v1, std::string v2)
{
    //Build 'vs' ("version splits") which is a vector of string splits
    std::vector<std::vector<std::string>> vs;
    vs.push_back(FileHandler::split(v1, "."));
    vs.push_back(FileHandler::split(v2, "."));
    for(int i = 0; i<2; i++) {
        char x = vs[i][2][vs[i][2].size()-1];
        if(x=='a'||x=='b') {
            vs[i][2] = vs[i][2].substr(0, vs[i][2].size()-1);
            std::stringstream xs; xs << x;
            vs[i].push_back(xs.str());
        } else {
            vs[i].push_back("c");
        }
    }
    
    //Get v1 and v2's supermajor, major, and minor numbers
    int v1sm; int v1ma; int v1mi;
    int v2sm; int v2ma; int v2mi;
    try {
        v1sm = std::stoi(vs[0][0]); v1ma = std::stoi(vs[0][1]); v1mi = std::stoi(vs[0][2]);
        v2sm = std::stoi(vs[1][0]); v2ma = std::stoi(vs[1][1]); v2mi = std::stoi(vs[1][2]);
    } catch(...) {}

    //Compare
    if(vs[0][3]=="c" && (vs[1][3]=="a"||vs[1][3]=="b")) { return true; }    //Release newer than alpha/beta
    if(vs[0][3]=="b" && (vs[1][3]=="a"))                { return true; }    //Beta newer than alpha
    if(v1sm>v2sm)   { return true; }  //Compare supermajor
    if(v1ma>v2ma)   { return true; }  //Compare major
    if(v1mi>v2mi)   { return true; }  //Compare minor
    return false;
}

/*
    Go to noahc606.github.io/nch/bte/versions.txt and see if a newer version is listed there (higher than the current version defined by Main::VERSION_LABEL)
*/
bool CurlHandler::newBTEVersionAvailable()
{
    //Get version list as a split string
    std::string versionList;
    cURLAsString(&versionList, "https://noahc606.github.io/nch/bte/versions.txt");
    auto svl = FileHandler::split(versionList, "\n");   //'svl': 'split versionList'

    //Find maximum version within list
    std::string max = "0.0.0a";
    for(int i = 0; i<svl.size(); i++) {
        if(v1NewerThanV2(svl[i], max)) {
            max = svl[i];
        }
    }

    //Return whether we found a newer version or not
    if(v1NewerThanV2(max, Main::VERSION_LABEL)) {
        Log::log("Found newer version \"%s\" at noahc606.github.io/nch/bte/versions.txt. Maybe you should update.", max.c_str());
        return true;
    }
    return false;
}

std::vector<std::string> CurlHandler::getBTEDirList()
{
    std::vector<std::string> res;
    return res;
}

std::vector<std::string> CurlHandler::getBTEAssetPathList()
{
    std::vector<std::string> res;
    return res;
}
/*
    Private callback function for writing to a string
*/
size_t CurlHandler::curlWriteCallbackString(void *contents, size_t size, size_t nmemb, std::string *s)
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
size_t CurlHandler::curlWriteCallbackData(void* ptr, size_t size, size_t nmemb, FILE* stream)
{
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}