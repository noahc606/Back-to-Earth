#pragma once
#include "GUIHandler.h"

class GUIBuilder
{
public:
	GUIBuilder();
	virtual ~GUIBuilder();

	void buildTitleScreen(GUIHandler& gh);
	void buildMainOptions(GUIHandler& gh);
	void buildMainControls(GUIHandler& gh, FileHandler& fh);
	void buildMainGraphics(GUIHandler& gh, FileHandler& fh);

	void buildWorldPause(GUIHandler& gh);
	void buildCharacterMenu(GUIHandler& gh);

protected:

private:
	std::string getCtrl(std::string);
	Settings* settings = nullptr;
};
