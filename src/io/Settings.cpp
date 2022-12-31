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

    addTo(s, "FUNC_8", SDLK_F8);
    addTo(s, "FUNC_9", SDLK_F9);
    addTo(s, "FUNC_SCREENSHOT", SDLK_F10);
    addTo(s, "FUNC_FULLSCREEN", SDLK_F11);
    addTo(s, "FUNC_DEBUG", SDLK_F12);
    addTo(s, "INGAME_PAUSE", SDLK_ESCAPE);
    addTo(s, "MAP_USE_ZOOM", SDLK_LCTRL);

    /** Player */
    //Movement
    addTo(s, "PLAYER_MOVE_NORTH", SDLK_w);
    addTo(s, "PLAYER_MOVE_SOUTH", SDLK_s);
    addTo(s, "PLAYER_MOVE_WEST", SDLK_a);
    addTo(s, "PLAYER_MOVE_EAST", SDLK_d);
    addTo(s, "PLAYER_MOVE_UP", SDLK_UP);
    addTo(s, "PLAYER_MOVE_DOWN", SDLK_DOWN);
    addTo(s, "PLAYER_MOVE_VERTICAL", SDLK_SPACE);

    //Advanced Movement
    addTo(s, "PLAYER_JUMP", SDLK_SPACE);
    addTo(s, "PLAYER_SPRINT", SDLK_x);
    addTo(s, "PLAYER_CROUCH", SDLK_c);

    //Player Interactions
    addTo(s, "PLAYER_INVENTORY", SDLK_LSHIFT);
    addTo(s, "PLAYER_SELECT", "MOUSE_LEFT");
    addTo(s, "PLAYER_VIEW", SDLK_v);


    addTo(s, "HARDCODE_LEFT_CLICK", "MOUSE_LEFT");
    addTo(s, "HARDCODE_MIDDLE_CLICK", "MOUSE_MIDDLE");
    addTo(s, "HARDCODE_RIGHT_CLICK", "MOUSE_RIGHT");

    /** Number keys */
    addTo(s, "DEBUG_1", SDLK_1);
    addTo(s, "DEBUG_2", SDLK_2);
    addTo(s, "DEBUG_3", SDLK_3);
    addTo(s, "DEBUG_4", SDLK_4);
    addTo(s, "DEBUG_5", SDLK_5);
    addTo(s, "DEBUG_6", SDLK_6);
    addTo(s, "DEBUG_7", SDLK_7);
    addTo(s, "DEBUG_8", SDLK_8);
    addTo(s, "DEBUG_9", SDLK_9);
    addTo(s, "DEBUG_0", SDLK_0);
    addTo(s, "TEST1234", SDLK_BACKSPACE);

    s = &(defaultSettings[games]);
    addTo(s, "needTutorial", "true");

    s = &(defaultSettings[options]);
    addTo(s, "fullscreen", "false");
    addTo(s, "maxFPS", "none");
    addTo(s, "bteCursor", "true");

    s = &(defaultSettings[version]);
    addTo(s, "version", Main::VERSION);

    s = &(defaultSettings[session]);
    addTo(s, "date", MainLoop::getSystemTime() );
}

bool Settings::unload(int index)
{
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

bool Settings::save( std::ofstream* ofs, int index)
{
    bool success = false;
    t_settingsMap::iterator itr = settingsMap.find(index);
    if( itr!=settingsMap.end() ) {
        t_kvMap kvMap = itr->second;
        for( t_kvPair kvp : kvMap) {
            (*ofs) << kvp.first << "=" << kvp.second << "\n";
        }
        success = true;
    }

    ofs->close();
    return success;
}

std::string Settings::get(int index, std::string key)
{
    t_settingsMap::iterator itr = settingsMap.find(index);
    if( itr!=settingsMap.end() ) {
        return get(itr->second, key);
    }

    Log::warn(__PRETTY_FUNCTION__, "Tried to find a key-value pair in a nonexistent file index", "returning \"null\"");
    return "null";
}

Settings::t_kvMap Settings::getKvMap(int index)
{
    Settings::t_kvMap contents;

    t_settingsMap::iterator itr = settingsMap.find(index);
    if( itr!=settingsMap.end() ) {
        contents = itr->second;
    }

    return contents;
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
            addTo( settingSet, kvmItr->first, kvmItr->second );
            kvmItr++;
        }
    } else {
        //Create a new settingSet
        t_kvMap settingSet;

        //Add all elements of 'kvMap' into the 'settingSet'
        t_kvMap::iterator kvmItr = kvMap.begin();
        while( kvmItr!=kvMap.end() ) {
            addTo( &settingSet, kvmItr->first, kvmItr->second );
            kvmItr++;
        }

        //Insert the 'settingSet'
        settingsMap.insert( std::make_pair(index, settingSet) );
    }
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
    ss << "Failed to find a value for key \"" << key << "\" in the key-value set";
    Log::warn(__PRETTY_FUNCTION__, ss.str(), "returning \"null\"");
    return "null";
}

/**
    Adds a key-value pair ('key', 'val') to the provided 'kvMap'.
    If a key is added which already exists, the old value of the key is overwritten with the new value.
*/
void Settings::addTo(t_kvMap* kvMap, std::string key, std::string val)
{
    t_kvMap::iterator kvmItr = kvMap->find(key);
    if( kvmItr!=kvMap->end() ) {
        kvmItr->second = val;
    } else {
        kvMap->insert( std::make_pair(key, val) );
    }
}

/**
    Converts 'val' to an std::string and then calls addTo(t_kvMap*, std::string, std::string).
*/
void Settings::addTo(t_kvMap* kvMap, std::string key, int val)
{
    std::stringstream ss; ss << val;
    addTo(kvMap, key, ss.str());
}
