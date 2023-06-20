#include "Main.h"
#include <SDL.h>
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
