#include "Tests.h"
#include "Color.h"
#include "Text.h"
#include "Window.h"
#include "Real.h"
/**/

Tests::Tests(){}
void Tests::init(SDLHandler* sh, FileHandler* fh, Controls* ctrls)
{
    BTEObject::init(sh, fh, ctrls);

    Real x = -5.7;
    Real y =  3.1;

    std::cout << "Test:\n";
    std::cout << (x+y).getString() << "\n";
    std::cout << "Test end\n";

}
Tests::~Tests(){}

/**/

void Tests::draw()
{
}

void Tests::tick()
{

}

/**/

void Tests::info(std::stringstream& ss, int& tabs)
{
    //DebugScreen::newGroup(ss, tabs, "Noise");
    //noise.info(ss, tabs);
    //DebugScreen::endGroup(tabs);
}
