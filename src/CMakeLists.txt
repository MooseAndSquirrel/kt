cmake_minimum_required(VERSION 3.16)

project(kt-libraries)

add_subdirectory(string)
add_subdirectory(gfx)

add_library(kt-terminal
  terminal.cpp
  )
add_library(kt-options
  options.cpp
  )
