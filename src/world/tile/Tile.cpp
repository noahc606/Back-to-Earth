#include "Tile.h"

Tile::Tile()
{
    init();
}

Tile::~Tile(){}

void Tile::init()
{
    for(int i = 0; i<6; i++)
        atlasTexLocations.push_back(std::make_pair(0, 0));
}