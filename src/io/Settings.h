#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <vector>

class Settings
{
public:
    typedef std::vector<std::string> t_kvStrings;
    typedef std::pair<std::string, std::string> t_kvPair;
    typedef std::map<std::string, std::string> t_kvMap;
    typedef std::map<int, t_kvMap> t_settingsMap;

    Settings();

    static void logInfo(t_kvMap kvMap);

    bool unload(int index);
    void loadNewMapIntoOld(t_kvMap* kvMapOldVals, t_kvMap kvMapValsToAdd);
    void load(int index, t_kvMap kvMap);

    t_kvMap getDefaultSettings(int kvMapIndex);
    t_kvMap getKvMap(int kvMapIndex);

    static std::string getKey(t_kvMap kvMap, int index);
    static std::string get(t_kvMap kvMap, std::string key);
    std::string get(int kvMapIndex, std::string key);
    static double getNum(t_kvMap kvMap, std::string key);
    double getNum(int kvMapIndex, std::string key);
    static int64_t getI64(t_kvMap kvMap, std::string key);


    static int find(t_kvMap kvMap, std::string key);
    static void kv(t_kvMap* kvMap, std::string key, std::string val);
    static void kv(t_kvMap* kvMap, std::string key, int val);
    void kv(int kvMapIndex, std::string key, std::string val);
    void kv(int kvMapIndex, std::string key, int val);

    enum TextFiles
    {
		controls,
		games,
		options,
		character,

        //Keep these two last
        session,
        LAST_INDEX,
    };

    static const std::vector<std::string> visibleOptions;

private:
    void loadMap(int index, t_kvMap kvMap);

    bool logging = false;
    t_settingsMap settingsMap;
    t_kvMap defaultSettings[LAST_INDEX];
};
