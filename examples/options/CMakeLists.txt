cmake_minimum_required(VERSION 3.16)

project(kt-options-example)

include_directories($ENV{HOME}/kt)

add_executable(kt-options-demo
  kt-options-demo.cpp
  )
target_link_libraries(kt-options-demo kt-options kt-terminal)
