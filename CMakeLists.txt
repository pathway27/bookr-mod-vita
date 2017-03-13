## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)

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

## Define project parameters here
# Name of the project
project(bookr-mod-vita)
# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "Bookr MOD Vita")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "BOKR00027")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "00.01")

execute_process(COMMAND git describe --always
  OUTPUT_VARIABLE GIT_VERSION)
string(REGEX REPLACE "\n$" "" GIT_VERSION "${GIT_VERSION}")
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

# Add any additional include paths here
include_directories(
  ${CMAKE_BINARY_DIR}
  "${CMAKE_SOURCE_DIR}/src"
  "$ENV{VITASDK}/arm-vita-eabi/include/freetype2"
)

# Debug CMAKE/ENV variables
get_cmake_property(_variableNames VARIABLES)
foreach (_variableName ${_variableNames})
   message(STATUS "${_variableName}=${${_variableName}}")
endforeach()
execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")
add_definitions(-DDEBUG)
#add_definitions(-DGENKIHEN)

	
# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
)


## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  image.o
  sce_sys/icon0.o

  src/bookr.cpp
  src/graphics/fzscreenvita.cpp
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
  
  # 

  src/bkdocument.cpp
  src/bkbookmark.cpp
  src/filetypes/bkfancytext.cpp
  # src/filetypes/bkdjvu.cpp
  # src/filetypes/bkpalmdoc.cpp
  src/filetypes/bkplaintext.cpp
  # src/
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
#-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
target_link_libraries(bookr-mod-vita
  vita2d
  freetype
  psp2shell
  SceDisplay_stub
  SceGxm_stub
  SceSysmodule_stub
  SceCtrl_stub
  ScePgf_stub
  ScePvf_stub
  SceCommonDialog_stub
  ScePower_stub
  SceCpuForKernel_stub
  SceNet_stub # these four and psp2shell for debug
  SceNetCtl_stub
  SceAppMgr_stub
  SceLibKernel_stub
  png
  jpeg
  z
  m
  c
  tinyxml2
)

## Create Vita files
vita_create_self(eboot.bin bookr-mod-vita
#  UNSAFE
)
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(bookr-mod-vita.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE sce_sys/icon0.png sce_sys/icon0.png
  FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
  FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
  FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)

# From VitaShell
add_custom_target(send
    COMMAND curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/
    DEPENDS eboot.bin
)

add_custom_target(copy
    COMMAND cp eboot.bin F:/app/${VITA_TITLEID}/eboot.bin
    DEPENDS eboot.bin
)
