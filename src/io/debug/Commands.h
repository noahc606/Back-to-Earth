#pragma once
#include <map>
#include <string>
#include <vector>

class Commands
{
public:
    typedef std::map<std::string, int>          t_kvIntSet;
    typedef std::map<std::string, std::string>  t_kvStrSet;
    /**/
    Commands();
    virtual ~Commands();
    /**/
    /* Command line access */
    static int executeCMD(std::string cmd);
    static int cKV(std::string arg1, int arg2);
    static int cKV(std::string arg1, std::string arg2);
    /* Get command line state */
    static std::string cycleCMDs(int x);
    static int resetCMDEntered(std::string methodName);
    /**/
    /* Get CMD entered or CMD result */
    static bool cmdEntered();
    static int* cmdIntResult(std::string cmd);
    static std::string* cmdStringResult(std::string cmd);
    /* Get a value from a key (int, string) */
    static int* getInt(int key);
    static int* getInt(std::string key);
    static std::string* getString(int key);
    static std::string* getString(std::string key);
    /* Misc. getters */
    static int getCmdID(std::string cmd);
    /**/
    /**/
protected:

private:
    /**/
    static int cmdKV(std::string arg1, std::string arg2);
    static int cmdKV(std::string arg1, int arg2);
    static int cmdKV(std::string arg1);
    /**/
    static int parseString(std::string& word);
    static std::string errorInvalidArg(std::string commandName, int argNumber, std::string validType);
    static std::string errorMissingArg(int args, std::string commandName);
    static std::string errorTooManyArgs(int args, std::string commandName);
    static std::string getArgID(int arg);
    /**/
    static void surpressMessages(bool sm);
    static void ssWords(const std::vector<std::string>& words);

    static int userHistoryPos;
    static std::vector<std::string> userHistory;
    static std::vector<std::string> commandList;

    enum CommandIDs {
        CMD_kv,
        CMD_ksv,
        CMD_debug,
        CMD_gs,
        CMD_tele,
        CMD_boom,
    };

    enum ParseTypes {
        UNKNOWN = -1,
        INT = 1000,
        STRING,
        BOOL,
        NULL_,
    };

    static t_kvIntSet kvIntSet;
    static t_kvStrSet kvStrSet;

    static bool surpressingMessages;


};
