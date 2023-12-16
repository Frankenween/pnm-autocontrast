cmake_minimum_required(VERSION 3.20)
project(hw5)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-fopenmp -O3")
add_executable(contrast_fix main.cpp PNMImage.cpp PNMImage.h Colour.cpp Colour.h)
