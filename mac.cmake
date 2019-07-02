find_package(PkgConfig REQUIRED)

list(APPEND CMAKE_PREFIX_PATH /usr/local)

find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm REQUIRED)
pkg_search_module(GLFW REQUIRED glfw3)

include_directories(
  "${FREETYPE_INCLUDE_DIRS}"
  "${GLFW_INCLUDE_DIRS}"
  "${GLM_INCLUDE_DIRS}"
  # "${GLFW3_INCLUDE_DIRS}"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  "${CMAKE_SOURCE_DIR}/ext/SOIL/src"
)

link_directories(
  
)

## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  ${COMMON_SRCS}
  # src/graphics/shaders/shader.cpp
  src/graphics/screen_glfw.cpp
)


target_link_libraries(bookr-mod-vita
  tinyxml2
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLM_LIBRARIES}
  ${GLFW_LIBRARIES}
)

# add_dependencies(bookr-mod-vita glew glm freetype)
