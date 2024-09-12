https://github.com/user-attachments/assets/b01da03d-b784-4b52-8636-419d5d1fed2c

# Project Background
Back to Earth is a game & game engine written in C++ using "basic" cross-platform libraries. It does not use OpenGL or another API/engine. The libraries I used are in the 'Credits' section.

# Running
Go into the /bin folder and run the appropriate executable file. You should see two binary executable files, one with a .exe extension (for Windows) and one without (for Linux). The unix executable was built on Debian 12 (x64), so it's likely you need to build the program from source if you are using other hardware or another OS. MAKE SURE that the executable itself is in the same folder as the 'backtoearth' folder (which holds all resources for the program!). Basically just keep it in the root /bin folder it's already in and you'll be fine.

# Building
Building is cross-platform and uses CMake.

### Include 3rd Party Libraries
If you are on Linux/MacOS, you need to make sure you have all the necessary dev libraries for libsdl and libcurl installed. If you are on Windows, the linking step (with the -l) in the CMakeLists.txt can be confusing to get working properly. I recommend using MSYS2 and MinGW for building on Windows. See the CMakeLists.txt for more info on this.

### Include NCH-CPP-Utils
BTE 1.2-alpha now depends on <a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a>. Clone that repo and go to the root's 'include/nch' directory. There, copy or symlink the two directories 'cpp-utils' and 'sdl-utils' into a new 'include/nch' directory within BTE's root.

### Building & Running
Once you have the 'src', 'include', and 'bin' folders all in the same place (BTE's root), run the following:
```
mkdir build
cmake -Bbuild -H.
cmake --build build
```

For the application to run, it must also have its assets available (everything within [BTE root]/bin/backtoearth). Copy that from the main branch into the 'bin' folder if it's not there already.

# Credits
Libraries used:<br>
-The <a href="https://www.libsdl.org/">SDL2</a> libraries (SDL2, SDL2_image, SDL2_mixer, SDL2_ttf, SDL2_net).<br>
-<a href="https://curl.se/">cURL</a>, which is a network data transfer library.<br>
-<a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a>, my own helper library
<br>
Miscellaneous:<br>
-Any music/SFX within 'bin/backtoearth/resources/audio' (credits to those authors can be found in that diretory in credits.html).<br>
<br>
Things I developed:<br>
-Everything within the src folder (unless specified otherwise, a few occassions). You may use any of my source code without crediting me.<br>
-<a href="https://github.com/noahc606/nch-cpp-utils">NCH-CPP-Utils</a>