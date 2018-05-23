# TODO: needed Externals switch-freetype

# Add any additional include paths here
include_directories(
  ${CMAKE_BINARY_DIR}
  $ENV{DEVKITPRO}/portlibs/switch/include
  $ENV{DEVKITPRO}/portlibs/switch/include/freetype2
  ${CMAKE_SOURCE_DIR}/ext/mupdf/src/mupdf_lib/include
  "${SOURCE_DIR}/include"
)

add_executable(bookr-modern
  ${COMMON_SRCS}
  ${bk_resources}
)

# Library to link to (drop the -l prefix). This will mostly be stubs.
#-lpsp2shell -lSceSysmodule_stub -lSceNet_stub \ -lSceNetCtl_stub -lSceKernel_stub -lScePower_stub -lSceAppMgr_stub
#mupdf -ldjvulibre -lraster -lworld -lfonts -lstream -lbase -lm
target_link_libraries(bookr-modern
  mupdf
  mupdfthird
)