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

# Utility rule file for app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.

# Include the progress variables for this target.
include examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/progress.make

examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0: examples/light_ctl/ctl_lc_server/light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.hex
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/ctl_lc_server && /usr/local/bin/python /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/CMake/nrfjprog.py /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/ctl_lc_server/light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.hex --sectorerase

app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0: examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0
app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0: examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/build.make

.PHONY : app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0

# Rule to build all files generated by this target.
examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/build: app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0

.PHONY : examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/build

examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/clean:
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/ctl_lc_server && $(CMAKE_COMMAND) -P CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/cmake_clean.cmake
.PHONY : examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/clean

examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/depend:
	cd /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples/light_ctl/ctl_lc_server /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/ctl_lc_server /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : examples/light_ctl/ctl_lc_server/CMakeFiles/app_flash_light_ctl_lc_server_nrf52832_xxAA_s132_7.2.0.dir/depend

