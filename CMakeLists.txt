cmake_minimum_required(VERSION 3.15...3.30)

project(meng
    VERSION 1.1.0
    LANGUAGES C CXX ASM
    DESCRIPTION "A modern, lightweight, cross-platform coroutine library"
)

# Standard C++ setting
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)
option(MENG_BUILD_TESTS "Build test suite" ON)
option(MENG_BUILD_BENCHMARKS "Build benchmark suite" ON)
option(MENG_ENABLE_PROFILER "Enable gperftools profiler in legacy test" OFF)

# Platform / Architecture Assembly Selection
if(WIN32)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(MENG_ASM_SRC src/win64/swapcontext.asm)
    else()
        set(MENG_ASM_SRC src/win32/swapcontext.asm)
    endif()
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)")
    set(MENG_ASM_SRC src/arm64/swapcontext.s)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(x86_64|amd64)")
    set(MENG_ASM_SRC src/linux64/swapcontext.s)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

add_library(meng
    src/meng.cpp
    ${MENG_ASM_SRC}
)
add_library(meng::meng ALIAS meng)

include(GNUInstallDirs)

target_include_directories(meng
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(meng PUBLIC cxx_std_11)

if(MSVC)
    target_compile_options(meng PRIVATE /W4)
else()
    target_compile_options(meng PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Installation & CMake Package Configuration
include(CMakePackageConfigHelpers)

install(TARGETS meng
    EXPORT mengTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES
    include/meng.h
    include/meng.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT mengTargets
    FILE mengTargets.cmake
    NAMESPACE meng::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/meng
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/mengConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mengConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/mengConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/meng
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/mengConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/mengConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/meng
)

# Tests & Benchmarks
if(MENG_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
