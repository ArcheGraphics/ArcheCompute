#  Copyright (c) 2024 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

if (NOT DEFINED CMAKE_C_COMPILER_LAUNCHER AND NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
    find_program(CCACHE_FOUND ccache)
    find_program(SCCACHE_FOUND sccache)
    if (CCACHE_FOUND AND NOT SCCACHE_FOUND)
        message("setting CCACHE to ${CCACHE_FOUND}")
        set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_FOUND})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_FOUND})
    elseif (SCCACHE_FOUND AND NOT CCACHE_FOUND)
        message("setting CCACHE to ${CCACHE_FOUND}")
        set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE_FOUND})
        set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE_FOUND})
    endif (CCACHE_FOUND AND NOT SCCACHE_FOUND)
endif ()

cmake_minimum_required(VERSION 3.16)

# globally add VKB_DEBUG for the debug build
add_compile_definitions($<$<CONFIG:DEBUG>:METAL_DEBUG>)

project(ArcheCompute)

# Add path for local cmake scripts
list(APPEND CMAKE_MODULE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(global_options)
include(check_atomic)
include(build_metallib)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(MLX_METAL_PATH ${CMAKE_CURRENT_BINARY_DIR}/kernels/)

# Add third party libraries
add_subdirectory(third_party)

add_subdirectory(common)
add_subdirectory(runtime)
add_subdirectory(cpptests)
add_subdirectory(benchmark)

add_subdirectory(python)
add_subdirectory(apps)