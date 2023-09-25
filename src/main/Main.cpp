#include "Main.h"
#include <iostream>
#include <SDL2/SDL.h>
#include "MainLoop.h"

//Constants
const std::string Main::VERSION = "Back to Earth";
const bool Main::DEBUG = true;
const bool Main::TROUBLESHOOTING = false;

int main(int argc, char *argv[])
{
    //Run the MainLoop
    MainLoop ml;
	
    //Program ends
    return 0;
}

int WinMain()
{
    return main(0, new char*);
}