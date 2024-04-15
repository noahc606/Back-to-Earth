# Project Background
Back to Earth is a game/game engine written in C++. It does not use OpenGL or another API/game engine. The libraries I used are in the 'Credits' section.

# Running
Go into the /bin folder and run the appropriate executable file. You should see two binary executable files, one with a .exe extension (for Windows) and one without (for Linux). The unix executable was built on Debian Linux 12, so you may need to build the program from source if you are using another Linux/macOS/etc. MAKE SURE that the executable itself is in the same folder as the 'backtoearth' folder (which holds all resources for the program!). Basically just keep it in the root /bin folder it's already in and you'll be fine.

# Building
Building is cross-platform and uses CMake. I recommend using VSCode but using the terminal or another environment should work. If you are on Linux/MacOS, you need to make sure you have all the necessary dev libraries for libsdl and libcurl installed. If you are on Windows, the linking step (with the -l) in the CMakeLists.txt can be confusing to get working properly. I recommend using MSYS2 and MinGW for building on Windows. See the CMakeLists.txt for more info on this.

# Credits
<br>
Libraries used:<br>
-SDL2 library (SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, SDL2_net, from https://www.libsdl.org/). This project statically links with the SDL library files contained within the 'lib' folder.<br>
-cURL, which is a network data transfer library (https://curl.se/).<br>
<br>
Miscellaneous:<br>
-Any music/SFX within 'bin/backtoearth/resources/audio' (credits to those authors can be found in that diretory in credits.html).<br>
<br>
Things I developed:<br>
-Everything within the src folder (unless specified otherwise, a few occassions). You may use any of my source code without crediting me.<br>