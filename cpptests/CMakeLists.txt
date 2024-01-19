#  Copyright (c) 2023 Feng Yang
#
#  I am making my contributions/submissions to this project solely in my
#  personal capacity and am not conveying any rights to any intellectual
#  property of any third parties.

# create sample app project
project(cpp-tests LANGUAGES C CXX)

set(SRC
        main.cpp
        test_metal.cpp
)

add_executable(${PROJECT_NAME} ${SRC})

enable_testing()

find_package(GTest CONFIG REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR} .
        ../)

target_link_libraries(${PROJECT_NAME} PRIVATE common GTest::gtest GTest::gtest_main GTest::gmock GTest::gmock_main)
target_link_libraries(${PROJECT_NAME} PRIVATE metal metal-cpp)
