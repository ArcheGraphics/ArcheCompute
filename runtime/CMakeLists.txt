#  Copyright (c) 2024 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(runtime LANGUAGES C CXX)

set(PRIMITIVES_FILES
        primitives/reduce.h
        primitives/reduce.cpp
)

set(COMMON_FILES
        types/half_types.h
        types/spatial.h
        types/spatial.cpp
        dtypes.h
        dtypes.cpp
        metal.h
        metal.cpp
        device.h
        device.cpp
        stream.h
        stream.cpp
        stream_objc.mm
        allocator.h
        allocator.cpp
        array.h
        array.cpp
        kernel.h
        kernel.cpp
        counter.h
        counter.cpp
        utils.h
        utils.cpp
)

set(EXTENSION_FILES
        extension/debug_capture_ext.h
        extension/debug_capture_ext.cpp
)

set(PROJECT_FILES
        ${COMMON_FILES}
        ${EXTENSION_FILES}
        ${PRIMITIVES_FILES}
)

add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        common
        metal-cpp
)

set(KERNEL_FIELS
        ${CMAKE_CURRENT_SOURCE_DIR}/../shader/builtin/mad_throughput.metal
        ${CMAKE_CURRENT_SOURCE_DIR}/../shader/builtin/reduce.metal
)

build_metallib(
        TARGET metal_kernel_metallib
        TITLE metal_kernel
        SOURCES ${KERNEL_FIELS}
        INCLUDE_DIRS ${PROJECT_SOURCE_DIR} ${MLX_INCLUDE_DIRS}
        OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_dependencies(
        ${PROJECT_NAME}
        metal_kernel_metallib
)

set(MLX_METAL_PATH ${CMAKE_CURRENT_BINARY_DIR}/)
target_compile_definitions(
        ${PROJECT_NAME} PRIVATE METAL_PATH="${MLX_METAL_PATH}/metal_kernel.metallib")

# python install
install(
        FILES ${MLX_METAL_PATH}/metal_kernel.metallib
        DESTINATION arche_compute
        COMPONENT metallib
)