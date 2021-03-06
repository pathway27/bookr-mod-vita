## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 3.0)
include(ExternalProject)

message("CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Use CMAKE_BINARY_DIR to determine target platform
# Defaults to building for Vita on *nix
IF(CMAKE_BINARY_DIR MATCHES desktop OR CMAKE_BINARY_DIR MATCHES CMakeBuild)
  set(DESKTOP ON)
ELSEIF(CMAKE_BINARY_DIR MATCHES switch)
  # TODO: move to toolchain file
  add_definitions(-DSWITCH)
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{DEVKITPRO})
      set(CMAKE_TOOLCHAIN_FILE "cmake/switch.toolchain.cmake" CACHE PATH "toolchain file")
    else()
      message(FATAL_ERROR "Please define DEVKITPRO to point to your SDK path!")
    endif()
  endif()
ELSE()
  ## This includes the Vita toolchain, must go before project definition
  # It is a convenience so you do not have to type
  # -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake for cmake. It is
  # highly recommended that you include this block for all projects.
  if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if(DEFINED ENV{VITASDK})
      set(CMAKE_TOOLCHAIN_FILE "$ENV{VITASDK}/share/vita.toolchain.cmake" CACHE PATH "toolchain file")
    else()
      message(FATAL_ERROR "Please define VITASDK to point to your SDK path!")
    endif()
  endif()
ENDIF()

## Define project parameters here
# Name of the project
project(bookr-mod-vita)

execute_process(
  COMMAND git describe
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_VERSION
)
string(REGEX REPLACE "\n$" "" GIT_VERSION "${GIT_VERSION}")

configure_file(
  "${CMAKE_SOURCE_DIR}/bookrconfig.h.in"
  "${CMAKE_BINARY_DIR}/bookrconfig.h"
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# Use CMAKE_BINARY_DIR Determine if release or debug variant
# Defaults to Debug
# TODO: This is bad apparently; but I prefer it rather than typing `cmake -DCMAKE_BUILD_TYPE=Release ..`
IF(${CMAKE_BINARY_DIR} MATCHES "Release")
  set(CMAKE_BUILD_TYPE Release)
ELSE()
  set(CMAKE_BUILD_TYPE Debug)
ENDIF()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

IF(CMAKE_BUILD_TYPE MATCHES Debug)
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
  execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
ENDIF()

# Debug CMAKE/ENV variables
message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
IF(CMAKE_BUILD_TYPE MATCHES Debug)
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
    message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
  execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
ENDIF()


## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

# Specific flags on Debug and Release
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG")

set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

# tinyxml2
set(TINYXML2_DIR "${CMAKE_SOURCE_DIR}/ext/tinyxml2" CACHE PATH "The path to the tinyxml2 library.")

option(BUILD_SHARED_LIBS "build as shared library" OFF)
option(BUILD_STATIC_LIBS "build as static library" ON)
add_subdirectory(${TINYXML2_DIR})

include_directories(
  ${TINYXML2_DIR}
)

file(GLOB res_files RELATIVE
  ${CMAKE_SOURCE_DIR}
  data/logos/icon0_t.png

  # data/fonts/URW Gothic L Demi.ttf
  data/icons/*.png
  data/fonts/NotoSans-Regular.ttf
)

file(GLOB shader_files RELATIVE
  ${CMAKE_SOURCE_DIR}
  src/graphics/shaders/*.vert
  src/graphics/shaders/*.frag
)

set(COMMON_SRCS
  src/main.cpp
  src/bookr.cpp
  
  src/utils.cpp
  src/graphics/screen_common.cpp
  src/layer_common.cpp

  src/user.cpp

  #texture image refcounted
  src/graphics/refcount.cpp
  src/graphics/image.cpp

  src/graphics/instreammem.cpp
  src/ui/logo.cpp
  src/ui/mainmenu.cpp
  src/ui/popup.cpp
  src/ui/filechooser.cpp

  src/bookmark.cpp
  src/document.cpp
  src/filetypes/fancytext.cpp
  src/filetypes/mudocument.cpp
  src/filetypes/plaintext.cpp
)

# TODO: Apparently this is bad
if(DESKTOP)
  add_definitions(-DDESKTOP)
  if(WIN32)
    include(cmake/win.cmake)
  elseif(APPLE)
    include(cmake/mac.cmake)
  endif(WIN32)
else()
  if(SWITCH)
    include(cmake/switch.cmake)
  elseif(VITA)
    include(cmake/vita.cmake)
  endif (SWITCH)
endif(DESKTOP)
