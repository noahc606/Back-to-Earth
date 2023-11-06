# Project Background:
Back to Earth is a game written in C++ using the SDL2 libraries (SDL, SDL_image, SDL_mixer, SDL_ttf). It does not use OpenGL or another API/game engine. The main purpose of this project is to show that many things are possible to do purely with the SDL2 libraries, without help from an extra API.

# Project Goal:
Right now the project is considered "mostly finished" since my original goal was to create a 2D game framework. It is not yet a full-fledged video game, but it is something I am working toward.

# Running:
If you want to run the program: MAKE SURE that the executable itself is in the same folder as the 'backtoearth' folder (which holds all resources for the program!). Basically just keep it in the root /bin folder it's already in and you'll be fine. Also, you should see two binary executable files, one with a .exe extension (for Windows) and one without (for Linux).

# Building:
You can rebuild the program yourself to get a new executable file. The easiest way to do this is by using GNU make. After installing that for Windows or Linux, it's as simple as running 'make' in the terminal while in the repo's root folder. This will try to rebuild the program from source and give you a working executable on any platform where C/C++ & SDL works (Windows, macOS, Linux, and theoretically iOS and Android). However, keep in mind that I have only tested this application on Windows and Linux.

# Credits:
The SDL library header files (https://www.libsdl.org/) contained within the 'include' folder were not made by me.
Credits to creators of music + SFX are in 'backtoearth/resources/audio'.
You may use any of my source code (anything within the 'src' folder) without crediting me.