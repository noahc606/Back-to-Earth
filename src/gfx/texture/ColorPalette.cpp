#include "ColorPalette.h"

const std::vector<Color> ColorPalette::skinColors = {
    Color(040,  20,  10),   //Black
    Color(070,  40,  15),   //Dark brown
    Color(120,  75,  40),   //Brown
    Color(165, 115,  80),   //Dark tan
    Color(200, 150, 120),   //Tan
    Color(220, 180, 145),   //Light tan
    Color(235, 220, 140),   //Yellow tan
    Color(235, 200, 180),   //Fair (default)
    Color(250, 220, 160),   //Light yellow
    Color(250, 220, 180),   //Fair yellow
    Color(250, 220, 210),   //Fair pink
    Color(250, 230, 160),   //Yellow
    Color(255, 195, 170),   //Pink
    Color(255, 230, 200),   //Light pink
    Color(255, 250, 230),   //White
};
const std::vector<Color> ColorPalette::hairColors = {
    Color( 10,  10,  05),   //Midnight black
    Color( 50,  35,  50),   //Off black
    Color( 60,  50,  40),   //Darkest brown
    Color( 80,  85,  65),   //Medium dark drown

    Color( 90,  70,  70),   //Chestnut brown
    Color(105,  80,  65),   //Light chestnut brown
    Color( 95,  70,  55),   //Dark golden brown
    Color(165, 135, 105),   //Light golden brown

    Color(185, 150, 120),   //Dark honey blonde
    Color(220, 210, 185),   //Bleached blonde
    Color(220, 190, 150),   //Light ash blonde
    Color(150, 120,  95),   //Light ash brown

    Color(230, 205, 165),   //Lightest blonde
    Color(230, 200, 170),   //Pale Golden blonde
    Color(165, 185,  70),   //Strawberry blonde
    Color(145,  85,  60),   //Light auburn

    Color( 80,  60,  55),   //Dark auburn
    Color(115, 100,  95),   //Darkest gray
    Color(185, 165, 160),   //Medium gray
    Color(215, 195, 195),   //Light gray

    Color(255,  25, 225),   //White blonde
    Color(200, 190, 175),   //Platinum blonde
    Color(140,  75,  70),   //Russet red
    Color(180,  80,  60),   //Terra cotta
};

void ColorPalette::init(int id)
{
    switch( id )
    {
        case DEFAULT_PLAYER: {
            palette.insert( std::make_pair( "hair", hairColors[12] ) );
            palette.insert( std::make_pair( "skin", skinColors[7] ) );
            palette.insert( std::make_pair( "eyes", Color(130, 183, 211) ) );
            palette.insert( std::make_pair( "mouth", Color(234, 171, 169) ) );
            palette.insert( std::make_pair( "shirt", Color(50, 185, 50) ) );
            palette.insert( std::make_pair( "leggings", Color(127, 127, 180) ) );
            palette.insert( std::make_pair( "shoes", Color(78, 20, 21) ) );
        } break;
        case RANDOM_PLAYER: {
            palette.insert( std::make_pair( "hair", randHairColor() ) );
            palette.insert( std::make_pair( "skin", randSkinColor() ) );
            palette.insert( std::make_pair( "eyes", randColor() ) );
            palette.insert( std::make_pair( "mouth", Color(234, 171, 169) ) );
            palette.insert( std::make_pair( "shirt", randColor() ) );
            palette.insert( std::make_pair( "leggings", randColor() ) );
            palette.insert( std::make_pair( "shoes", randColor() ) );
        } break;
    }
}

Color ColorPalette::randColor()
{
    return Color(rand(), rand(), rand(), 255);
}

Color ColorPalette::randSkinColor()
{
    int index = rand()%skinColors.size();
    return skinColors[index];
}

Color ColorPalette::randHairColor()
{
    int index = rand()%hairColors.size();
    return hairColors[index];
}

Color ColorPalette::randEyeColor()
{
    Color c;
    return c;
}

Color ColorPalette::get(std::string key)
{
    t_palette::iterator pitr = palette.find(key);
    if( pitr!=palette.end() ) {
        return pitr->second;
    } else {
        return Color();
    }
}
