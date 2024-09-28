#pragma once
#include <map>
#include <nch/cpp-utils/color.h>
#include <string>
#include <vector>
#include "Settings.h"

class ColorPalette
{
public:
	typedef std::map<std::string, nch::Color> t_palette;
	
	void init(int id);
	void initPlayerPalette(Settings* s);
	nch::Color get(std::string key);
	static nch::Color randColor();
	static nch::Color randSkinColor();
	static nch::Color randHairColor();
	static nch::Color randEyeColor();

	
	enum Palettes {
		DEFAULT_PLAYER,
		RANDOM_PLAYER,
	};
	
protected:

private:
	static nch::Color skinColors[];
	//Taken from https://1.bp.blogspot.com/-c9JCgylzCvg/Ugg6aJs16KI/AAAAAAAARCM/aWIRNZ58eG4/s1600/rbg+for+hair.JPG
	static nch::Color hairColors[];
	static nch::Color eyeColors[];
	
	t_palette palette;
};
