cmake_minimum_required(VERSION "3.0.0")

set(BTE_OUT_UNIX "Back-to-Earth-unix")
set(BTE_OUT_WINDOWS "Back-to-Earth-windows")

# Project
project(${BTE_OUT_UNIX} VERSION 0.1.0.0 LANGUAGES C CXX)
set(CMAKE_C_FLAGS "-std=c++11 -o")


# Get list of sources and headers
file (GLOB_RECURSE BTE_SOURCES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.cpp")
file (GLOB_RECURSE BTE_HEADERS CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/src/*.h")

# Build list of includes
set (BTE_SRC_DIRS "")
foreach (_headerFile ${BTE_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND BTE_SRC_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES BTE_SRC_DIRS)

# Add executable
add_executable(${BTE_OUT_UNIX} ${BTE_SOURCES})
set_target_properties(${BTE_OUT_UNIX} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)

# Linking
target_link_libraries(${BTE_OUT_UNIX} PRIVATE "-lcurl -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2_net")

# Add include directories (everywhere there is a header file: libraries and BTE src dirs)
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.10-i686-w64-mingw32/include")
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.10-i686-w64-mingw32/include/SDL2")    #Include both / and /SDL2
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2_image-2.0.5-i686-w64-mingw32/include")
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2_mixer-2.0.4-i686-w64-mingw32/include")
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2_ttf-2.0.15-i686-w64-mingw32/include")
target_include_directories(${BTE_OUT_UNIX} PRIVATE "${CMAKE_SOURCE_DIR}/lib/SDL2_net-2.2.0-i686-w64-mingw32/include")
target_include_directories(${BTE_OUT_UNIX} PRIVATE ${BTE_SRC_DIRS})
