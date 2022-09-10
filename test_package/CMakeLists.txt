cmake_minimum_required(VERSION 3.20)
project(PackageTest CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(example example.cc)
target_link_libraries(example ${CONAN_LIBS})
