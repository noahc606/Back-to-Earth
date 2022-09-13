#include "Commands.h"
#include <vector>
#include "Log.h"

Commands::t_kvIntSet Commands::kvIntSet;
Commands::t_kvStrSet Commands::kvStrSet;
int Commands::cmdPos = 0;
std::vector<std::string> Commands::cmds;

bool Commands::surpressingMessages = false;

Commands::Commands(){}
Commands::~Commands(){}

/**/

int Commands::executeCMD(std::string cmd)
{
    if(cmd=="") return -1;

    //Add to 'cmds' vector
    cmds.push_back(cmd);
    cmdPos = (int)cmds.size();

    //Create a vector of "word" strings
    std::vector<std::string> words;

    //Split cmd string into smaller strings split by ' ', and put them into "words".
    //Make sure spaces within quotes  aren't counted, escape characters ('\\') allow for special characters, etc.
    bool withinQuotes = false;
    bool withinEsc = false;
    std::string currentWord = "";
    for( int i = 0; i<(int)cmd.size(); i++ ) {
        //Tracker whether inside of an escape character right now
        if( !withinEsc && cmd[i]=='\\' ) {
            withinEsc = true;
            continue;
        }

        //Track entering quoted text
        if( !withinQuotes && cmd[i]=='\"' && !withinEsc ) {
            withinQuotes = true;
        } else
        //Track if exiting quoted text
        if( withinQuotes && cmd[i]=='\"' && !withinEsc ) {
            withinQuotes = false;
        }

        //If currently on a space character (' ') AND not within quoted text
        if( cmd[i]==' ' && !withinQuotes ) {
            //If currentWord size is not 0
            if( (int)currentWord.size()!=0 ) {
                //Add 'currentWord' to 'words'
                words.push_back(currentWord);
                //Reset value of 'currentWord' for next word
                currentWord = "";
            }
        //If not currently on a space character
        } else {
            //Add the character to the current word.
            currentWord = currentWord + cmd[i];
        }

        //No longer within escape character (keep at the end)
        withinEsc = false;
    }
    words.push_back(currentWord);

    //
    std::string args[10] = { "" };
    std::string kvCMD = "kv";
    std::string ksvCMD = "ksv";
    std::string gsCMD = "gs";
    std::string teleCMD = "tele";

    int s = (int)words.size();
    for( int w = 0; w<s; w++ ) {

        std::string word = words.at(w);

        if( w==0 ) {
            if( word==kvCMD ||
                word==ksvCMD ||
                word==gsCMD ||
                word==teleCMD
            ) {
                args[0] = word;
                if(s<=1) {
                    Log::log(errorMissingArg(2, args[0]));
                    return -1;
                }
            }
        }

        if( w==1 ) {
            if( args[0]==kvCMD ) {
                //Set arg1 to word
                args[1] = word;
                //If there are only two arguments
                if(s==2) {
                    int x = cmdKV(word);
                    if( x!=0 ) {
                        Log::log(errorMissingArg(3, args[0]));
                        return -1;
                    }
                }
            }

            if( args[0]==ksvCMD ) {
                //Set arg1 to word
                args[1] = "\""+word+"\"";
                //If there are only two arguments
                if(s==2) {
                    int x = cmdKV("\""+word+"\"");
                    if( x!=0 ) {
                        Log::log(errorMissingArg(3, args[0]));
                        return -1;
                    }
                }
            }

            if( args[0]==gsCMD ) {
                args[1] = word;
                if( parseString(word)==ParseTypes::INT ) {
                    cKV("~cmd.gs", std::stoi(word));
                } else {
                    Log::log("Argument 2 must be an integer!");
                    return -1;
                }
            }

            if( args[0]==teleCMD ) {
                //Set arg1 to word
                args[1] = word;
                //If there are only two arguments
                if(s==2) {
                    Log::log(errorMissingArg(3, args[0]));
                    return -1;
                }
                //Parse string as an int
                if( parseString(word)==ParseTypes::INT ) {
                    cKV("~cmd.tele.x", std::stoi(word));
                } else {
                    Log::log("Argument 2 must be an integer!");
                    return -1;
                }
            }

        }

        if( w==2 ) {
            if(args[0]==kvCMD) {
                args[2] = word;
                cmdKV(args[1], args[2]);
            }

            if(args[0]==ksvCMD) {
                args[2] = word;
                cmdKV(args[1], args[2]);
            }

            if( args[0]==teleCMD ) {
                //Set arg2 to word
                args[2] = word;
                //Parse string as an int
                if( parseString(word)==ParseTypes::INT ) {
                    cKV("~cmd.tele.y", std::stoi(word));
                    cKV("~cmd.tele", 1);
                } else {
                    Log::log("Argument 3 must be an integer!");
                    return -1;
                }
            }

        }
    }

    if( args[0]=="" ) {
        std::stringstream ss;
        ss << "Invalid command \"" << words.at(0) << "\"";
        Log::log(ss.str());
        return -1;
    }

    //Track cmd state
    cKV("~cmd.state", 1);
    return 0;
}

int Commands::cKV(std::string arg1, int arg2) { return cmdKV("\""+arg1+"\"", arg2); }
int Commands::cKV(std::string arg1, std::string arg2) { return cmdKV("\""+arg1+"\"", arg2); }

std::string Commands::cycleCMDs(int x)
{
    if( cmds.size()==0 ) return "";

    //Cycle left/right x amount of times
    cmdPos += x;

    //'cmds' bounds
    if( cmdPos<0 ) cmdPos = 0;
    if( cmdPos>=(int)cmds.size() ) cmdPos = (int)cmds.size()-1;

    //Return the string in 'cmds' at cycle position
    return cmds[cmdPos];
}

int Commands::resetCMDEntered(std::string methodName)
{
    if( getInt("~cmd.state")==nullptr &&
        getString("~cmd.state")==nullptr
    ) {
        Log::warn(methodName, "Tried to reset key \"~cmd.state\" which doesn't exist");
        return -1;
    }

    surpressMessages(true);
    cKV("~cmd.gs", "null");
    cKV("~cmd.tele", "null");
    cKV("~cmd.state", "null");
    surpressMessages(false);
    return 0;
}

bool Commands::cmdEntered()
{
    if( getInt("~cmd.state")!=nullptr ) {
        return true;
    }
    return false;
}

int* Commands::cmdIntResult(std::string cmd)
{
    std::string cmdKey = "~cmd."+cmd;

    if( cmdEntered() && getInt(cmdKey)!=nullptr ) {
        return getInt(cmdKey);
    }

    return nullptr;
}

std::string* Commands::cmdStringResult(std::string cmd)
{
    std::string cmdKey = "~cmd."+cmd;

    if( cmdEntered() && getString(cmdKey)!=nullptr ) {
        return getString(cmdKey);
    }

    return nullptr;
}

int* Commands::getInt(int key)
{
    std::stringstream ss; ss << key;
    return getInt(ss.str());
}

int* Commands::getInt(std::string key)
{
    t_kvIntSet::iterator itr = kvIntSet.find(key);
    if( itr!=kvIntSet.end() ) {
        return &(itr->second);
    } else {
        return nullptr;
    }
}

std::string* Commands::getString(int key)
{
    std::stringstream ss; ss << key;
    return getString( ss.str() );
}

std::string* Commands::getString(std::string key)
{
    t_kvStrSet::iterator itr = kvStrSet.find(key);
    if( itr!=kvStrSet.end() ) {
        return &(itr->second);
    } else {
        return nullptr;
    }
}

/**/

int Commands::cmdKV(std::string arg1, std::string arg2)
{
    bool noMessage = false;
    if(surpressingMessages) {
        noMessage = true;
    }

    int pt1 = parseString(arg1);
    int pt2 = parseString(arg2);

    if( pt1==ParseTypes::UNKNOWN )
        return -1;

    if(arg1=="~cmd.state") {
        noMessage = true;
    }

    /* Remove any values corresponding to key 'arg1'. */
    for( t_kvStrSet::iterator itrKVS = kvStrSet.find(arg1); itrKVS!=kvStrSet.end(); ) {
        kvStrSet.erase(itrKVS);
        break;
    }
    for( t_kvIntSet::iterator itrKVI = kvIntSet.find(arg1); itrKVI!=kvIntSet.end(); ) {
        kvIntSet.erase(itrKVI);
        break;
    }

    //Insert a new key value pair kv(arg1, arg2) if arg1 is a string...
    if( pt2==ParseTypes::STRING ) {
        //...& arg2 is a string.

        /* Insert value */
        kvStrSet.insert( std::make_pair(arg1, arg2) );
        /* Message */
        if( !noMessage ) {
            std::stringstream ss;
            ss << "Set kv-pair (\"" << arg1 << "\", \"" << arg2 << "\").";
            Log::log(ss.str());
        }
        //Return
        return 0;
    } else
    if( pt2==ParseTypes::INT ) {
        //...& arg2 is an int.

        /* Insert value */
        kvIntSet.insert( std::make_pair(arg1, std::stoi(arg2)) );
        /* Message */
        if( !noMessage ) {
            std::stringstream ss;
            ss << "Set kv-pair (\"" << arg1 << "\", " << arg2 << ").";
            Log::log(ss.str());
        }
        //Return
        return 0;
    } else
    if( pt2==ParseTypes::BOOL ) {
        //...& arg2 is a bool.

        /* Insert value */
        if( arg2=="true" )  kvIntSet.insert( std::make_pair(arg1, 1) );
        if( arg2=="false" ) kvIntSet.insert( std::make_pair(arg1, 0) );
        /* Message */
        if( !noMessage ) {
            std::stringstream ss;
            ss << "Set kv-pair (\"" << arg1 << "\", " << arg2 << ").";
            Log::log(ss.str());
        }
        //Return
        return 0;
    } else
    if( pt2==ParseTypes::NULL_ ) {
        //...& arg2 is null.

        /* Message */
        if( !noMessage ) {
            std::stringstream ss;
            ss << "Removed kv-pair (\"" << arg1 << "\", " << arg2 << ").";
            Log::log(ss.str());
        }
        //Return
        return 0;
    }

    //Unsuccessful parsing
    return -1;
}
int Commands::cmdKV(std::string arg1, int arg2)
{
    std::stringstream ss;
    ss << arg2;
    return cmdKV(arg1, ss.str());
}
int Commands::cmdKV(std::string arg1)
{
    if( arg1=="~" || arg1=="clear" ) {
        //Message 1
        std::stringstream ss1;
        ss1 << "Clearing all key-value pairs...";
        Log::log(ss1.str());
        //Operation
        int ints = 0;
        for( t_kvIntSet::iterator itrKVI = kvIntSet.begin(); itrKVI!=kvIntSet.end(); ) {
            kvIntSet.erase(itrKVI);
            itrKVI = kvIntSet.begin();
            ints++;
        }
        int strs = 0;
        for( t_kvStrSet::iterator itrKVS = kvStrSet.begin(); itrKVS!=kvStrSet.end(); ) {
            kvStrSet.erase(itrKVS);
            itrKVS = kvStrSet.begin();
            strs++;
        }
        //Message 2
        std::stringstream ss2;
        ss2 << "Count: " << ints << " keys w/ integer values & " << strs << " keys w/ string values.";
        Log::log(ss2.str());
    } else
    if( arg1=="?" || arg1=="??" || arg1=="show" ) {
        bool showHidden = false;
        if( arg1=="??" || arg1=="show" ) {
            showHidden = true;
        }

        //Message 1
        std::stringstream ss1;
        if( !showHidden ) {
            ss1 << "List of key-value pairs:";
        } else {
            ss1 << "List of ALL key-value pairs:";
        }
        Log::log(ss1.str());
        /* Operation */
        //Count ints
        int ints = 0;
        int hiddenInts = 0;
        for( t_kvIntSet::iterator itrKVI = kvIntSet.begin(); itrKVI!=kvIntSet.end(); itrKVI++ ) {
            std::string str = itrKVI->first;
            if( str.size()>=1 ) {
                if( str.substr(0, 1)=="~" ) {
                    hiddenInts++;
                    if( !showHidden ) continue;
                    ints--;
                }
            }
            std::cout << "\t(\""<< itrKVI->first << "\", " << itrKVI->second << ")\n";
            ints++;
        }
        //Count strs
        int strs = 0;
        int hiddenStrs = 0;
        for( t_kvStrSet::iterator itrKVS = kvStrSet.begin(); itrKVS!=kvStrSet.end(); itrKVS++ ) {
            std::string str = itrKVS->first;
            if( str.size()>=1 ) {
                if( str.substr(0, 1)=="~" ) {
                    hiddenStrs++;
                    if( !showHidden ) continue;
                    strs--;
                }
            }
            std::cout << "\t(\""<< itrKVS->first << "\", \"" << itrKVS->second << "\")\n";
            strs++;
        }
        //Message 2
        std::stringstream ss2;

        if( !showHidden ) {
            ss2 << "Found " << ints << " integers, " << strs << " strings.";
        } else {
            ss2 << "Found " << ints << " (+" << hiddenInts << ") integers, " << strs << " (+" << hiddenStrs << ") strings.";
        }
        Log::log(ss2.str());
    } else
    {
        //Unsuccessful command
        return -1;
    }
    //Successful command
    return 0;
}

int Commands::parseString(std::string& word)
{
    //Try to parse "word" as a string
    int s = (int)word.size();
    if( s>=2 ) {
        if( word[0]=='\"' && word[s-1]=='\"' ) {
            std::string temp = word.substr(1, s-2);
            word = temp;
            return ParseTypes::STRING;
        }
    }

    //Try to parse 2nd arg as an int
    bool parseIntSuccess = true;
    for( int i = 0; i<s; i++ ) {
        if( !( word[i]=='-' || (word[i]>='0' && word[i]<='9') ) ) {
            parseIntSuccess = false;
            break;
        }
    }
    if( parseIntSuccess==true ) return ParseTypes::INT;

    //Try to parse 2nd arg as a bool
    if( word=="true" || word=="false" ) {
        return ParseTypes::BOOL;
    }

    //Try to parse 2nd arg as null
    if( word=="null" ) {
        return ParseTypes::NULL_;
    }

    //Unable to parse string if program has reached this point
    std::stringstream ss;
    ss << "Unable to parse \"" << word << "\" - must be a string (surrounded in quotes) or an int";
    Log::log(ss.str());
    return ParseTypes::UNKNOWN;
}

std::string Commands::errorMissingArg(int arg, std::string commandName)
{
    std::stringstream ss;
    ss << "No " << getArgID(arg) << " for command \"" << commandName << "\".";
    return ss.str();
}

std::string Commands::getArgID(int arg)
{
    std::string suffix = "th";
    switch( arg ) {
        case 1: suffix = "st"; break;
        case 2: suffix = "nd"; break;
        case 3: suffix = "rd"; break;
    }

    std::stringstream ss;
    ss << arg << suffix << " argument";
    return ss.str();
}

void Commands::surpressMessages(bool sm) { surpressingMessages = sm; }

void Commands::ssWords(const std::vector<std::string>& words)
{
    int wordCount = 0;
    std::stringstream wordsFormatted;
    wordsFormatted << "{ ";
    for( std::string word : words ) {
        wordCount++;
        wordsFormatted << "\"" << word << "\" ";
    }
    wordsFormatted << "}";
    Log::log(wordsFormatted.str());
}
