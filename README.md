# Project Background:
Back to Earth is a game written in C++ using the SDL2 libraries (SDL, SDL_image, SDL_mixer, SDL_ttf). It does not use OpenGL or another API/game engine. The main purpose of this project is to show that many things are possible to do purely with the SDL2 libraries, without help from an extra API.

# Project Goal:
Right now the project is considered "mostly finished" since my original goal was to create a 2D game framework. It is not yet a full-fledged video game, but it is something I am working toward.

# Running:
Go into the /bin folder and run the appropriate executable file. You should see two binary executable files, one with a .exe extension (for Windows) and one without (for Linux). MAKE SURE that the executable itself is in the same folder as the 'backtoearth' folder (which holds all resources for the program!). Basically just keep it in the root /bin folder it's already in and you'll be fine.

# Building:
You can build the program yourself to get a new executable file. There are two ways to do this: with CMake (only on Linux right now) or Make (Windows and Linux). If you are on Linux I recommend using VSCode or another environment with integrated support for building from CMakeLists and using that.

For cross-platform building, you can use GNU Make, although it is slower than CMake. After installing GNU Make for Windows or Linux, it's as simple as running 'make' in the terminal while in the repo's root folder. This will try to rebuild the program from source and give you a working executable on any platform where C/C++ & SDL works (Windows, macOS, Linux, and theoretically iOS and Android). However, keep in mind that I have only tested this application on Windows and Linux.

# Credits:
Things I did not work on:
-The SDL library files (https://www.libsdl.org/) contained within the 'include' folder.
-cURL, which is a network data transfer library (https://curl.se/).
-Any music/SFX within 'bin/backtoearth/resources/audio' (credits to those authors can be found in that diretory in credits.html).
Things I developed:
-Everything within the src folder (unless specified otherwise, a few occassions). You may use any of my source code without crediting me.