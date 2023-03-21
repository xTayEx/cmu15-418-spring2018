cmake_minimum_required(VERSION 3.16)
project(assit1_prog1)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE INCLUDE_FILES ../common/CycleTimer.h)
file(GLOB_RECURSE SRC_FILES mandelbrot.cpp ../common/*.cpp)

add_executable(mandelbrot main.cpp ${SRC_FILES} ${INCLUDE_FILES})
target_include_directories(mandelbrot PUBLIC ../common/)
target_link_libraries(mandelbrot pthread)

