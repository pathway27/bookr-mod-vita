# Code based on vita.toolchain.cmake and switch.cmake from:
# https://github.com/devkitPro/pacman-packages/blob/master/pkgbuild-scripts/switch.cmake

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

set(SWITCH_LIBNX TRUE)


if(NOT DEFINED ENV{DEVKITPRO})
  message(FATAL_ERROR "Please set DEVKITPRO in your environment")
else()
  set(DEVKITPRO "$ENV{DEVKITPRO}")
endif()

add_definitions(-D__SWITCH__ -Wfatal-errors)
include_directories(SYSTEM "$ENV{DEVKITPRO}/libnx/include")
include_directories("${PROJECT_SOURCE_DIR}/extern/switch")

set( TOOL_OS_SUFFIX "" )
if( CMAKE_HOST_WIN32 )
 set( TOOL_OS_SUFFIX ".exe" )
endif()

set(CMAKE_C_COMPILER     "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "C compiler" )
set(CMAKE_CXX_COMPILER   "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-g++${TOOL_OS_SUFFIX}"     CACHE PATH "C++ compiler" )
set(CMAKE_ASM_COMPILER   "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "assembler" )
set(CMAKE_STRIP          "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" )
set(CMAKE_AR             "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" )
set(CMAKE_LINKER         "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" )
set(CMAKE_NM             "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" )
set(CMAKE_OBJCOPY        "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" )
set(CMAKE_OBJDUMP        "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" )
set(CMAKE_RANLIB         "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" )
set(PKG_CONFIG_EXECUTABLE "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-pkg-config${TOOL_OS_SUFFIX}"  CACHE PATH "pkg-config")

# cache flags
set(CMAKE_CXX_FLAGS "-MP -MD -Wall -O2 -ffunction-sections -march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE" CACHE STRING "" FORCE)
set(CMAKE_C_FLAGS   "")
set( CMAKE_CXX_FLAGS_RELEASE   "-O3 -DNDEBUG"            CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-O3 -DNDEBUG"            CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set(CMAKE_EXE_LINKER_FLAGS_INIT "-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE -ftls-model=local-exec -L/opt/devkitpro/portlibs/switch/lib -L/opt/devkitpro/libnx/lib -specs=/opt/devkitpro/libnx/switch.specs")
set(CMAKE_MODULE_LINKER_FLAGS_INIT "${CMAKE_EXE_LINKER_FLAGS_INIT}")

# set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
# set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# SET(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Shared libs not available" )

# set(CMAKE_FIND_ROOT_PATH /opt/devkitpro/portlibs/switch/)
# set(CMAKE_PREFIX_PATH "/opt/devkitpro/portlibs/switch/")

# set these global flags for cmake client scripts to change behavior
set( SWITCH True )

# where is the target environment
# set( CMAKE_FIND_ROOT_PATH "${DEVKITPRO}/bin" "${DEVKITPRO}/devkitA64/aarch64-none-elf" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )
# set( CMAKE_INSTALL_PREFIX "${DEVKITPRO}/devkitA64/aarch64-none-elf" CACHE PATH "default install path" )

# only search for libraries and includes in dkp toolchain
# set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
# set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )

#TODO: linking creates an elf but no extension for that...?

macro(switch_create_nro source)
  #set(VITA_ELF_CREATE_FLAGS "${VITA_ELF_CREATE_FLAGS}" CACHE STRING "vita-elf-create flags")
  #set(VITA_MAKE_FSELF_FLAGS "${VITA_MAKE_FSELF_FLAGS}" CACHE STRING "vita-make-fself flags")

  #set(oneValueArgs CONFIG)
  #cmake_parse_arguments(vita_create_self "${options}" "${oneValueArgs}" "" ${ARGN})

  # LIST
  add_custom_command(OUTPUT ${source}.lst
    COMMAND aarch64-none-elf-gcc-nm -CSn ${source} > ${source}.lst
    DEPENDS ${source}
    COMMENT "Creating list from ELF ${source}" VERBATIM
  )
  # NACP
  add_custom_command(OUTPUT ${source}.nacp
    COMMAND nacptool --create "APP_TITLE" "SREE" "1.0.0" ${source}.nacp
    DEPENDS ${source}
    COMMENT "Creating NACP ${source}"
  )
  # NRO
  add_custom_command(OUTPUT ${source}.nro
    COMMAND elf2nro ${source} ${source}.nro --icon=/opt/devkitpro/libnx/default_icon.jpg --nacp=${source}.nacp
    DEPENDS ${source}.nacp
    COMMENT "Creating NRO ${source}.nro"
  )
  #add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${target}.nro
  #  COMMAND ${VITA_MAKE_FSELF} ${VITA_MAKE_FSELF_FLAGS} ${source}.velf ${target}
  #  DEPENDS ${source}
  #)
  add_custom_target(NRO ALL DEPENDS ${source}.lst ${source}.nacp ${source}.nro)
endmacro(switch_create_nro)
############################
