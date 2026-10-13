cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
project(delta_crdt VERSION 0.1.0 LANGUAGES CXX)

option(CRDT_BUILD_TESTS "Build delta_crdt tests" OFF)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

add_library(delta_crdt INTERFACE)
add_library(delta_crdt::delta_crdt ALIAS delta_crdt)
target_compile_features(delta_crdt INTERFACE cxx_std_20)
target_include_directories(
    delta_crdt
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

configure_package_config_file(
    cmake/delta_crdt-config.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/delta_crdt-config.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/delta_crdt
)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/delta_crdt-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    TARGETS delta_crdt
    EXPORT delta_crdtTargets
)
install(
    DIRECTORY include/delta_crdt
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
install(
    EXPORT delta_crdtTargets
    FILE delta_crdtTargets.cmake
    NAMESPACE delta_crdt::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/delta_crdt
)
install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/delta_crdt-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/delta_crdt-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/delta_crdt
)

if(CRDT_BUILD_TESTS)
    enable_testing()
    find_package(GTest CONFIG QUIET)
    if(NOT TARGET GTest::gtest_main)
        include(FetchContent)
        FetchContent_Declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        )
        set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
        FetchContent_MakeAvailable(googletest)
    endif()

    add_subdirectory(tests)
endif()
