# [ Makefile variables ] #######################################################################################################
## Directories
DIR_ROOT := $(dir $(realpath $(lastword $(MAKEFILE_LIST))))
DIR_SRCS := $(wildcard src/ src/*/ src/*/*/ src/*/*/*/ src/*/*/*/*/)
DIR_BUILD := bin
DIR_INCLUDE := include
#SDL 2 base for mingw32
DIR_LIBSDL2_SDL := $(DIR_ROOT)$(DIR_INCLUDE)/SDL2-2.0.10-i686-w64-mingw32
#SDL 2 image for mingw32
DIR_LIBSDL2_IMG := $(DIR_ROOT)$(DIR_INCLUDE)/SDL2_image-2.0.5-i686-w64-mingw32
#SDL 2 mixer for mingw32
DIR_LIBSDL2_MIX := $(DIR_ROOT)$(DIR_INCLUDE)/SDL2_mixer-2.0.4-i686-w64-mingw32
#SDL 2 ttf for mingw32
DIR_LIBSDL2_TTF := $(DIR_ROOT)$(DIR_INCLUDE)/SDL2_ttf-2.0.15-i686-w64-mingw32

## Files
#Find any c/c++ source files that should be compiled into .o files; .d files hold dependencies.
SRCS_WIN := $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
OBJS_WIN := $(patsubst src/%.cpp,$(DIR_BUILD)/src/%.cpp.o,$(SRCS_WIN))
DEPS_WIN := $(patsubst %.o,%.d,$(OBJS_WIN))

## Flags
FLAGS_INC := -I$(DIR_LIBSDL2_SDL)/include/SDL2 -I$(DIR_LIBSDL2_SDL)/include -I$(DIR_LIBSDL2_IMG)/include -I$(DIR_LIBSDL2_MIX)/include -I$(DIR_LIBSDL2_TTF)/include
FLAGS_LIB := -L$(DIR_LIBSDL2_SDL)/lib -L$(DIR_LIBSDL2_IMG)/lib -L$(DIR_LIBSDL2_MIX)/lib -L$(DIR_LIBSDL2_TTF)/lib

# -o at the end???? may cause problems
FLAGS_CPP := $(addprefix -I,$(DIR_SRCS)) -MMD -MP -std=c++11 -o

FLAGS_LD = -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf

## Miscellaneous
#Executable name
TARGET_EXEC := Back-to-Earth
#Compiler to use (g++)
CXX := g++

# [ Build interface ] ##########################################################################################################
build-interface:
	@echo "Only if you are on Windows (where executables have a .exe extension):"
	@echo "    1. use 'make windows' in the root directory of the repo."
	@echo "Only if you are on Unix (where executables have no extension):"
	@echo "    1. Install sdl2, sdl2-img, sdl2-mixer, and sdl2-ttf using your package manager."
	@echo "       (ex: 'sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev' for Debian/Ubuntu)"
	@echo "    2. use 'make unix' in the root directory of the repo."
	@echo "    "
	@echo "Some stuff about platforms:"
	@echo "    - Linux, macOS, iOS, and Android are all Unix. However, as of yet I have only tested building on Linux and Windows."
	@echo "    - If you are on another platform, you will have to figure out how to build the src yourself (if SDL2 even works)."
	@echo "    - The application was designed to run in a desktop environment - I have not tested what would happen if you ran the executable on a mobile device."

windows:
	@echo "Building Back to Earth for Windows..."
	@make build-dirs-win
	@make $(DIR_BUILD)/$(TARGET_EXEC)-windows.exe

unix:
	@echo "Building Back to Earth for Unix..."
	@make build-dirs-unix
	@make $(DIR_BUILD)/$(TARGET_EXEC)-unix

# [ Build targets ] ############################################################################################################
build-dirs-win:
	make $(subst /,\\,$(addprefix $(DIR_BUILD)/,$(DIR_SRCS)))

build-dirs-unix:
	mkdir -p $(addprefix $(DIR_BUILD)/,$(DIR_SRCS))

# Build C++ executable (for Windows)
$(DIR_BUILD)/Back-to-Earth-windows.exe: $(OBJS_WIN)
	$(CXX) $(OBJS_WIN) $(FLAGS_INC) $(FLAGS_LIB) -w -Wl,-subsystem,windows -mconsole $(FLAGS_LD) -o $@

# Build C++ executable (for unix)
$(DIR_BUILD)/Back-to-Earth-unix: $(OBJS_WIN)
	$(CXX) $(OBJS_WIN) -o $@ $(FLAGS_LD)

# Build C++ source files
$(DIR_BUILD)/%.cpp.o: %.cpp
	$(CXX) $(FLAGS_CPP) $(FLAGS_INC) $(FLAGS_LIB) -c $< -o $@

# Create subdirectories within build/ (holds .o and .d files).
$(DIR_BUILD)\\%\\:
	mkdir $@

.PHONY: clean
clean:
	rm -r $(DIR_BUILD)

-include $(DEPS_WIN)