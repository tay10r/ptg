cmake_minimum_required(VERSION 3.14.7)

project(ptg)

option(PTG_EXAMPLE "Whether or not to build the example program." OFF)

include(FetchContent)

FetchContent_Declare(glm URL "${CMAKE_CURRENT_SOURCE_DIR}/deps/glm/glm-0.9.9.8.zip")
FetchContent_MakeAvailable(glm)

if(PTG_EXAMPLE)
endif(PTG_EXAMPLE)

set(glad_sources
  src/glad/include/glad/glad.h
  src/glad/include/KHR/khrplatform.h
  src/glad/src/glad.c)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${glad_sources})

set(cpu_kernels
  src/kernels/raise_kernel.hpp
  src/kernels/raise_kernel.cpp
  src/kernels/render_kernel.hpp
  src/kernels/render_kernel.cpp)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/src" FILES ${cpu_kernels})

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
  src/output.cpp
  src/render.hpp
  src/render.cpp
  src/kernel.hpp
  src/kernel_registry.hpp
  src/kernel_registry.cpp
  src/cpu_kernel.hpp
  src/cpu_kernel.cpp
  ${cpu_kernels}
  ${glad_sources})

target_include_directories(ptg PUBLIC include PRIVATE src/glad/include)

target_link_libraries(ptg PRIVATE glm)

target_compile_features(ptg PRIVATE cxx_std_17)

add_executable(ptg_example
  example/main.cpp
  example/stb_image_write.h
  example/stb_image_write.c)

target_link_libraries(ptg_example PRIVATE ptg)

set_target_properties(ptg_example PROPERTIES OUTPUT_NAME example)
