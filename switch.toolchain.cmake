# Code based on vita.toolchain.cmake

cmake_minimum_required(VERSION 2.4)
set(CMAKE_SYSTEM_NAME Generic)

if(NOT DEFINED ENV{DEVKITPRO})
  message(FATAL_ERROR "Please set DEVKITPRO in your environment")
else()
  set(DEVKITPRO "$ENV{DEVKITPRO}")
endif()

set( TOOL_OS_SUFFIX "" )
if( CMAKE_HOST_WIN32 )
 set( TOOL_OS_SUFFIX ".exe" )
endif()

set(CMAKE_SYSTEM_PROCESSOR armv8-a)
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

# cache flags
set( CMAKE_CXX_FLAGS           ""                        CACHE STRING "c++ flags" )
set( CMAKE_C_FLAGS             ""                        CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS_RELEASE   "-O3 -DNDEBUG"            CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-O3 -DNDEBUG"            CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set( CMAKE_SHARED_LINKER_FLAGS ""                        CACHE STRING "shared linker flags" )
set( CMAKE_MODULE_LINKER_FLAGS ""                        CACHE STRING "module linker flags" )
set( CMAKE_EXE_LINKER_FLAGS    "-Wl,-z,nocopyreloc"      CACHE STRING "executable linker flags" )

# we require the relocation table
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wl,-q" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-q" )

# set these global flags for cmake client scripts to change behavior
set( SWITCH True )

# where is the target environment
set( CMAKE_FIND_ROOT_PATH "${DEVKITPRO}/bin" "${DEVKITPRO}/devkitA64/aarch64-none-elf" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )
set( CMAKE_INSTALL_PREFIX "${DEVKITPRO}/devkitA64/aarch64-none-elf" CACHE PATH "default install path" )

# only search for libraries and includes in dkp toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )