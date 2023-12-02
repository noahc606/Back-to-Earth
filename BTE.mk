##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=BTE
ConfigurationName      :=Debug
WorkspaceConfiguration :=Debug
WorkspacePath          :=/home/nch/Technical/CodeLite/C++
ProjectPath            :=/home/nch/Technical/Git/Back-to-Earth/BTE
IntermediateDirectory  :=$(ProjectPath)/bin/src
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Noah Hebert
Date                   :=12/02/2023
CodeLitePath           :=/home/nch/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputDirectory        :=$(ProjectPath)/bin
OutputFile             :=$(ProjectPath)/bin/Back-to-Earth-unix
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="BTE.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch)$(ProjectPath)/include/SDL2_image-2.0.5-i686-w64-mingw32/include/SDL2 $(IncludeSwitch)$(ProjectPath)/include/SDL2_mixer-2.0.4-i686-w64-mingw32/include/SDL2 $(IncludeSwitch)$(ProjectPath)/include/SDL2_ttf-2.0.15-i686-w64-mingw32/include/SDL2 $(IncludeSwitch)$(ProjectPath)/include/SDL2-2.0.10-i686-w64-mingw32/include/SDL2 $(IncludeSwitch)$(ProjectPath)/src $(IncludeSwitch)$(ProjectPath)/src/gfx $(IncludeSwitch)$(ProjectPath)/src/gfx/gui $(IncludeSwitch)$(ProjectPath)/src/gfx/gui/obj $(IncludeSwitch)$(ProjectPath)/src/gfx/texture $(IncludeSwitch)$(ProjectPath)/src/gfx/texture/access $(IncludeSwitch)$(ProjectPath)/src/helper $(IncludeSwitch)$(ProjectPath)/src/helper/base $(IncludeSwitch)$(ProjectPath)/src/helper/math $(IncludeSwitch)$(ProjectPath)/src/helper/type $(IncludeSwitch)$(ProjectPath)/src/io $(IncludeSwitch)$(ProjectPath)/src/io/debug $(IncludeSwitch)$(ProjectPath)/src/io/file $(IncludeSwitch)$(ProjectPath)/src/io/resources $(IncludeSwitch)$(ProjectPath)/src/main $(IncludeSwitch)$(ProjectPath)/src/world $(IncludeSwitch)$(ProjectPath)/src/world/gen $(IncludeSwitch)$(ProjectPath)/src/world/gen/noise $(IncludeSwitch)$(ProjectPath)/src/world/phys $(IncludeSwitch)$(ProjectPath)/src/world/tile 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overridden using an environment variable
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -gdwarf-2 $(Preprocessors)
CFLAGS   :=  -gdwarf-2 $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_TileType.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_TileMap.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_debug_Timer.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_helper_type_Defs.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_gfx_Text.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_ControlBinding.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_Settings.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_Player.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main_Tests.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main_SDLHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_main_Main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_Controls.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_Color.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_ButtonAction.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_helper_type_Real.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_Camera.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_helper_NameBuilder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_PlayerMenu.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_World.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main_MainLoop.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main_BTE.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_phys_Point3D.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(ObjectSuffix) 

Objects1=$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_FileHandler.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_FilePath.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_DataStream.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_file_LevelSave.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_debug_Loggable.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_debug_Commands.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_io_debug_Log.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_world_gen_Noise.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(ObjectSuffix) 



Objects=$(Objects0) $(Objects1) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	@echo $(Objects1) >> $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d $(ProjectPath)/bin/src || $(MakeDirCommand) $(ProjectPath)/bin/src


$(IntermediateDirectory)/.d:
	@test -d $(ProjectPath)/bin/src || $(MakeDirCommand) $(ProjectPath)/bin/src

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(ObjectSuffix): src/gfx/texture/access/STexture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(DependSuffix) -MM src/gfx/texture/access/STexture.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/access/STexture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(PreprocessSuffix): src/gfx/texture/access/STexture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_access_STexture.cpp$(PreprocessSuffix) src/gfx/texture/access/STexture.cpp

$(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(ObjectSuffix): src/gfx/texture/access/TTexture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(DependSuffix) -MM src/gfx/texture/access/TTexture.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/access/TTexture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(PreprocessSuffix): src/gfx/texture/access/TTexture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_access_TTexture.cpp$(PreprocessSuffix) src/gfx/texture/access/TTexture.cpp

$(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(ObjectSuffix): src/world/tile/RegTexBuilder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(DependSuffix) -MM src/world/tile/RegTexBuilder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/RegTexBuilder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(PreprocessSuffix): src/world/tile/RegTexBuilder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_RegTexBuilder.cpp$(PreprocessSuffix) src/world/tile/RegTexBuilder.cpp

$(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(ObjectSuffix): src/world/tile/TileMapScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(DependSuffix) -MM src/world/tile/TileMapScreen.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/TileMapScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(PreprocessSuffix): src/world/tile/TileMapScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_TileMapScreen.cpp$(PreprocessSuffix) src/world/tile/TileMapScreen.cpp

$(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(ObjectSuffix): src/gfx/gui/WindowData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(DependSuffix) -MM src/gfx/gui/WindowData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/WindowData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(PreprocessSuffix): src/gfx/gui/WindowData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_WindowData.cpp$(PreprocessSuffix) src/gfx/gui/WindowData.cpp

$(IntermediateDirectory)/src_world_tile_TileType.cpp$(ObjectSuffix): src/world/tile/TileType.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_TileType.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_TileType.cpp$(DependSuffix) -MM src/world/tile/TileType.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/TileType.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_TileType.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_TileType.cpp$(PreprocessSuffix): src/world/tile/TileType.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_TileType.cpp$(PreprocessSuffix) src/world/tile/TileType.cpp

$(IntermediateDirectory)/src_world_tile_TileMap.cpp$(ObjectSuffix): src/world/tile/TileMap.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_TileMap.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_TileMap.cpp$(DependSuffix) -MM src/world/tile/TileMap.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/TileMap.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_TileMap.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_TileMap.cpp$(PreprocessSuffix): src/world/tile/TileMap.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_TileMap.cpp$(PreprocessSuffix) src/world/tile/TileMap.cpp

$(IntermediateDirectory)/src_io_debug_Timer.cpp$(ObjectSuffix): src/io/debug/Timer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_Timer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_Timer.cpp$(DependSuffix) -MM src/io/debug/Timer.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/Timer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_Timer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_Timer.cpp$(PreprocessSuffix): src/io/debug/Timer.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_Timer.cpp$(PreprocessSuffix) src/io/debug/Timer.cpp

$(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(ObjectSuffix): src/world/tile/TileIterator.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(DependSuffix) -MM src/world/tile/TileIterator.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/TileIterator.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(PreprocessSuffix): src/world/tile/TileIterator.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_TileIterator.cpp$(PreprocessSuffix) src/world/tile/TileIterator.cpp

$(IntermediateDirectory)/src_helper_type_Defs.cpp$(ObjectSuffix): src/helper/type/Defs.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_helper_type_Defs.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_helper_type_Defs.cpp$(DependSuffix) -MM src/helper/type/Defs.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/helper/type/Defs.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_helper_type_Defs.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_helper_type_Defs.cpp$(PreprocessSuffix): src/helper/type/Defs.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_helper_type_Defs.cpp$(PreprocessSuffix) src/helper/type/Defs.cpp

$(IntermediateDirectory)/src_gfx_Text.cpp$(ObjectSuffix): src/gfx/Text.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_Text.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_Text.cpp$(DependSuffix) -MM src/gfx/Text.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/Text.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_Text.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_Text.cpp$(PreprocessSuffix): src/gfx/Text.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_Text.cpp$(PreprocessSuffix) src/gfx/Text.cpp

$(IntermediateDirectory)/src_io_ControlBinding.cpp$(ObjectSuffix): src/io/ControlBinding.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_ControlBinding.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_ControlBinding.cpp$(DependSuffix) -MM src/io/ControlBinding.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/ControlBinding.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_ControlBinding.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_ControlBinding.cpp$(PreprocessSuffix): src/io/ControlBinding.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_ControlBinding.cpp$(PreprocessSuffix) src/io/ControlBinding.cpp

$(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(ObjectSuffix): src/world/tile/RegTexUpdates.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(DependSuffix) -MM src/world/tile/RegTexUpdates.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/RegTexUpdates.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(PreprocessSuffix): src/world/tile/RegTexUpdates.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_RegTexUpdates.cpp$(PreprocessSuffix) src/world/tile/RegTexUpdates.cpp

$(IntermediateDirectory)/src_io_Settings.cpp$(ObjectSuffix): src/io/Settings.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_Settings.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_Settings.cpp$(DependSuffix) -MM src/io/Settings.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/Settings.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_Settings.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_Settings.cpp$(PreprocessSuffix): src/io/Settings.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_Settings.cpp$(PreprocessSuffix) src/io/Settings.cpp

$(IntermediateDirectory)/src_world_Player.cpp$(ObjectSuffix): src/world/Player.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_Player.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_Player.cpp$(DependSuffix) -MM src/world/Player.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/Player.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_Player.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_Player.cpp$(PreprocessSuffix): src/world/Player.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_Player.cpp$(PreprocessSuffix) src/world/Player.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(ObjectSuffix): src/gfx/gui/obj/Button.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(DependSuffix) -MM src/gfx/gui/obj/Button.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/Button.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(PreprocessSuffix): src/gfx/gui/obj/Button.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_Button.cpp$(PreprocessSuffix) src/gfx/gui/obj/Button.cpp

$(IntermediateDirectory)/src_main_Tests.cpp$(ObjectSuffix): src/main/Tests.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main_Tests.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main_Tests.cpp$(DependSuffix) -MM src/main/Tests.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/main/Tests.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main_Tests.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main_Tests.cpp$(PreprocessSuffix): src/main/Tests.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main_Tests.cpp$(PreprocessSuffix) src/main/Tests.cpp

$(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(ObjectSuffix): src/gfx/GUIBuilder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(DependSuffix) -MM src/gfx/GUIBuilder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/GUIBuilder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(PreprocessSuffix): src/gfx/GUIBuilder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_GUIBuilder.cpp$(PreprocessSuffix) src/gfx/GUIBuilder.cpp

$(IntermediateDirectory)/src_main_SDLHandler.cpp$(ObjectSuffix): src/main/SDLHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main_SDLHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main_SDLHandler.cpp$(DependSuffix) -MM src/main/SDLHandler.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/main/SDLHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main_SDLHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main_SDLHandler.cpp$(PreprocessSuffix): src/main/SDLHandler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main_SDLHandler.cpp$(PreprocessSuffix) src/main/SDLHandler.cpp

$(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(ObjectSuffix): src/io/file/TextureLoader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(DependSuffix) -MM src/io/file/TextureLoader.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/TextureLoader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(PreprocessSuffix): src/io/file/TextureLoader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_TextureLoader.cpp$(PreprocessSuffix) src/io/file/TextureLoader.cpp

$(IntermediateDirectory)/src_main_Main.cpp$(ObjectSuffix): src/main/Main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main_Main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main_Main.cpp$(DependSuffix) -MM src/main/Main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/main/Main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main_Main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main_Main.cpp$(PreprocessSuffix): src/main/Main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main_Main.cpp$(PreprocessSuffix) src/main/Main.cpp

$(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(ObjectSuffix): src/gfx/gui/WindowComponent.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(DependSuffix) -MM src/gfx/gui/WindowComponent.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/WindowComponent.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(PreprocessSuffix): src/gfx/gui/WindowComponent.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_WindowComponent.cpp$(PreprocessSuffix) src/gfx/gui/WindowComponent.cpp

$(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(ObjectSuffix): src/gfx/texture/SpriteSheet.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(DependSuffix) -MM src/gfx/texture/SpriteSheet.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/SpriteSheet.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(PreprocessSuffix): src/gfx/texture/SpriteSheet.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_SpriteSheet.cpp$(PreprocessSuffix) src/gfx/texture/SpriteSheet.cpp

$(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(ObjectSuffix): src/helper/base/BTEObject.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(DependSuffix) -MM src/helper/base/BTEObject.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/helper/base/BTEObject.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(PreprocessSuffix): src/helper/base/BTEObject.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_helper_base_BTEObject.cpp$(PreprocessSuffix) src/helper/base/BTEObject.cpp

$(IntermediateDirectory)/src_io_Controls.cpp$(ObjectSuffix): src/io/Controls.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_Controls.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_Controls.cpp$(DependSuffix) -MM src/io/Controls.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/Controls.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_Controls.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_Controls.cpp$(PreprocessSuffix): src/io/Controls.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_Controls.cpp$(PreprocessSuffix) src/io/Controls.cpp

$(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(ObjectSuffix): src/helper/math/Vec2D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(DependSuffix) -MM src/helper/math/Vec2D.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/helper/math/Vec2D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(PreprocessSuffix): src/helper/math/Vec2D.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_helper_math_Vec2D.cpp$(PreprocessSuffix) src/helper/math/Vec2D.cpp

$(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(ObjectSuffix): src/world/phys/CollidingVolume.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(DependSuffix) -MM src/world/phys/CollidingVolume.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/phys/CollidingVolume.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(PreprocessSuffix): src/world/phys/CollidingVolume.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_phys_CollidingVolume.cpp$(PreprocessSuffix) src/world/phys/CollidingVolume.cpp

$(IntermediateDirectory)/src_gfx_texture_Color.cpp$(ObjectSuffix): src/gfx/texture/Color.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_Color.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_Color.cpp$(DependSuffix) -MM src/gfx/texture/Color.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/Color.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_Color.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_Color.cpp$(PreprocessSuffix): src/gfx/texture/Color.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_Color.cpp$(PreprocessSuffix) src/gfx/texture/Color.cpp

$(IntermediateDirectory)/src_io_ButtonAction.cpp$(ObjectSuffix): src/io/ButtonAction.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_ButtonAction.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_ButtonAction.cpp$(DependSuffix) -MM src/io/ButtonAction.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/ButtonAction.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_ButtonAction.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_ButtonAction.cpp$(PreprocessSuffix): src/io/ButtonAction.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_ButtonAction.cpp$(PreprocessSuffix) src/io/ButtonAction.cpp

$(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(ObjectSuffix): src/gfx/GUIAligner.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(DependSuffix) -MM src/gfx/GUIAligner.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/GUIAligner.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(PreprocessSuffix): src/gfx/GUIAligner.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_GUIAligner.cpp$(PreprocessSuffix) src/gfx/GUIAligner.cpp

$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(ObjectSuffix): src/io/debug/DebugScreen.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(DependSuffix) -MM src/io/debug/DebugScreen.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/DebugScreen.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(PreprocessSuffix): src/io/debug/DebugScreen.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_DebugScreen.cpp$(PreprocessSuffix) src/io/debug/DebugScreen.cpp

$(IntermediateDirectory)/src_helper_type_Real.cpp$(ObjectSuffix): src/helper/type/Real.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_helper_type_Real.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_helper_type_Real.cpp$(DependSuffix) -MM src/helper/type/Real.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/helper/type/Real.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_helper_type_Real.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_helper_type_Real.cpp$(PreprocessSuffix): src/helper/type/Real.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_helper_type_Real.cpp$(PreprocessSuffix) src/helper/type/Real.cpp

$(IntermediateDirectory)/src_gfx_Camera.cpp$(ObjectSuffix): src/gfx/Camera.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_Camera.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_Camera.cpp$(DependSuffix) -MM src/gfx/Camera.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/Camera.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_Camera.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_Camera.cpp$(PreprocessSuffix): src/gfx/Camera.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_Camera.cpp$(PreprocessSuffix) src/gfx/Camera.cpp

$(IntermediateDirectory)/src_helper_NameBuilder.cpp$(ObjectSuffix): src/helper/NameBuilder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_helper_NameBuilder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_helper_NameBuilder.cpp$(DependSuffix) -MM src/helper/NameBuilder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/helper/NameBuilder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_helper_NameBuilder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_helper_NameBuilder.cpp$(PreprocessSuffix): src/helper/NameBuilder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_helper_NameBuilder.cpp$(PreprocessSuffix) src/helper/NameBuilder.cpp

$(IntermediateDirectory)/src_world_PlayerMenu.cpp$(ObjectSuffix): src/world/PlayerMenu.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_PlayerMenu.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_PlayerMenu.cpp$(DependSuffix) -MM src/world/PlayerMenu.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/PlayerMenu.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_PlayerMenu.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_PlayerMenu.cpp$(PreprocessSuffix): src/world/PlayerMenu.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_PlayerMenu.cpp$(PreprocessSuffix) src/world/PlayerMenu.cpp

$(IntermediateDirectory)/src_world_World.cpp$(ObjectSuffix): src/world/World.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_World.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_World.cpp$(DependSuffix) -MM src/world/World.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/World.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_World.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_World.cpp$(PreprocessSuffix): src/world/World.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_World.cpp$(PreprocessSuffix) src/world/World.cpp

$(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(ObjectSuffix): src/world/tile/TileRegion.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(DependSuffix) -MM src/world/tile/TileRegion.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/tile/TileRegion.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(PreprocessSuffix): src/world/tile/TileRegion.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_tile_TileRegion.cpp$(PreprocessSuffix) src/world/tile/TileRegion.cpp

$(IntermediateDirectory)/src_main_MainLoop.cpp$(ObjectSuffix): src/main/MainLoop.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main_MainLoop.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main_MainLoop.cpp$(DependSuffix) -MM src/main/MainLoop.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/main/MainLoop.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main_MainLoop.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main_MainLoop.cpp$(PreprocessSuffix): src/main/MainLoop.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main_MainLoop.cpp$(PreprocessSuffix) src/main/MainLoop.cpp

$(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(ObjectSuffix): src/gfx/GUIHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(DependSuffix) -MM src/gfx/GUIHandler.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/GUIHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(PreprocessSuffix): src/gfx/GUIHandler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_GUIHandler.cpp$(PreprocessSuffix) src/gfx/GUIHandler.cpp

$(IntermediateDirectory)/src_main_BTE.cpp$(ObjectSuffix): src/main/BTE.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main_BTE.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main_BTE.cpp$(DependSuffix) -MM src/main/BTE.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/main/BTE.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main_BTE.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main_BTE.cpp$(PreprocessSuffix): src/main/BTE.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main_BTE.cpp$(PreprocessSuffix) src/main/BTE.cpp

$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(ObjectSuffix): src/gfx/gui/GUI.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(DependSuffix) -MM src/gfx/gui/GUI.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/GUI.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(PreprocessSuffix): src/gfx/gui/GUI.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_GUI.cpp$(PreprocessSuffix) src/gfx/gui/GUI.cpp

$(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(ObjectSuffix): src/gfx/gui/GUIAlignable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(DependSuffix) -MM src/gfx/gui/GUIAlignable.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/GUIAlignable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(PreprocessSuffix): src/gfx/gui/GUIAlignable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_GUIAlignable.cpp$(PreprocessSuffix) src/gfx/gui/GUIAlignable.cpp

$(IntermediateDirectory)/src_world_phys_Point3D.cpp$(ObjectSuffix): src/world/phys/Point3D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_phys_Point3D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_phys_Point3D.cpp$(DependSuffix) -MM src/world/phys/Point3D.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/phys/Point3D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_phys_Point3D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_phys_Point3D.cpp$(PreprocessSuffix): src/world/phys/Point3D.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_phys_Point3D.cpp$(PreprocessSuffix) src/world/phys/Point3D.cpp

$(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(ObjectSuffix): src/gfx/gui/WindowPanelData.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(DependSuffix) -MM src/gfx/gui/WindowPanelData.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/WindowPanelData.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(PreprocessSuffix): src/gfx/gui/WindowPanelData.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_WindowPanelData.cpp$(PreprocessSuffix) src/gfx/gui/WindowPanelData.cpp

$(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(ObjectSuffix): src/io/file/AudioLoader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(DependSuffix) -MM src/io/file/AudioLoader.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/AudioLoader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(PreprocessSuffix): src/io/file/AudioLoader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_AudioLoader.cpp$(PreprocessSuffix) src/io/file/AudioLoader.cpp

$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(ObjectSuffix): src/gfx/texture/SpriteSheetBuilder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(DependSuffix) -MM src/gfx/texture/SpriteSheetBuilder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/SpriteSheetBuilder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(PreprocessSuffix): src/gfx/texture/SpriteSheetBuilder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_SpriteSheetBuilder.cpp$(PreprocessSuffix) src/gfx/texture/SpriteSheetBuilder.cpp

$(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(ObjectSuffix): src/gfx/texture/TextureBuilder.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(DependSuffix) -MM src/gfx/texture/TextureBuilder.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/TextureBuilder.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(PreprocessSuffix): src/gfx/texture/TextureBuilder.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_TextureBuilder.cpp$(PreprocessSuffix) src/gfx/texture/TextureBuilder.cpp

$(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(ObjectSuffix): src/gfx/texture/Texture.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(DependSuffix) -MM src/gfx/texture/Texture.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/Texture.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(PreprocessSuffix): src/gfx/texture/Texture.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_Texture.cpp$(PreprocessSuffix) src/gfx/texture/Texture.cpp

$(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(ObjectSuffix): src/gfx/texture/Canvas.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(DependSuffix) -MM src/gfx/texture/Canvas.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/Canvas.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(PreprocessSuffix): src/gfx/texture/Canvas.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_Canvas.cpp$(PreprocessSuffix) src/gfx/texture/Canvas.cpp

$(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(ObjectSuffix): src/gfx/texture/ColorPalette.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(DependSuffix) -MM src/gfx/texture/ColorPalette.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/texture/ColorPalette.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(PreprocessSuffix): src/gfx/texture/ColorPalette.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_texture_ColorPalette.cpp$(PreprocessSuffix) src/gfx/texture/ColorPalette.cpp

$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(ObjectSuffix): src/world/phys/Box3D.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(DependSuffix) -MM src/world/phys/Box3D.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/phys/Box3D.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_phys_Box3D.cpp$(PreprocessSuffix): src/world/phys/Box3D.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_phys_Box3D.cpp$(PreprocessSuffix) src/world/phys/Box3D.cpp

$(IntermediateDirectory)/src_io_file_FileHandler.cpp$(ObjectSuffix): src/io/file/FileHandler.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_FileHandler.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_FileHandler.cpp$(DependSuffix) -MM src/io/file/FileHandler.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/FileHandler.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_FileHandler.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_FileHandler.cpp$(PreprocessSuffix): src/io/file/FileHandler.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_FileHandler.cpp$(PreprocessSuffix) src/io/file/FileHandler.cpp

$(IntermediateDirectory)/src_io_file_FilePath.cpp$(ObjectSuffix): src/io/file/FilePath.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_FilePath.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_FilePath.cpp$(DependSuffix) -MM src/io/file/FilePath.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/FilePath.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_FilePath.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_FilePath.cpp$(PreprocessSuffix): src/io/file/FilePath.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_FilePath.cpp$(PreprocessSuffix) src/io/file/FilePath.cpp

$(IntermediateDirectory)/src_io_file_DataStream.cpp$(ObjectSuffix): src/io/file/DataStream.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_DataStream.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_DataStream.cpp$(DependSuffix) -MM src/io/file/DataStream.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/DataStream.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_DataStream.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_DataStream.cpp$(PreprocessSuffix): src/io/file/DataStream.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_DataStream.cpp$(PreprocessSuffix) src/io/file/DataStream.cpp

$(IntermediateDirectory)/src_io_file_LevelSave.cpp$(ObjectSuffix): src/io/file/LevelSave.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_file_LevelSave.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_file_LevelSave.cpp$(DependSuffix) -MM src/io/file/LevelSave.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/file/LevelSave.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_file_LevelSave.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_file_LevelSave.cpp$(PreprocessSuffix): src/io/file/LevelSave.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_file_LevelSave.cpp$(PreprocessSuffix) src/io/file/LevelSave.cpp

$(IntermediateDirectory)/src_io_debug_Loggable.cpp$(ObjectSuffix): src/io/debug/Loggable.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_Loggable.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_Loggable.cpp$(DependSuffix) -MM src/io/debug/Loggable.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/Loggable.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_Loggable.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_Loggable.cpp$(PreprocessSuffix): src/io/debug/Loggable.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_Loggable.cpp$(PreprocessSuffix) src/io/debug/Loggable.cpp

$(IntermediateDirectory)/src_io_debug_Commands.cpp$(ObjectSuffix): src/io/debug/Commands.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_Commands.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_Commands.cpp$(DependSuffix) -MM src/io/debug/Commands.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/Commands.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_Commands.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_Commands.cpp$(PreprocessSuffix): src/io/debug/Commands.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_Commands.cpp$(PreprocessSuffix) src/io/debug/Commands.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(ObjectSuffix): src/gfx/gui/obj/CheckBox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(DependSuffix) -MM src/gfx/gui/obj/CheckBox.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/CheckBox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(PreprocessSuffix): src/gfx/gui/obj/CheckBox.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_CheckBox.cpp$(PreprocessSuffix) src/gfx/gui/obj/CheckBox.cpp

$(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(ObjectSuffix): src/io/debug/CommandParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(DependSuffix) -MM src/io/debug/CommandParser.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/CommandParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(PreprocessSuffix): src/io/debug/CommandParser.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_CommandParser.cpp$(PreprocessSuffix) src/io/debug/CommandParser.cpp

$(IntermediateDirectory)/src_io_debug_Log.cpp$(ObjectSuffix): src/io/debug/Log.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_io_debug_Log.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_io_debug_Log.cpp$(DependSuffix) -MM src/io/debug/Log.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/io/debug/Log.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_io_debug_Log.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_io_debug_Log.cpp$(PreprocessSuffix): src/io/debug/Log.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_io_debug_Log.cpp$(PreprocessSuffix) src/io/debug/Log.cpp

$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(ObjectSuffix): src/world/gen/noise/Terrain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(DependSuffix) -MM src/world/gen/noise/Terrain.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/gen/noise/Terrain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(PreprocessSuffix): src/world/gen/noise/Terrain.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_gen_noise_Terrain.cpp$(PreprocessSuffix) src/world/gen/noise/Terrain.cpp

$(IntermediateDirectory)/src_world_gen_Noise.cpp$(ObjectSuffix): src/world/gen/Noise.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_world_gen_Noise.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_world_gen_Noise.cpp$(DependSuffix) -MM src/world/gen/Noise.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/world/gen/Noise.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_world_gen_Noise.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_world_gen_Noise.cpp$(PreprocessSuffix): src/world/gen/Noise.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_world_gen_Noise.cpp$(PreprocessSuffix) src/world/gen/Noise.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(ObjectSuffix): src/gfx/gui/obj/Window.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(DependSuffix) -MM src/gfx/gui/obj/Window.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/Window.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(PreprocessSuffix): src/gfx/gui/obj/Window.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_Window.cpp$(PreprocessSuffix) src/gfx/gui/obj/Window.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(ObjectSuffix): src/gfx/gui/obj/Icon.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(DependSuffix) -MM src/gfx/gui/obj/Icon.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/Icon.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(PreprocessSuffix): src/gfx/gui/obj/Icon.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_Icon.cpp$(PreprocessSuffix) src/gfx/gui/obj/Icon.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(ObjectSuffix): src/gfx/gui/obj/TextBox.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(DependSuffix) -MM src/gfx/gui/obj/TextBox.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/TextBox.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(PreprocessSuffix): src/gfx/gui/obj/TextBox.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_TextBox.cpp$(PreprocessSuffix) src/gfx/gui/obj/TextBox.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(ObjectSuffix): src/gfx/gui/obj/Tooltip.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(DependSuffix) -MM src/gfx/gui/obj/Tooltip.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/Tooltip.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(PreprocessSuffix): src/gfx/gui/obj/Tooltip.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_Tooltip.cpp$(PreprocessSuffix) src/gfx/gui/obj/Tooltip.cpp

$(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(ObjectSuffix): src/gfx/gui/obj/RadioButton.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(DependSuffix) -MM src/gfx/gui/obj/RadioButton.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/nch/Technical/Git/Back-to-Earth/BTE/src/gfx/gui/obj/RadioButton.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(PreprocessSuffix): src/gfx/gui/obj/RadioButton.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_gfx_gui_obj_RadioButton.cpp$(PreprocessSuffix) src/gfx/gui/obj/RadioButton.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r $(ProjectPath)/bin/src/


