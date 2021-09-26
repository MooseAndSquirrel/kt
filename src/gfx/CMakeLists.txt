cmake_minimum_required(VERSION 3.16)

project(kt-gfx)

find_package(SDL2 REQUIRED)
add_library(kt-gfx
  libkt-gfx.cpp
  color.cpp
  texture.cpp
  renderer.cpp
  view.cpp
  ui.cpp
  )

