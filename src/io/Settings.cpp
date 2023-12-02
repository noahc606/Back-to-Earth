#include "Settings.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include "Log.h"
#include "Main.h"
#include "MainLoop.h"

Settings::Settings()
{
    t_kvMap* s = &(defaultSettings[controls]);

	/* Debugging, UI. */
    kv(s, "FUNC_8", SDLK_F8);
    kv(s, "FUNC_9", SDLK_F9);
    kv(s, "FUNC_SCREENSHOT", SDLK_F10);
    kv(s, "FUNC_FULLSCREEN", SDLK_F11);
    kv(s, "FUNC_DEBUG", SDLK_F12);
    kv(s, "INGAME_PAUSE", SDLK_ESCAPE);
    kv(s, "MAP_USE_ZOOM", SDLK_LCTRL);
	kv(s, "NAV_NEXT_UI", SDLK_TAB);
	kv(s, "INTERACT_UI", SDLK_RETURN);

    /** Player */
    //Movement
    kv(s, "PLAYER_MOVE_NORTH", SDLK_w);
    kv(s, "PLAYER_MOVE_SOUTH", SDLK_s);
    kv(s, "PLAYER_MOVE_WEST", SDLK_a);
    kv(s, "PLAYER_MOVE_EAST", SDLK_d);
    kv(s, "PLAYER_MOVE_UP", SDLK_UP);
    kv(s, "PLAYER_MOVE_DOWN", SDLK_DOWN);
    kv(s, "PLAYER_MOVE_VERTICAL", SDLK_SPACE);

    //Advanced Movement
    kv(s, "PLAYER_JUMP", SDLK_SPACE);
    kv(s, "PLAYER_SPRINT", SDLK_x);
    kv(s, "PLAYER_CROUCH", SDLK_c);

    //Player Interactions
    kv(s, "PLAYER_INVENTORY", SDLK_LSHIFT);
    kv(s, "PLAYER_SELECT", "MOUSE_LEFT");
    kv(s, "PLAYER_VIEW", SDLK_v);

	//Hardcoded controls
    kv(s, "HARDCODE_LEFT_CLICK", "MOUSE_LEFT");
    kv(s, "HARDCODE_MIDDLE_CLICK", "MOUSE_MIDDLE");
    kv(s, "HARDCODE_RIGHT_CLICK", "MOUSE_RIGHT");

    /** Number keys */
    kv(s, "DEBUG_1", SDLK_1);
    kv(s, "DEBUG_2", SDLK_2);
    kv(s, "DEBUG_3", SDLK_3);
    kv(s, "DEBUG_4", SDLK_4);
    kv(s, "DEBUG_5", SDLK_5);
    kv(s, "DEBUG_6", SDLK_6);
    kv(s, "DEBUG_7", SDLK_7);
    kv(s, "DEBUG_8", SDLK_8);
    kv(s, "DEBUG_9", SDLK_9);
    kv(s, "DEBUG_0", SDLK_0);
    kv(s, "TEST1234", SDLK_BACKSPACE);

    s = &(defaultSettings[games]);
    kv(s, "needTutorial", "true");

    s = &(defaultSettings[options]);
    kv(s, "fullscreen", "false");
    kv(s, "maxFPS", "none");
    kv(s, "bteCursor", "true");
    kv(s, "debugEnabled", "false");
    kv(s, "debugHacks", "typeincorrectpassword");
    kv(s, "debugTesting", "false");
    kv(s, "debugHardTesting", "false");

    s = &(defaultSettings[version]);
    kv(s, "version", Main::VERSION);

    s = &(defaultSettings[session]);
    kv(s, "date", MainLoop::getSystemTime() );
}

bool Settings::unload(int index)
{
	//Erase all elements in settingsMap.
    t_settingsMap::iterator itr = settingsMap.find(index);
    if( itr!=settingsMap.end() ) {
        settingsMap.erase(itr);
        return true;
    }

    return false;
}

bool Settings::load(int index, t_kvMap kvMap)
{
    //Get the default settings just in case there is a missing setting (for example: when there is an update new settings might be added)
    t_kvMap settingSet = defaultSettings[index];

    //Load in the default kv pairs.
    if( settingSet.size()!=0 ) {

        if(logging) {
            std::cout << "\nDefault kvMap:\n";
            for( t_kvMap::iterator itr = settingSet.begin(); itr!=settingSet.end(); itr++ ) {
                std::cout << itr->first << ", " << itr->second << "\n";
            }
        }

        loadMap(index, settingSet);
    } else {
        std::stringstream ss;
        ss << "Default key-value pairs for setting set '" << index << "' doesn't exist";
        Log::error(__PRETTY_FUNCTION__, ss.str() );
    }

    if(logging) {
        std::cout << "\nRead in kvMap:\n";
        for( t_kvMap::iterator itr = kvMap.begin(); itr!=kvMap.end(); itr++ ) {
            std::cout << itr->first << ", " << itr->second << "\n";
        }
    }


    //Load in kv pairs from the file if they exist.
    loadMap(index, kvMap);

    if(logging) {
        t_settingsMap::iterator smItr = settingsMap.find(index);
        if( smItr!=settingsMap.end() ) {
            t_kvMap map = smItr->second;
            std::cout << "\nFinal kvMap:\n";
            for( t_kvMap::iterator itr = map.begin(); itr!=map.end(); itr++ ) {
                std::cout << itr->first << ", " << itr->second << "\n";
            }
        } else {
            std::cout << "\ncouldn't find final kvMap\n";
        }
    }

    return true;
}

std::string Settings::get(int kvMapIndex, std::string key)
{
    t_settingsMap::iterator itr = settingsMap.find(kvMapIndex);
    if( itr!=settingsMap.end() ) {
        return get(itr->second, key);
    }

    Log::warn(__PRETTY_FUNCTION__, "Tried to find a key-value pair in a nonexistent file index", "returning \"null\"");
    return "null";
}

Settings::t_kvMap Settings::getDefaultSettings(int kvMapIndex) { return Settings::defaultSettings[kvMapIndex]; }

Settings::t_kvMap Settings::getKvMap(int index)
{
    Settings::t_kvMap contents;

    t_settingsMap::iterator itr = settingsMap.find(index);
    if( itr!=settingsMap.end() ) {
        contents = itr->second;
    }

    return contents;
}

std::string Settings::getKey(t_kvMap kvMap, int index)
{
    int currIndex = 0;
    t_kvMap::iterator kvmItr = kvMap.begin();
    while( kvmItr!=kvMap.end() ) {
        if( currIndex==index ) {
            return kvmItr->first;
        }
        currIndex++;
        kvmItr++;
    }

    std::stringstream ss;
    ss << "Failed to find the key-value pair at " << index << " in the key-value set";
    Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning \"null\"");
    return "null";
}
/**
    Gets a value from a 'key' located in 'kvMap'.
*/
std::string Settings::get(t_kvMap kvMap, std::string key)
{
    t_kvMap::iterator kvmItr = kvMap.find(key);
    if( kvmItr!=kvMap.end() ) {
        return kvmItr->second;
    }

    std::stringstream ss;
    ss << "Failed to find the key \"" << key << "\" in the key-value set";
    Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning \"null\"");
    return "null";
}

int Settings::find(t_kvMap kvMap, std::string key)
{
    unsigned int index = 0;
    t_kvMap::iterator kvmItr = kvMap.begin();
    while( kvmItr->first!=key && kvmItr!=kvMap.end() ) {
        index++;
        kvmItr++;
    }

    if( index>=kvMap.size() ) {
        std::stringstream ss;
        ss << "Failed to find the key \"" << key << "\" in the key-value set";
        Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning -1");
        return -1;
    }
    return (int)index;
}

/**
    Adds a key-value pair ('key', 'val') to the provided 'kvMap'.
    If a key is added which already exists, the old value of the key is overwritten with the new value.
*/
void Settings::kv(t_kvMap* kvMap, std::string key, std::string val)
{
    t_kvMap::iterator kvmItr = kvMap->find(key);
    if( kvmItr!=kvMap->end() ) {
        kvmItr->second = val;
    } else {
        kvMap->insert( std::make_pair(key, val) );
    }
}

/**
    Converts 'val' to an std::string and then calls kv(t_kvMap*, std::string, std::string).
*/
void Settings::kv(t_kvMap* kvMap, std::string key, int val)
{
    std::stringstream ss; ss << val;
    kv(kvMap, key, ss.str());
}

void Settings::kv(int index, std::string key, std::string val)
{
    auto itrSM = settingsMap.find(index);
    if( itrSM!=settingsMap.end() ) {
        Settings::t_kvMap* kvMap;
        kvMap = &(itrSM->second);

        auto itrKVM = kvMap->find(key);
        if( itrKVM!=kvMap->end() ) {
            itrKVM->second = val;
        } else {
            kvMap->insert( std::make_pair(key, val) );
        }
    } else {
        std::stringstream ss;
        ss << "Settings map with index '" << index << "' doesn't exist";
        Log::error(__PRETTY_FUNCTION__, ss.str(), "doing nothing");
    }
}
void Settings::kv(int index, std::string key, int val)
{
    std::stringstream ss; ss << val;
    kv(index, key, ss.str());
}


void Settings::loadMap(int index, t_kvMap kvMap)
{
    //Find or create a new settingSet (t_kvMap) within 'settingsMap'.

    //If the index within 'settingsMap' exists, 'settingSet' will be a pointer to 'settingsMap'->second.
    //If the index doesn't exist, 'settingSet' will remain == nullptr.
    t_settingsMap::iterator smItr = settingsMap.find(index);
    if( smItr!=settingsMap.end() ) {
        //Assign settingSet to an existing settingSet
        t_kvMap* settingSet = &smItr->second;

        //Add all elements of 'kvMap' into the 'settingSet'
        t_kvMap::iterator kvmItr = kvMap.begin();
        while( kvmItr!=kvMap.end() ) {
            kv( settingSet, kvmItr->first, kvmItr->second );
            kvmItr++;
        }
    } else {
        //Create a new settingSet
        t_kvMap settingSet;

        //Add all elements of 'kvMap' into the 'settingSet'
        t_kvMap::iterator kvmItr = kvMap.begin();
        while( kvmItr!=kvMap.end() ) {
            kv( &settingSet, kvmItr->first, kvmItr->second );
            kvmItr++;
        }

        //Insert the 'settingSet'
        settingsMap.insert( std::make_pair(index, settingSet) );
    }
}
