# This line adds Vita helper macros, must go after project definition in order
# to build Vita specific artifacts (self/vpk).
include("${VITASDK}/share/vita.cmake" REQUIRED)
option(BUILD_SHARED_LIBS "build as shared library" ON)

# turn images to binary with
# https://beesbuzz.biz/blog/e/2014/07/31-embedding_binary_resources_with_cmake_and_c11.php
FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
    GET_FILENAME_COMPONENT(out_fn ${in_f} NAME_WE)
    SET(out_f "${CMAKE_CURRENT_BINARY_DIR}/${out_fn}.o")
    GET_FILENAME_COMPONENT(out_dir ${out_f} DIRECTORY)
    ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}
      COMMAND ${CMAKE_LINKER} -r -b binary -o ${out_f} ${in_f}
      DEPENDS ${in_f}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Building resource ${out_f}"
      VERBATIM
      )
    LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

add_resources(bk_resources ${res_files})

## Configuration options for this app
# Display name (under bubble in LiveArea)
set(VITA_APP_NAME "Bookr MOD Vita")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "PATH00027")

# bash "-c" "echo -n hello | sed 's/hello/world/;'"
execute_process(COMMAND git describe
                COMMAND sed -e "s,v,0," -e "s,\\(.*\\)\\.,\\1," -e "s,-.*,,g"
                OUTPUT_VARIABLE VITA_VERSION)
string(REGEX REPLACE "\n$" "" VITA_VERSION "${VITA_VERSION}")

# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION "${VITA_VERSION}")

# Optional. You can specify more param.sfo flags this way.
set(VITA_MKSFOEX_FLAGS "${VITA_MKSFOEX_FLAGS} -d PARENTAL_LEVEL=1")


# Huge 62MB with lots of submodules
# TODO: this makes it under ext/mupdf/src/mupdf_lib; which is annoying
# TODO: add to ext/CMakeLists.txt
# TODO: add to vitasdk/packages
#       similar to http://www.linuxfromscratch.org/blfs/view/cvs/pst/mupdf.html
ExternalProject_Add(mupdf_lib
  PREFIX "${CMAKE_SOURCE_DIR}/ext/mupdf-lib"
  GIT_REPOSITORY https://github.com/pathway27/mupdf
  GIT_TAG origin/vg-console
  GIT_PROGRESS 1
  #--Configure step-------------
  # SOURCE_SUBDIR "${CMAKE_SOURCE_DIR}/ext/mupdf"
  CONFIGURE_COMMAND make generate
  UPDATE_COMMAND ""
  #--Build step-----------------
  BUILD_COMMAND make OS=vita build=release prefix=/usr/local/vitasdk/arm-vita-eabi HAVE_X11=no HAVE_GLUT=no
  BUILD_IN_SOURCE 1
  #--Install step---------------
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(mupdf_lib SOURCE_DIR)

# Add any additional include paths here
include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/ext/psp2shell/psp2shell_m/include
  ${CMAKE_SOURCE_DIR}/ext/psp2shell/common
  $ENV{VITASDK}/arm-vita-eabi/include/freetype2
  "${SOURCE_DIR}/include"
)


# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  "${SOURCE_DIR}/build/vita/release"
)


## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  ${COMMON_SRCS}
  ${bk_resources}
  src/document.cpp
  src/bookmark.cpp
  src/filetypes/fancytext.cpp
  src/filetypes/plaintext.cpp

  data/fonts/res_txtfont.c
  data/fonts/res_uifont.c
  
  src/graphics/texture.cpp

  src/graphics/screen_vita.cpp
  src/layer_vita.cpp

  src/filetypes/mudocument.cpp
  src/graphics/font_vita.cpp
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
#-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
#mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm
target_link_libraries(bookr-mod-vita
  vita2d
  mupdf
  mupdf-third
  c
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
  tinyxml2
)

## Create Vita files
vita_create_self(eboot.bin bookr-mod-vita
#  UNSAFE
)

vita_create_vpk(bookr-mod-vita.vpk ${VITA_TITLEID} eboot.bin
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE data/logos/icon0.png sce_sys/icon0.png
  FILE data/logos/bg.png sce_sys/livearea/contents/bg.png
  FILE data/logos/startup.png sce_sys/livearea/contents/startup.png
  FILE data/sce_sys/livearea/contents/template.xml sce_sys/livearea/contents/template.xml
)

add_dependencies(bookr-mod-vita mupdf_lib)

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
