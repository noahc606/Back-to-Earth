#include "Main.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>
#include "MainLoop.h"

//Constants
const std::string Main::VERSION = "Back to Earth 1.1.0a";
const bool Main::DEBUG = true;
const bool Main::TROUBLESHOOTING = false;

/*
 *	Create & run a MainLoop(). When the MainLoop is finished, end the program.
 */
int main(int argc, char *argv[])
{
	//Seed RNG with current time (# of seconds since 1970)
	srand(time(NULL));
	
	//Run the MainLoop through its constructor
	MainLoop ml;
	
	//Program ends
	return 0;
}

/*
 * 	WinMain() is called for Windows GUI applications instead of main() - this is needed for cross-platform compatibility.
 */
int WinMain()
{
	return main(0, new char*);
}