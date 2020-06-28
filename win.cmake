message("WIN CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR}")

# Ugh not working, compile this with visual studio
#include_external_msproject(mupdf ${CMAKE_SOURCE_DIR}/ext/mupdf/platform/win32/mupdf.vcxproj PLATFORM Win32)

# TODO: add_subdirectory and CMAKE_BUILD_TYPE makes an extra folder where exe is
# Cmake doesnt move dlls to binary folder so we have to do it here or using install
ExternalProject_Add(glew
  PREFIX       "${CMAKE_BINARY_DIR}/ext/glew"
  DOWNLOAD_DIR "${CMAKE_BINARY_DIR}/ext"

  URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0-win32.zip
  URL_MD5 32a72e6b43367db8dbea6010cd095355
  #--Configure step-------------
  CONFIGURE_COMMAND ""
  #--Build step-----------------
  BUILD_COMMAND ""
  #--Install step---------------
  UPDATE_COMMAND "" # Skip annoying updates for every build
  INSTALL_COMMAND ""
    #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/ext/glew/src/glew/lib/Release/Win32/glew32.lib ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
    #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/ext/freetype/win32/freetype.dll ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/ext/tinyxml2/Debug/tinyxml2.dll ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
    #COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/ext/mupdf/platform/win32/Debug/libthirdparty.lib ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
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

# ExternalProject_Add(mupdf
#  URL https://mupdf.com/downloads/archive/mupdf-1.17.0-windows.zip
#  URL_HASH SHA1=32644f7b793f003278cd96d8127587f66c2da54f
#  #--Configure step-------------
#  SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/mupdf-windows"
#  CONFIGURE_COMMAND ""
#  #--Build step-----------------
#  BUILD_COMMAND ""
#  #--Install step---------------
#  UPDATE_COMMAND "" # Skip annoying updates for every build
#  INSTALL_COMMAND ""
# )

#ExternalProject_Add(freetype2
#  URL https://github.com/ubawurinna/freetype-windows-binaries/archive/v2.6.5.zip
#  URL_MD5 ab9e0e387b93daec8c486e5432cb8362
#  #--Configure step-------------
#  SOURCE_DIR "${CMAKE_SOURCE_DIR}/ext/freetype"
#  CONFIGURE_COMMAND ""
#  #--Build step-----------------
#  BUILD_COMMAND ""
#  #--Install step---------------
#  UPDATE_COMMAND "" # Skip annoying updates for every build
#  INSTALL_COMMAND ""
#)

include_directories(
  ${CMAKE_BINARY_DIR}
  "${CMAKE_SOURCE_DIR}/src"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  "${CMAKE_SOURCE_DIR}/ext/glfw/include"
  "${CMAKE_SOURCE_DIR}/ext/glew/include"
  "${CMAKE_SOURCE_DIR}/ext/glm"
  "${CMAKE_SOURCE_DIR}/ext/freetype/include"
  "${CMAKE_SOURCE_DIR}/ext/freetype/include/freetype2"
  "${CMAKE_SOURCE_DIR}/ext/glad/include"
  "${CMAKE_SOURCE_DIR}/ext/stb"
)

link_directories(
  ${CMAKE_CURRENT_BINARY_DIR}
  "${CMAKE_CURRENT_BINARY_DIR}/ext/tinyxml2/Debug"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/libs"
  "${CMAKE_SOURCE_DIR}/ext/glew/lib/Release/Win32"
  "${CMAKE_SOURCE_DIR}/ext/glfw/lib-vc2015"
  "${CMAKE_SOURCE_DIR}/ext/freetype/win32"
  "${CMAKE_SOURCE_DIR}/ext/psp2shell/win32"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/platform/win32/compiled_lib/Debug"
)

## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  ${COMMON_SRCS}
  src/graphics/shader.cpp
  src/graphics/screen_glfw.cpp
  src/layer.cpp

  src/graphics/shader.cpp
  src/resource_manager.cpp
  src/graphics/texture2d.cpp
  src/graphics/sprite_renderer.cpp
  src/graphics/text_renderer.cpp
  #src/filetypes/mudocument.cpp
  "${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c"
)


target_link_libraries(bookr-mod-vita
  OpenGL32
  glew32s
  glew32
  glfw3
  #freetype6.dll
  tinyxml2
  #SOIL
  libmupdf
)

# freetype2
add_dependencies(bookr-mod-vita glew glm glfw tinyxml2)

# $<TARGET_FILE_DIR:bookr-mod-vita>/shaders
add_custom_command (
  TARGET bookr-mod-vita POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/src/graphics/shaders ${CMAKE_BINARY_DIR}/Debug/shaders
  DEPENDS ${CMAKE_BINARY_DIR}/Debug
  COMMENT "Copying shaders" VERBATIM
)

add_custom_command (
  TARGET bookr-mod-vita POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/Debug/data
  DEPENDS ${CMAKE_BINARY_DIR}/Debug
  COMMENT "Copying data" VERBATIM
)
