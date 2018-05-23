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
