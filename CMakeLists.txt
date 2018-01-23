## This file is a quick tutorial on writing CMakeLists for targeting the Vita
cmake_minimum_required(VERSION 2.8)
include(ExternalProject)

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


add_subdirectory("${CMAKE_SOURCE_DIR}/ext/tinyxml2")


set(COMMON_SRCS
  	src/bookr.cpp
	  src/graphics/fzscreencommon.cpp

	  src/bkuser.cpp

	  #texture image refcounted
	  src/graphics/fzrefcount.cpp
	  src/graphics/fzimage.cpp
	  src/graphics/fztexture.cpp

	  #src/graphics/fzfont.cpp
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
)

if (WIN32)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
  include_external_msproject(SOIL_WIN "${CMAKE_SOURCE_DIR}/ext/SOIL/projects/VS2017/SOIL.vcxproj")

  ExternalProject_Add(glew
    URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip
    URL_MD5 32a72e6b43367db8dbea6010cd095355
    #--Configure step-------------
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/glew"
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step---------------
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
  )

  ExternalProject_Add(glm
    URL https://github.com/g-truc/glm/releases/download/0.9.8.5/glm-0.9.8.5.zip
    URL_MD5 456c9f54cf9da189733a74c173b840b5
    #--Configure step-------------
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/glm"
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step---------------
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
  )

  ExternalProject_Add(glfw
    URL https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN32.zip
    URL_MD5 c1fce22f39deab17a819da9d23b3a002
    #--Configure step-------------
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/glfw"
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step---------------
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
  )

  ExternalProject_Add(freetype2
    URL https://github.com/ubawurinna/freetype-windows-binaries/archive/v2.6.5.zip
    URL_MD5 ab9e0e387b93daec8c486e5432cb8362
    #--Configure step-------------
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/freetype"
    CONFIGURE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND ""
    #--Install step---------------
    UPDATE_COMMAND "" # Skip annoying updates for every build
    INSTALL_COMMAND ""
  )

  include_directories(
	  ${CMAKE_BINARY_DIR}
	  "${CMAKE_SOURCE_DIR}/src"
	  #"${CMAKE_SOURCE_DIR}/ext/mupdf/include"
	  "${CMAKE_SOURCE_DIR}/ext/glfw/include"
	  "${CMAKE_SOURCE_DIR}/ext/glew/include"
	  "${CMAKE_SOURCE_DIR}/ext/glm"
	  "${CMAKE_SOURCE_DIR}/ext/freetype/include/freetype2"
	  "${CMAKE_SOURCE_DIR}/ext/freetype/include"
	  "${CMAKE_SOURCE_DIR}/ext/SOIL/src"
  )

  link_directories(
	  ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_CURRENT_BINARY_DIR}/ext/tinyxml2/Debug"
	  "${CMAKE_SOURCE_DIR}/ext/mupdf/libs"
	  "${CMAKE_SOURCE_DIR}/ext/glew/lib/Release/Win32"
	  "${CMAKE_SOURCE_DIR}/ext/glfw/lib-vc2015"
	  "${CMAKE_SOURCE_DIR}/ext/SOIL/projects/VS2017/Debug"
	  "${CMAKE_SOURCE_DIR}/ext/freetype/win32"
    "${CMAKE_SOURCE_DIR}/ext/psp2shell/win32"
	)

  ## Build and link
  # Add all the files needed to compile here
  add_executable(bookr-mod-vita
	  ${COMMON_SRCS}
    src/graphics/shaders/shader.cpp
	  src/graphics/fzscreenglfw.cpp
  )
  

  target_link_libraries(bookr-mod-vita
	  OpenGL32
	  glew32s
	  glew32
	  glfw3
	  freetype265
	  #freetype6.dll
	  tinyxml2
	  SOIL
  )

  add_dependencies(bookr-mod-vita glew glm glfw freetype2 tinyxml2 SOIL_WIN)
else()
  # Add any additional include paths here
  include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/ext/psp2shell/psp2shell_m/include
    ${CMAKE_SOURCE_DIR}/ext/psp2shell/common
    $ENV{VITASDK}/arm-vita-eabi/include/freetype2
    # "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
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
    "${CMAKE_SOURCE_DIR}/ext/mupdf/libs"
  )


  ## Build and link
  # Add all the files needed to compile here
  add_executable(bookr-mod-vita
    ${COMMON_SRCS}
    
    image.o
    sce_sys/icon0_t.o
    data/fonts/res_txtfont.c
    data/fonts/res_uifont.c

    src/graphics/fzscreenvita.cpp
    
    # src/filetypes/bkpdf.cpp
  )

  # Library to link to (drop the -l prefix). This will mostly be stubs.
  #-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
  #mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm 
  target_link_libraries(bookr-mod-vita
    vita2d
    mupdf
    freetype
    raster
    world
    fonts
    stream
    base
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

  vita_create_vpk(bookr-mod-vita.vpk ${VITA_TITLEID} eboot.bin
    VERSION ${VITA_VERSION}
    NAME ${VITA_APP_NAME}
    FILE sce_sys/icon0.png sce_sys/icon0.png
    FILE sce_sys/livearea/contents/bg.png sce_sys/livearea/contents/bg.png
    FILE sce_sys/livearea/contents/startup.png sce_sys/livearea/contents/startup.png
    FILE sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
  )
endif (WIN32)

# From VitaShell
add_custom_target(send
    COMMAND curl -T eboot.bin ftp://$(PSVITAIP):1337/ux0:/app/${VITA_TITLEID}/
    DEPENDS eboot.bin
)

add_custom_target(sendvpk
    COMMAND curl -T bookr-mod-vita.vpk ftp://$(PSVITAIP):1337/ux0:/vpk/
    DEPENDS bookr-mod-vita.vpk
)

add_custom_target(copy
    COMMAND cp eboot.bin F:/app/${VITA_TITLEID}/eboot.bin
    DEPENDS bookr-mod-vita.self
)
