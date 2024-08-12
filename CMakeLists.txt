# If on Windows, recommended to:
# 1) Install + Open MSYS2
# 2) pacman -Syu
# 3) pacman -Sy mingw-w64-x86_64-curl
# 4) pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_net
# -lSDL2/-lSDL2_image/etc should now work.

cmake_minimum_required(VERSION 3.5)

if (WIN32)
    set(PROJ_OUT "Back-to-Earth-windows")
    set(CMAKE_WIN32_EXECUTABLE TRUE)
endif (WIN32)
if (UNIX)
    set(PROJ_OUT "Back-to-Earth-unix")
endif (UNIX)

# Project
project(${PROJ_OUT} VERSION 0.1.1.0 LANGUAGES C CXX)
#set(CMAKE_C_FLAGS "-std=c++17")
set(CMAKE_C_FLAGS "-std=c++17 -O3 -DNDEBUG -mwindows -s -flto")

# Get list of sources and headers
file (GLOB_RECURSE PROJ_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB_RECURSE PROJ_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h")

# Build list of includes
set (PROJ_SRC_DIRS "")
foreach (_headerFile ${PROJ_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND PROJ_SRC_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES PROJ_SRC_DIRS)

# Add executable
add_executable(${PROJ_OUT} ${PROJ_SOURCES})
set_target_properties(${PROJ_OUT} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Linking
target_link_libraries(${PROJ_OUT} PRIVATE "-lcurl -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_net")

# Add include directories (everywhere there is a header file: libraries and BTE src dirs)
target_include_directories(${PROJ_OUT} PRIVATE ${PROJ_SRC_DIRS})