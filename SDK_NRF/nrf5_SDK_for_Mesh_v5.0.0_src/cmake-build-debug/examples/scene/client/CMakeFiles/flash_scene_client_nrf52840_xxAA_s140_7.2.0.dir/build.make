# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug

# Utility rule file for flash_scene_client_nrf52840_xxAA_s140_7.2.0.

# Include the progress variables for this target.
include examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/progress.make

examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0: examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client && /usr/local/bin/python /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/CMake/nrfjprog.py /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex --chiperase

examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex: examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0.elf
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex"
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client && /usr/local/bin/mergehex -m /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/../nRF5_SDK_17.0.2_d674dde/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0.hex -o /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex

flash_scene_client_nrf52840_xxAA_s140_7.2.0: examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0
flash_scene_client_nrf52840_xxAA_s140_7.2.0: examples/scene/client/scene_client_nrf52840_xxAA_s140_7.2.0_merged.hex
flash_scene_client_nrf52840_xxAA_s140_7.2.0: examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/build.make

.PHONY : flash_scene_client_nrf52840_xxAA_s140_7.2.0

# Rule to build all files generated by this target.
examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/build: flash_scene_client_nrf52840_xxAA_s140_7.2.0

.PHONY : examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/build

examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/clean:
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client && $(CMAKE_COMMAND) -P CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/cmake_clean.cmake
.PHONY : examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/clean

examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/depend:
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/scene/client /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/scene/client/CMakeFiles/flash_scene_client_nrf52840_xxAA_s140_7.2.0.dir/depend

