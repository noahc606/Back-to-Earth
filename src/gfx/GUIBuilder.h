#pragma once
#include "GUIHandler.h"

class GUIBuilder
{
public:
	GUIBuilder();
	virtual ~GUIBuilder();

	void buildUpdatePrompt(GUIHandler& gh, Window* titleWindow, std::string newVersion);
	void buildUpdatingScreen(GUIHandler& gh);

	void buildTitleScreen(GUIHandler& gh);
	void buildMainOptions(GUIHandler& gh);
	void buildMainControls(GUIHandler& gh, FileHandler& fh);
	void buildMainGraphics(GUIHandler& gh, FileHandler& fh);
	void buildMainCharacter(GUIHandler& gh, FileHandler& fh);
	void buildMainDebug(GUIHandler& gh, FileHandler& fh);

	void buildWorldPause(GUIHandler& gh);
	void buildCharacterMenu(GUIHandler& gh);
	void buildColorSelector(GUIHandler& gh, Window* parentWindow, int extraID);

	void buildSelectCampaign(GUIHandler& gh, FileHandler& fh);

protected:

private:
	int ch = GUIAlignable::CENTER_H;
	int cv = GUIAlignable::CENTER_V;
	
	std::string getCtrl(std::string);
	Settings* settings = nullptr;
};
