list(APPEND CMAKE_PREFIX_PATH /usr/local)

find_package(OpenGL REQUIRED)
find_package(freetype REQUIRED)
find_package(glew REQUIRED)
find_package(glm REQUIRED)
find_package(glfw3 REQUIRED)

include_directories(
  "${FREETYPE_INCLUDE_DIRS}"
  "${GLM_INCLUDE_DIRS}"
  "${CMAKE_SOURCE_DIR}/ext/mupdf/include"
  "${CMAKE_SOURCE_DIR}/ext/glad/include"
  "${CMAKE_SOURCE_DIR}/ext/stb"
)

link_directories(

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
  tinyxml2
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
     ${CMAKE_SOURCE_DIR}/data/fonts $<TARGET_FILE_DIR:bookr-mod-vita>/fonts
  COMMENT "Copying fonts" VERBATIM
)

# add_dependencies(bookr-mod-vita glew glm freetype)
