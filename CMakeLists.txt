## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)
set(DESKTOP ON CACHE BOOL "Build for Desktop")
add_definitions(-DDESKTOP)

if (NOT DESKTOP)
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
endif(NOT DESKTOP)

## Define project parameters here
# Name of the project
project(bookr-mod-vita)

if (NOT DESKTOP)
  # This line adds Vita helper macros, must go after project definition in order
  # to build Vita specific artifacts (self/vpk).
  include("${VITASDK}/share/vita.cmake" REQUIRED)

  ## Configuration options for this app
  # Display name (under bubble in LiveArea)
  set(VITA_APP_NAME "Bookr MOD Vita")
  # Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
  # unique string of developer and Y = a unique number for this app
  set(VITA_TITLEID  "PATH00027")
endif(NOT DESKTOP)

execute_process(COMMAND git describe --tags
  OUTPUT_VARIABLE GIT_VERSION)
string(REGEX REPLACE "\n$" "" GIT_VERSION "${GIT_VERSION}")

# bash "-c" "echo -n hello | sed 's/hello/world/;'"
execute_process(COMMAND git describe --tags
                COMMAND sed -e "s,v,0," -e "s,\\(.*\\)\\.,\\1," -e "s,-.*,,g"
                OUTPUT_VARIABLE VITA_VERSION)
string(REGEX REPLACE "\n$" "" VITA_VERSION "${VITA_VERSION}")
message(STATUS "VITA_VERSION: ${VITA_VERSION}")

# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION "${VITA_VERSION}")

configure_file(
  "${CMAKE_SOURCE_DIR}/bookrconfig.h.in"
  "${CMAKE_BINARY_DIR}/bookrconfig.h"
)

## Flags and includes for building
# Note that we make sure not to overwrite previous flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")

add_subdirectory(ext/tinyxml2)

if (WIN32)
  include_directories(
	  ${CMAKE_BINARY_DIR}
	  "${CMAKE_SOURCE_DIR}/src"
	  #"${CMAKE_SOURCE_DIR}/ext/mupdf/include"
	  "${CMAKE_SOURCE_DIR}/ext/glfw-3.2.1.bin.WIN32/include"
	  "${CMAKE_SOURCE_DIR}/ext/glew-2.0.0/include"
	  "${CMAKE_SOURCE_DIR}/ext/glm"
	  "${CMAKE_SOURCE_DIR}/ext/freetype-2.3.5-1-lib/include/freetype2"
	  "${CMAKE_SOURCE_DIR}/ext/freetype-2.3.5-1-lib/include"
	  "${CMAKE_SOURCE_DIR}/ext/SOIL/src"
  )

  link_directories(
	  ${CMAKE_CURRENT_BINARY_DIR}
	  "${CMAKE_SOURCE_DIR}/ext/mupdf/libs"
	  "${CMAKE_SOURCE_DIR}/ext/glew-2.0.0/lib/Release/Win32"
	  "${CMAKE_SOURCE_DIR}/ext/glfw-3.2.1.bin.WIN32/lib-vc2015"
	  "${CMAKE_SOURCE_DIR}/ext/SOIL/projects/VC9/Debug"
	  "${CMAKE_SOURCE_DIR}/ext/freetype-2.3.5-1-lib/lib"
	  "${CMAKE_SOURCE_DIR}/ext/freetype-2.3.5-1-bin/bin"
	)

  ## Build and link
  # Add all the files needed to compile here
  add_executable(bookr-mod-vita
	  #data/fonts/res_txtfont.c
	  #data/fonts/res_uifont.c
	  src/graphics/shaders/shader.cpp

	  src/bookr.cpp
	  src/graphics/fzscreenglfw.cpp
	  src/graphics/fzscreencommon.cpp

	  src/bkuser.cpp

	  #texture image refcounted
	  src/graphics/fzrefcount.cpp
	  src/graphics/fzimage.cpp
	  src/graphics/fztexture.cpp

	  src/graphics/fzfont.cpp
	  src/graphics/fzinstreammem.cpp
	  src/bklayer.cpp
	  src/bklogo.cpp
	  src/bkmainmenu.cpp
	  src/bkpopup.cpp
	  src/bkfilechooser.cpp

	  src/bkdocument.cpp
	  src/bkbookmark.cpp
	  src/filetypes/bkfancytext.cpp
	  src/filetypes/bkplaintext.cpp
	  #src/filetypes/bkpdf.cpp
	  # src/filetypes/bkdjvu.cpp
	  # src/filetypes/bkpalmdoc.cpp
  
	  # src/
  )


  target_link_libraries(bookr-mod-vita
	  OpenGL32
	  glew32s
	  glew32
	  glfw3
	  freetype
	  #freetype6.dll
	  tinyxml2
	  SOIL
  )
endif (WIN32)

# From VitaShell
add_custom_target(send
    COMMAND curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/
    DEPENDS eboot.bin
)

add_custom_target(copy
    COMMAND cp eboot.bin F:/app/${VITA_TITLEID}/eboot.bin
    DEPENDS eboot.bin
)
