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
set(ARCH "-march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIE")

set(CMAKE_C_COMPILER     "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc${TOOL_OS_SUFFIX}"     CACHE PATH "C compiler" )
set(CMAKE_CXX_COMPILER   "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-g++${TOOL_OS_SUFFIX}"     CACHE PATH "C++ compiler" )
set(CMAKE_ASM_COMPILER   "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-as${TOOL_OS_SUFFIX}"     CACHE PATH "assembler" )
set(CMAKE_STRIP          "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-strip${TOOL_OS_SUFFIX}"   CACHE PATH "strip" )
set(CMAKE_AR             "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc-ar${TOOL_OS_SUFFIX}"      CACHE PATH "archive" )
set(CMAKE_LINKER         "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-ld${TOOL_OS_SUFFIX}"      CACHE PATH "linker" )
set(CMAKE_NM             "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc-nm${TOOL_OS_SUFFIX}"      CACHE PATH "nm" )
set(CMAKE_OBJCOPY        "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-objcopy${TOOL_OS_SUFFIX}" CACHE PATH "objcopy" )
set(CMAKE_OBJDUMP        "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-objdump${TOOL_OS_SUFFIX}" CACHE PATH "objdump" )
set(CMAKE_RANLIB         "${DEVKITPRO}/devkitA64/bin/aarch64-none-elf-gcc-ranlib${TOOL_OS_SUFFIX}"  CACHE PATH "ranlib" )

# Switch specific tools
set(SWITCH_BIN2S         "${DEVKITPRO}/tools/bin/bin2s${TOOL_OS_SUFFIX}"            CACHE PATH "switch-bin2s")
set(SWITCH_BMP2BIN       "${DEVKITPRO}/tools/bin/bmp2bin${TOOL_OS_SUFFIX}"          CACHE PATH "switch-bmp2bin")
set(SWITCH_BUILD_PFS0    "${DEVKITPRO}/tools/bin/build_pfs0${TOOL_OS_SUFFIX}"     CACHE PATH "switch-build-pfs0")
set(SWITCH_BUILD_ROMFS   "${DEVKITPRO}/tools/bin/build_romfs${TOOL_OS_SUFFIX}"    CACHE PATH "switch-build-romfs")
set(SWITCH_ELF2KIP       "${DEVKITPRO}/tools/bin/elf2kip${TOOL_OS_SUFFIX}"        CACHE PATH "switch-elf2kip")
set(SWITCH_ELF2NRO       "${DEVKITPRO}/tools/bin/elf2nro${TOOL_OS_SUFFIX}"        CACHE PATH "switch-elf2nro")
set(SWITCH_ELF2NSO       "${DEVKITPRO}/tools/bin/elf2nso${TOOL_OS_SUFFIX}"        CACHE PATH "switch-elf2nso")
set(SWITCH_NACPTOOL      "${DEVKITPRO}/tools/bin/nacptool${TOOL_OS_SUFFIX}"       CACHE PATH "switch-nacptool")
set(SWITCH_NPDMTOOL      "${DEVKITPRO}/tools/bin/npdmtool${TOOL_OS_SUFFIX}"       CACHE PATH "switch-npdmtool")
set(SWITCH_NXLINK        "${DEVKITPRO}/tools/bin/nxlink${TOOL_OS_SUFFIX}"         CACHE PATH "switch-nxlink")
set(SWITCH_PADBIN        "${DEVKITPRO}/tools/bin/padbin${TOOL_OS_SUFFIX}"         CACHE PATH "switch-padbin")
set(SWITCH_RAW2C         "${DEVKITPRO}/tools/bin/raw2c${TOOL_OS_SUFFIX}"          CACHE PATH "switch-raw2c")

# cache flags
set( CMAKE_C_FLAGS             "${ARCH} -DSWITCH"      CACHE STRING "c flags" )
set( CMAKE_CXX_FLAGS           "${ARCH} -fno-rtti -std=gnu++11 -DSWITCH"                        CACHE STRING "c++ flags" )
set( CMAKE_CXX_FLAGS_RELEASE   "-O3 -DNDEBUG"            CACHE STRING "c++ Release flags" )
set( CMAKE_C_FLAGS_RELEASE     "-O3 -DNDEBUG"            CACHE STRING "c Release flags" )
set( CMAKE_CXX_FLAGS_DEBUG     "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c++ Debug flags" )
set( CMAKE_C_FLAGS_DEBUG       "-O0 -g -DDEBUG -D_DEBUG" CACHE STRING "c Debug flags" )
set( CMAKE_EXE_LINKER_FLAGS    "-specs=${DEVKITPRO}/libnx/switch.specs"      CACHE STRING "executable linker flags" )


# set these global flags for cmake client scripts to change behavior
set( SWITCH True )

# where is the target environment
set( CMAKE_FIND_ROOT_PATH "${DEVKITPRO}/bin" "${DEVKITPRO}/devkitA64/aarch64-none-elf" "${CMAKE_INSTALL_PREFIX}" "${CMAKE_INSTALL_PREFIX}/share" )
set( CMAKE_INSTALL_PREFIX "${DEVKITPRO}/devkitA64/aarch64-none-elf" CACHE PATH "default install path" )

# only search for libraries and includes in dkp toolchain
set( CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY )
set( CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY )


macro(switch_create_pfs0 target source)
  add_custom_target(${source}.nso ALL
    COMMAND ${SWITCH_ELF2NSO} ${source} ${source}.nso
    DEPENDS ${source}
    COMMENT "Building ${source}.nso"
  )

  add_custom_target(create-exefs ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory exefs
  )

  add_custom_target(exefs-main
    COMMAND ${CMAKE_COMMAND} -E copy
    "${source}.nso"
    "${CMAKE_BINARY_DIR}/exefs/main"
  )

  add_custom_target(${target} ALL
    COMMAND ${SWITCH_BUILD_PFS0} exefs ${target}
    DEPENDS ${source}.nso
    COMMENT "Building ${target}"
  )
  add_dependencies(${target} create-exefs exefs-main)
endmacro(switch_create_pfs0)

macro(switch_create_nacp target)
  add_custom_target(${target} ALL
    COMMAND ${SWITCH_NACPTOOL} --create ${SWITCH_APP_TITLE} ${SWITCH_AUTHOR} ${SWITCH_VERSION} ${target}
    COMMENT "Building ${target}"
  )
endmacro(switch_create_nacp)

macro(switch_create_nro target source)
  add_custom_target(${target} ALL
    COMMAND ${SWITCH_ELF2NRO} ${source} ${target}
    DEPENDS ${source}
    COMMENT "Building ${target}.nro"
  )
  add_dependencies(${target} ${source})
endmacro(switch_create_nro)