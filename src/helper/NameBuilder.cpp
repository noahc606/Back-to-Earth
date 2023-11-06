#include "NameBuilder.h"
#include <stdlib.h>

std::string NameBuilder::getRandomPlanetName(int syllables)
{
    return "asdf";
}

std::string NameBuilder::getRandomPlanetName()
{
    return getRandomPlanetName( 1+std::rand()%4 );
}
