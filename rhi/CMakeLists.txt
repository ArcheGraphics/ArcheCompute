#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

cmake_minimum_required(VERSION 3.16)

project(rhi LANGUAGES C CXX)

set(COMMON_FILES
        device.h
        stream.h
        buffer.h
        buffer.cpp
        kernel.h
        argument.h
        command.h
)

set(EXTENSION_FILES
        extension/device_extension.h
        extension/debug_capture_ext.h
)

set(PROJECT_FILES
        ${COMMON_FILES}
        ${EXTENSION_FILES}
)

add_library(${PROJECT_NAME} STATIC ${PROJECT_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ../)

target_link_libraries(${PROJECT_NAME} PUBLIC
        fmt::fmt
        spdlog::spdlog
)