list(APPEND CMAKE_PREFIX_PATH /usr/local)

find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)
find_library(SOIL SOIL ${CMAKE_SOURCE_DIR}/ext/SOIL/lib)

include_directories(
  "${FREETYPE_INCLUDE_DIRS}"
  "${GLM_INCLUDE_DIRS}"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  "${CMAKE_SOURCE_DIR}/ext/SOIL/src"
  "${CMAKE_SOURCE_DIR}/ext/glad/include"
)

link_directories(
  "${CMAKE_SOURCE_DIR}/ext/SOIL/lib"
)

## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  ${COMMON_SRCS}
  src/graphics/shaders/shader.cpp
  src/layer.cpp
  
  src/graphics/screen_glfw.cpp
  src/graphics/font.cpp
  "${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c"
)


target_link_libraries(bookr-mod-vita
  tinyxml2
  ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLM_LIBRARIES}
  glfw
  ${SOIL}
  "-framework Foundation"
)

# add_dependencies(bookr-mod-vita glew glm freetype)
