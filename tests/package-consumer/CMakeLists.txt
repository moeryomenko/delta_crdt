cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(delta_crdt_consumer LANGUAGES CXX)

find_package(delta_crdt CONFIG REQUIRED)

add_executable(package-consumer main.cpp)
target_link_libraries(package-consumer PRIVATE delta_crdt::delta_crdt)
