#include "Main.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <time.h>
#include "MainLoop.h"

//Constants
const std::string Main::VERSION_LABEL = "1.1.0a";
const std::string Main::VERSION_DATE = "17 June 2024";
const std::string Main::TITLE = "Back to Earth";
const std::string Main::PASSWORD = "opensesame";
const bool Main::DEBUGGING = true;
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
 * 	WinMain() is called for Windows GUI applications instead of main()
 */
#if ( defined(_WIN32) || defined(WIN32) )
int WinMain()
{
    char** x = new char*[1];
    return main(0, x);
}
#endif