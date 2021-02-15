# Install script for directory: /Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/examples

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/common/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/beaconing/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/pb_remote/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/provisioner/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/sensor/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/scene/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_lightness/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_lc/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_ctl/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/enocean_switch/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/dimming/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/lpn/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/dfu/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/serial/cmake_install.cmake")
  include("/Users/vitaliy/Downloads/nrf5_SDK_for_Mesh_v5.0.0_src/cmake-build-debug/examples/light_switch/cmake_install.cmake")

endif()

