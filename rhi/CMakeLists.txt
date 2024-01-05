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
        kernel.h
        argument.h
)

set(EXTENSION_FILES
        extension/device_extension.h
        extension/debug_capture_ext.h
)

set(PROJECT_FILES
        ${COMMON_FILES}
        ${EXTENSION_FILES}
)

add_library(${PROJECT_NAME} INTERFACE ${PROJECT_FILES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ../)