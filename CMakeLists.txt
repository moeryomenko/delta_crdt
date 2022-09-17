cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(delta_crdt CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# need for clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(STATUS "Using conan")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

# setup ccache if available.
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
	set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
	set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

add_library(delta_crdt INTERFACE)
target_include_directories(
  delta_crdt INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_include_directories(
  delta_crdt SYSTEM INTERFACE $<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>)

# Setup package config
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/delta_crdt)

set(DELTA_CRDT_CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P})
set(CMAKE_SIZEOF_VOID_P "")
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/cmake/delta_crdt-config-version.cmake
  VERSION 0.0.1
  COMPATIBILITY SameMajorVersion)
set(CMAKE_SIZEOF_VOID_P ${DELTA_CRDT_CMAKE_SIZEOF_VOID_P})

# Install target
install(DIRECTORY include/${PROJECT_NAME}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if(CRDT_BUILD_TESTS)
  find_package(ut REQUIRED)

  include(CTest)
  enable_testing()

  add_subdirectory(tests)
endif()
