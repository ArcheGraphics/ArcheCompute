#  Copyright (c) 2024 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

# create sample app project
project(benchmark LANGUAGES C CXX)

find_package(benchmark CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

set(SRC
        data_type_util.h
        data_type_util.cpp
        main.cpp
        mad_throughput.cpp
        reduce.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} PRIVATE common runtime metal-cpp benchmark::benchmark benchmark::benchmark_main GTest::gtest)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../)
