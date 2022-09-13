#pragma once
#include <map>
#include <mem.h>
#include <string>
#include <vector>

class Settings
{
public:
    typedef std::pair< std::string, std::string > t_kvPair;
    typedef std::map< std::string, std::string > t_kvMap;
    typedef std::map<int, t_kvMap> t_settingsMap;

    Settings();

    bool unload(int index);
    bool load(int index, t_kvMap kvMap);
    bool save( std::ofstream* ofs, int index );

    void setSettings(std::string file, t_kvMap kvMap);
    std::string get(int index, std::string key);
    t_kvMap getKvMap(int index);

    enum TextFiles
    {
        controls,
        games,
        options,
        version,

        //Keep these two last
        session,
        LAST_INDEX,
    };

private:
    void loadMap(int index, t_kvMap kvMap);
    void loadDefault(int index);
    std::string get(t_kvMap kvMap, std::string key);
    void addTo(t_kvMap* kvMap, std::string key, std::string val);
    void addTo(t_kvMap* kvMap, std::string key, int val);

    bool logging = false;
    t_settingsMap settingsMap;
    t_kvMap defaultSettings[LAST_INDEX];
};
