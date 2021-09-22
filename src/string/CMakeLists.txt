cmake_minimum_required(VERSION 3.16)

project(kt_string)

enable_testing       ()
add_executable       (test_kt_string test_kt_string_concat.cpp)
target_link_libraries(test_kt_string gtest_main)
