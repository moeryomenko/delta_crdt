cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(delta_crdt CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# need for clangd.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(STATUS "Using conan")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
endif()

add_subdirectory(include)

include(CTest)
enable_testing()
if (CRDT_BUILD_TESTS)
	add_subdirectory(tests)
endif()
