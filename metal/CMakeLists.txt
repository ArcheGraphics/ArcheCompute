#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(metal LANGUAGES C CXX)

set(COMMON_FILES
        metal_device.h
        metal_device.cpp
        metal_stream.h
        metal_stream.cpp
        metal_stream_objc.mm
        metal_buffer.h
        metal_buffer.cpp
        metal_kernel.h
        metal_kernel.cpp
        metal_command_encoder.h
        metal_command_encoder.cpp
        metal_callback_context.h
        metal_callback_context.cpp
        metal_counter.h
        metal_counter.cpp
)

set(EXTENSION_FILES
        extension/metal_debug_capture_ext.h
        extension/metal_debug_capture_ext.cpp
)

set(PROJECT_FILES
        ${COMMON_FILES}
        ${EXTENSION_FILES}
)

add_library(${PROJECT_NAME} OBJECT ${PROJECT_FILES})
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)

# Link third party libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
        common
        rhi
        metal-cpp
)

set(KERNEL_FIELS
        ${CMAKE_CURRENT_SOURCE_DIR}/../shader/builtin/mad_throughput.metal
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