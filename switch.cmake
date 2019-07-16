# TODO: needed Externals switch-freetype

FUNCTION(ADD_RESOURCES out_var)
  SET(result)
  FOREACH(in_f ${ARGN})
    GET_FILENAME_COMPONENT(out_fn_e ${in_f} NAME)
    GET_FILENAME_COMPONENT(out_fn ${in_f} NAME_WE)
    SET(out_f "${CMAKE_CURRENT_BINARY_DIR}/${out_fn_e}.o")
    GET_FILENAME_COMPONENT(out_dir ${out_f} DIRECTORY)
    ADD_CUSTOM_COMMAND(OUTPUT ${out_f}
      COMMAND ${CMAKE_COMMAND} -E make_directory ${out_dir}

      COMMAND ${BIN2S_TOOL} ${in_f} | ${CMAKE_AS} -o ${out_f}
      COMMAND cd ${CMAKE_BINARY_DIR} && ${CMAKE_SOURCE_DIR}/make_switch_resources.sh ${out_fn_e}

      DEPENDS ${in_f}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      COMMENT "Building resource ${out_fn_e}.o and ${out_fn_e}.h"
      VERBATIM 
      )
    LIST(APPEND result ${out_f})
  ENDFOREACH()
  SET(${out_var} "${result}" PARENT_SCOPE)
ENDFUNCTION()

add_resources(bk_resources ${res_files})

# Add any additional include paths here
include_directories(
  ${CMAKE_BINARY_DIR}
  $ENV{DEVKITPRO}/libnx/include
  $ENV{DEVKITPRO}/portlibs/switch/include
  $ENV{DEVKITPRO}/portlibs/switch/include/freetype2
  ${CMAKE_SOURCE_DIR}/ext/mupdf/include
  ${CMAKE_SOURCE_DIR}/ext/tinyxml2
  ${CMAKE_SOURCE_DIR}/ext/stb
  "${SOURCE_DIR}/include"
)

# Add any additional library paths here
# ${CMAKE_CURRENT_BINARY_DIR} lets you use any library currently being built
link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_SOURCE_DIR}/ext/mupdf/build/switch/release
)

add_executable(bookr-modern
  ${COMMON_SRCS}
  ${bk_resources}
  data/fonts/res_txtfont.c
  data/fonts/res_uifont.c

  src/graphics/screen_switch.cpp

  src/graphics/font_vita.cpp
  src/layer.cpp

  src/graphics/shader.cpp
  src/graphics/texture2d.cpp
  src/graphics/sprite_renderer.cpp
  src/graphics/text_renderer.cpp
  src/resource_manager.cpp
  
  src/filetypes/mudocument.cpp
)

set(OPENGL_opengl_LIBRARY EGL glapi drm_nouveau)
add_definitions(-DUSE_GLAD=1)
add_definitions(-DENABLE_NXLINK)

# Library to link to (drop the -l prefix). This will mostly be stubs.
#-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
#mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm
target_link_libraries(bookr-modern
  glad
  EGL
  glapi
  drm_nouveau
  nx
  mupdf
  mupdf-third
  z
  m
  tinyxml2
)

switch_create_nro(bookr-modern bookr-modern)