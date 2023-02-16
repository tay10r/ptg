cmake_minimum_required(VERSION 3.14.7)

project(ptg)

add_library(ptg
  include/ptg.h
  src/ptg.cpp
  src/model.hpp
  src/model.cpp
  src/texture.hpp
  src/device.hpp
  src/cpu_device.hpp
  src/cpu_device.cpp
  src/output.hpp
  src/output.cpp)

target_include_directories(ptg PUBLIC include)

add_executable(ptg_example
  example/main.cpp
  example/stb_image_write.h
  example/stb_image_write.c)

target_link_libraries(ptg_example PRIVATE ptg)

set_target_properties(ptg_example PROPERTIES OUTPUT_NAME example)
