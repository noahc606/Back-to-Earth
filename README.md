# Project Background
Back to Earth is a game/game engine written in C++. It does not use OpenGL or another API/game engine. The libraries I used are in the 'Credits' section.

# Running
Go into the /bin folder and run the appropriate executable file. You should see two binary executable files, one with a .exe extension (for Windows) and one without (for Linux). MAKE SURE that the executable itself is in the same folder as the 'backtoearth' folder (which holds all resources for the program!). Basically just keep it in the root /bin folder it's already in and you'll be fine.

# Building
Right now building only works on Linux. I recently added two new libraries (cURL and SDL_net). I still need to update the cross-platform Makefile.

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