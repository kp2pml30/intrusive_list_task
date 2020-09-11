cmake_minimum_required(VERSION 3.15)

find_package(Threads)

add_library(gtest
    gtest.h
    gtest-all.cc)

set_property(TARGET gtest PROPERTY CXX_STANDARD 17)
target_link_libraries(gtest Threads::Threads)
