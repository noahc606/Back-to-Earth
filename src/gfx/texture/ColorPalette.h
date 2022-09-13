#pragma once
#include <map>
#include "Color.h"

class ColorPalette
{
public:
    typedef std::map<std::string, Color> t_palette;

    void init(int id);
    Color get(std::string key);

    enum Palettes {
        DEFAULT_PLAYER,
    };

protected:

private:
     t_palette palette;
};
