#include "ColorPalette.h"

void ColorPalette::init(int id)
{
    switch( id )
    {
        case DEFAULT_PLAYER: {
            palette.insert( std::make_pair( "hair", Color(193, 178, 127) ) );
            palette.insert( std::make_pair( "skin", Color(236, 200, 179) ) );
            palette.insert( std::make_pair( "eyes", Color(130, 183, 211) ) );
            palette.insert( std::make_pair( "mouth", Color(234, 171, 169) ) );
            palette.insert( std::make_pair( "shirt", Color(255, 0, 0) ) );
            palette.insert( std::make_pair( "leggings", Color(127, 127, 127) ) );
            palette.insert( std::make_pair( "shoes", Color(78, 20, 21) ) );
        } break;
    }
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
