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
  include(CMakeWindows)
else()
  # TODO: turn images to binary with
  # arm-vita-eabi-ld -r -b binary -o icon0_t.o icon0_t.png


  ExternalProject_Add(mupdf_lib
    PREFIX "${CMAKE_SOURCE_DIR}/ext/mupdf"
    GIT_REPOSITORY https://github.com/pathway27/mupdf
    GIT_TAG origin/1.12.0-vita
    GIT_PROGRESS 1
    #--Configure step-------------
    SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/mupdf"
    CONFIGURE_COMMAND make generate
    UPDATE_COMMAND ""
    #--Build step-----------------
    BUILD_COMMAND make OS=vita build=release prefix=/usr/local/vitasdk/arm-vita-eabi HAVE_X11=no HAVE_GLUT=no
    BUILD_IN_SOURCE 1
    #--Install step---------------
    INSTALL_COMMAND ""
  )

  # Add any additional include paths here
  include_directories(
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/ext/psp2shell/psp2shell_m/include
    ${CMAKE_SOURCE_DIR}/ext/psp2shell/common
    $ENV{VITASDK}/arm-vita-eabi/include/freetype2
    "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  )

  # Debug CMAKE/ENV variables
  get_cmake_property(_variableNames VARIABLES)
  foreach (_variableName ${_variableNames})
     message(STATUS "${_variableName}=${${_variableName}}")
  endforeach()
  execute_process(COMMAND "${CMAKE_COMMAND}" "-E" "environment")

  message("CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}")
  IF(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG)
    #add_definitions(-DDEBUG_RENDER)
  ENDIF()

  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -O3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -std=c++11")
  #add_definitions(-DGENKIHEN)


  # Add any additional library paths here
  # ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
  link_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    "${CMAKE_SOURCE_DIR}/ext/mupdf/build/release"
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

    src/filetypes/bkmudocument.cpp
  )

  # Library to link to (drop the -l prefix). This will mostly be stubs.
  #-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
  #mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm
  target_link_libraries(bookr-mod-vita
    vita2d
    mupdf
    mupdfthird
    pthread
    freetype
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

  add_dependencies(bookr-mod-vita mupdf_lib)
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
