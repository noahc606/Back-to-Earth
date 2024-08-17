#pragma once
#include <map>
#include <nch/cpp-utils/gfx/Color.h>
#include <string>
#include <vector>
#include "Settings.h"

class ColorPalette
{
public:
	typedef std::map<std::string, NCH_Color> t_palette;
	
	void init(int id);
	void initPlayerPalette(Settings* s);
	NCH_Color get(std::string key);
	static NCH_Color randColor();
	static NCH_Color randSkinColor();
	static NCH_Color randHairColor();
	static NCH_Color randEyeColor();

	
	enum Palettes {
		DEFAULT_PLAYER,
		RANDOM_PLAYER,
	};
	
protected:

private:
	static NCH_Color skinColors[];
	//Taken from https://1.bp.blogspot.com/-c9JCgylzCvg/Ugg6aJs16KI/AAAAAAAARCM/aWIRNZ58eG4/s1600/rbg+for+hair.JPG
	static NCH_Color hairColors[];
	static NCH_Color eyeColors[];
	
	t_palette palette;
};
