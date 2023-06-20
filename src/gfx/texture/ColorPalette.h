#pragma once
#include <map>
#include <string>
#include <vector>
#include "Color.h"

class ColorPalette
{
public:
    typedef std::map<std::string, Color> t_palette;

    void init(int id);
    Color get(std::string key);
    static Color randColor();
    static Color randSkinColor();
    static Color randHairColor();
    static Color randEyeColor();

    enum Palettes {
        DEFAULT_PLAYER,
        RANDOM_PLAYER,
    };

    static const std::vector<Color> skinColors;
    //Taken from https://1.bp.blogspot.com/-c9JCgylzCvg/Ugg6aJs16KI/AAAAAAAARCM/aWIRNZ58eG4/s1600/rbg+for+hair.JPG
    static const std::vector<Color> hairColors;
    static const std::vector<Color> eyeColors;

protected:

private:
     t_palette palette;
};
