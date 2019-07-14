# TODO: needed Externals switch-freetype


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
  src/layer_vita.cpp

  src/graphics/shader.cpp
  src/graphics/texture2d.cpp
  src/graphics/sprite_renderer.cpp
  src/graphics/text_renderer.cpp
  src/resource_manager.cpp
  
  src/filetypes/mudocument.cpp
)

set(OPENGL_opengl_LIBRARY EGL glapi drm_nouveau)
add_definitions(-DUSE_GLAD=1)

# Library to link to (drop the -l prefix). This will mostly be stubs.
#-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
#mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm
target_link_libraries(bookr-modern
  EGL
  glapi
  glad
  drm_nouveau
  mupdf
  mupdf-third
  z
  nx
  m
  tinyxml2
)

switch_create_nro(bookr-modern bookr-modern)