#include "Loggable.h"
#include <algorithm>
#include <vector>
#include "Log.h"

void Loggable::putInfo(std::stringstream& ss, int& tabs)
{
	ss << "Object has undefined log output behavior.";
}

std::string Loggable::getInfo()
{
	std::stringstream ss;
	int tabs = 0;
	putInfo(ss, tabs);
	
	return ss.str();
}

void Loggable::logInfo()
{
	//Split string returned by getInfo() by "\n". Store in 'lines' vector
	std::string s = getInfo();
	std::vector<std::string> lines;
	size_t pos = 0;
	std::string token;
	while ( (pos = s.find("\n"))!=std::string::npos ) {
		token = s.substr(0, pos);
		lines.push_back(token);
		s.erase(0, pos+2);
	}
	lines.push_back(token);
	
	//Log each individual line of the string.
	for(int i = 0; i<lines.size(); i++) {
		Log::log(lines[i]);
	}
}