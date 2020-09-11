cmake_minimum_required(VERSION 3.15)

project(intrusive_list)
include_directories(.)
add_subdirectory(gtest)

add_executable(intrusive_list_testing
    intrusive_list.cpp
    intrusive_list.h
    main.cpp
    test_utils.h)

set_property(TARGET intrusive_list_testing PROPERTY CXX_STANDARD 17)

target_link_libraries(intrusive_list_testing gtest)
