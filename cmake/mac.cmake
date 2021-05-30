list(APPEND CMAKE_PREFIX_PATH /usr/local)

find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

ExternalProject_Add(mupdf_lib
  PREFIX "${CMAKE_SOURCE_DIR}/ext/mupdf-lib"
  GIT_REPOSITORY https://github.com/pathway27/mupdf
  GIT_TAG origin/1.18.0-vg-console
  GIT_PROGRESS 1
  #--Configure step-------------
  # SOURCE_SUBDIR "${CMAKE_SOURCE_DIR}/ext/mupdf"
  CONFIGURE_COMMAND make generate
  UPDATE_COMMAND ""
  #--Build step-----------------
  BUILD_COMMAND make build=release HAVE_X11=no HAVE_GLUT=no
  BUILD_IN_SOURCE 1
  #--Install step---------------
  INSTALL_COMMAND ""
)
ExternalProject_Get_Property(mupdf_lib SOURCE_DIR)

include_directories(
  "${FREETYPE_INCLUDE_DIRS}"
  "${GLM_INCLUDE_DIRS}"
  # "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  "${CMAKE_SOURCE_DIR}/ext/mupdf-lib/src/mupdf_lib/include"
  "${CMAKE_SOURCE_DIR}/ext/glad/include"
  "${CMAKE_SOURCE_DIR}/ext/stb"
)

link_directories(
  # "${CMAKE_SOURCE_DIR}/ext/mupdf/build/release"
  "${CMAKE_SOURCE_DIR}/ext/mupdf-lib/src/mupdf_lib/build/release"
)

## Build and link
# Add all the files needed to compile here
add_executable(bookr-mod-vita
  ${COMMON_SRCS}
  src/graphics/shader.cpp
  src/layer.cpp
  
  src/graphics/screen_glfw.cpp
  src/graphics/font.cpp
  src/resource_manager.cpp

  src/graphics/texture2d.cpp
  src/graphics/sprite_renderer.cpp
  src/graphics/text_renderer.cpp
  "${CMAKE_SOURCE_DIR}/ext/glad/src/glad.c"
)


target_link_libraries(bookr-mod-vita
  mupdf
  mupdf-third
  tinyxml2_static
  ${OPENGL_LIBRARIES}
  ${FREETYPE_LIBRARIES}
  ${GLEW_LIBRARIES}
  ${GLM_LIBRARIES}
  glfw
  "-framework Foundation"
)

# ??? embed into binary?
add_custom_command(
  TARGET bookr-mod-vita POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory              
     ${CMAKE_SOURCE_DIR}/src/graphics/shaders $<TARGET_FILE_DIR:bookr-mod-vita>/shaders
  COMMENT "Copying shaders" VERBATIM
)

add_custom_command(
  TARGET bookr-mod-vita POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory              
     ${CMAKE_SOURCE_DIR}/data $<TARGET_FILE_DIR:bookr-mod-vita>/data
  COMMENT "Copying data" VERBATIM
)

# add_dependencies(bookr-mod-vita glew glm freetype)
