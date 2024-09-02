#include "ColorPalette.h"
#include <iostream>

nch::Color ColorPalette::skinColors[] = {
	nch::Color(040,  20,  10),   //Black
	nch::Color(070,  40,  15),   //Dark brown
	nch::Color(120,  75,  40),   //Brown
	nch::Color(165, 115,  80),   //Dark tan
	nch::Color(200, 150, 120),   //Tan
	nch::Color(220, 180, 145),   //Light tan
	nch::Color(235, 220, 140),   //Yellow tan
	nch::Color(235, 200, 180),   //Fair (default)
	nch::Color(250, 220, 160),   //Light yellow
	nch::Color(250, 220, 180),   //Fair yellow
	nch::Color(250, 220, 210),   //Fair pink
	nch::Color(250, 230, 160),   //Yellow
	nch::Color(255, 195, 170),   //Pink
	nch::Color(255, 230, 200),   //Light pink
	nch::Color(255, 250, 230),   //White
};
nch::Color ColorPalette::hairColors[] = {
	nch::Color( 10,  10,  05),   //Midnight black
	nch::Color( 50,  35,  50),   //Off black
	nch::Color( 60,  50,  40),   //Darkest brown
	nch::Color( 80,  85,  65),   //Medium dark drown

	nch::Color( 90,  70,  70),   //Chestnut brown
	nch::Color(105,  80,  65),   //Light chestnut brown
	nch::Color( 95,  70,  55),   //Dark golden brown
	nch::Color(165, 135, 105),   //Light golden brown

	nch::Color(185, 150, 120),   //Dark honey blonde
	nch::Color(220, 210, 185),   //Bleached blonde
	nch::Color(220, 190, 150),   //Light ash blonde
	nch::Color(150, 120,  95),   //Light ash brown

	nch::Color(230, 205, 165),   //Lightest blonde
	nch::Color(230, 200, 170),   //Pale Golden blonde
	nch::Color(165, 185,  70),   //Strawberry blonde
	nch::Color(145,  85,  60),   //Light auburn

	nch::Color( 80,  60,  55),   //Dark auburn
	nch::Color(115, 100,  95),   //Darkest gray
	nch::Color(185, 165, 160),   //Medium gray
	nch::Color(215, 195, 195),   //Light gray

	nch::Color(255,  25, 225),   //White blonde
	nch::Color(200, 190, 175),   //Platinum blonde
	nch::Color(140,  75,  70),   //Russet red
	nch::Color(180,  80,  60),   //Terra cotta
};

void ColorPalette::init(int id)
{
	switch( id )
	{
	case DEFAULT_PLAYER: {
		palette.insert( std::make_pair( "hair", nch::Color(230, 205, 165) ) );	//Hair color #13
		palette.insert( std::make_pair( "skin", nch::Color(235, 200, 180) ) );	//Skin color #8
		palette.insert( std::make_pair( "eyes", nch::Color(130, 183, 211) ) );
		palette.insert( std::make_pair( "mouth", nch::Color(234, 171, 169) ) );
		palette.insert( std::make_pair( "shirt", nch::Color(50, 185, 50) ) );
		palette.insert( std::make_pair( "leggings", nch::Color(127, 127, 180) ) );
		palette.insert( std::make_pair( "shoes", nch::Color(78, 20, 21) ) );
	} break;
	case RANDOM_PLAYER: {
		palette.insert( std::make_pair( "hair", randHairColor() ) );
		palette.insert( std::make_pair( "skin", randSkinColor() ) );
		palette.insert( std::make_pair( "eyes", randColor() ) );
		palette.insert( std::make_pair( "mouth", nch::Color(234, 171, 169) ) );
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
		
		nch::Color col(val);
		
		//If color failed to load
		if(col.getRGBA()==0) {
			col.a = 150;
		}
		
		palette.insert( std::make_pair(key, col) );
	}
}

nch::Color ColorPalette::randColor()
{
    return nch::Color(rand(), rand(), rand(), 255);
}

nch::Color ColorPalette::randSkinColor()
{
    int index = rand()%( sizeof(skinColors)/sizeof(*skinColors) );
    return skinColors[index];
}

nch::Color ColorPalette::randHairColor()
{
    int index = rand()%( sizeof(hairColors)/sizeof(*hairColors) );
    return hairColors[index];
}

nch::Color ColorPalette::randEyeColor()
{
    nch::Color c;
    return c;
}

nch::Color ColorPalette::get(std::string key)
{
    t_palette::iterator pitr = palette.find(key);
    if( pitr!=palette.end() ) {
        return pitr->second;
    } else {
        return nch::Color();
    }
}