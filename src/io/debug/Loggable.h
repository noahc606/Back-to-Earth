#pragma once
#include <sstream>

class Loggable {
public:
	/**/
	/**/
	virtual void putInfo(std::stringstream& ss, int& tabs);
	std::string getInfo();
	/**/
	/**/
	void logInfo();
};