#include "ColorPalette.h"
#include <iostream>

NCH_Color ColorPalette::skinColors[] = {
	NCH_Color(040,  20,  10),   //Black
	NCH_Color(070,  40,  15),   //Dark brown
	NCH_Color(120,  75,  40),   //Brown
	NCH_Color(165, 115,  80),   //Dark tan
	NCH_Color(200, 150, 120),   //Tan
	NCH_Color(220, 180, 145),   //Light tan
	NCH_Color(235, 220, 140),   //Yellow tan
	NCH_Color(235, 200, 180),   //Fair (default)
	NCH_Color(250, 220, 160),   //Light yellow
	NCH_Color(250, 220, 180),   //Fair yellow
	NCH_Color(250, 220, 210),   //Fair pink
	NCH_Color(250, 230, 160),   //Yellow
	NCH_Color(255, 195, 170),   //Pink
	NCH_Color(255, 230, 200),   //Light pink
	NCH_Color(255, 250, 230),   //White
};
NCH_Color ColorPalette::hairColors[] = {
	NCH_Color( 10,  10,  05),   //Midnight black
	NCH_Color( 50,  35,  50),   //Off black
	NCH_Color( 60,  50,  40),   //Darkest brown
	NCH_Color( 80,  85,  65),   //Medium dark drown

	NCH_Color( 90,  70,  70),   //Chestnut brown
	NCH_Color(105,  80,  65),   //Light chestnut brown
	NCH_Color( 95,  70,  55),   //Dark golden brown
	NCH_Color(165, 135, 105),   //Light golden brown

	NCH_Color(185, 150, 120),   //Dark honey blonde
	NCH_Color(220, 210, 185),   //Bleached blonde
	NCH_Color(220, 190, 150),   //Light ash blonde
	NCH_Color(150, 120,  95),   //Light ash brown

	NCH_Color(230, 205, 165),   //Lightest blonde
	NCH_Color(230, 200, 170),   //Pale Golden blonde
	NCH_Color(165, 185,  70),   //Strawberry blonde
	NCH_Color(145,  85,  60),   //Light auburn

	NCH_Color( 80,  60,  55),   //Dark auburn
	NCH_Color(115, 100,  95),   //Darkest gray
	NCH_Color(185, 165, 160),   //Medium gray
	NCH_Color(215, 195, 195),   //Light gray

	NCH_Color(255,  25, 225),   //White blonde
	NCH_Color(200, 190, 175),   //Platinum blonde
	NCH_Color(140,  75,  70),   //Russet red
	NCH_Color(180,  80,  60),   //Terra cotta
};

void ColorPalette::init(int id)
{
	switch( id )
	{
	case DEFAULT_PLAYER: {
		palette.insert( std::make_pair( "hair", NCH_Color(230, 205, 165) ) );	//Hair color #13
		palette.insert( std::make_pair( "skin", NCH_Color(235, 200, 180) ) );	//Skin color #8
		palette.insert( std::make_pair( "eyes", NCH_Color(130, 183, 211) ) );
		palette.insert( std::make_pair( "mouth", NCH_Color(234, 171, 169) ) );
		palette.insert( std::make_pair( "shirt", NCH_Color(50, 185, 50) ) );
		palette.insert( std::make_pair( "leggings", NCH_Color(127, 127, 180) ) );
		palette.insert( std::make_pair( "shoes", NCH_Color(78, 20, 21) ) );
	} break;
	case RANDOM_PLAYER: {
		palette.insert( std::make_pair( "hair", randHairColor() ) );
		palette.insert( std::make_pair( "skin", randSkinColor() ) );
		palette.insert( std::make_pair( "eyes", randColor() ) );
		palette.insert( std::make_pair( "mouth", NCH_Color(234, 171, 169) ) );
		palette.insert( std::make_pair( "shirt", randColor() ) );
		palette.insert( std::make_pair( "leggings", randColor() ) );
		palette.insert( std::make_pair( "shoes", randColor() ) );
	} break;
	}
}

void ColorPalette::initPlayerPalette(Settings* s)
{
	if(s==nullptr) {
		return;	
	}

	int ci = s->character;
	auto kvm = s->getKvMap(ci);
	for(int i = 0; i<7; i++) {
		std::string key = s->getKey(kvm, i);
		std::string val = s->get(kvm, key);
		
		NCH_Color col(val);
		
		//If color failed to load
		if(col.getRGBA()==0) {
			col.a = 150;
		}
		
		palette.insert( std::make_pair(key, col) );
	}
}

NCH_Color ColorPalette::randColor()
{
    return NCH_Color(rand(), rand(), rand(), 255);
}

NCH_Color ColorPalette::randSkinColor()
{
    int index = rand()%( sizeof(skinColors)/sizeof(*skinColors) );
    return skinColors[index];
}

NCH_Color ColorPalette::randHairColor()
{
    int index = rand()%( sizeof(hairColors)/sizeof(*hairColors) );
    return hairColors[index];
}

NCH_Color ColorPalette::randEyeColor()
{
    NCH_Color c;
    return c;
}

NCH_Color ColorPalette::get(std::string key)
{
    t_palette::iterator pitr = palette.find(key);
    if( pitr!=palette.end() ) {
        return pitr->second;
    } else {
        return NCH_Color();
    }
}