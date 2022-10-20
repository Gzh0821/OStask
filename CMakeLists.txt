cmake_minimum_required(VERSION 3.23)
project(OStask)

set(CMAKE_CXX_STANDARD 17)

add_executable(OStask main.cpp cmdline.h OStask.h OStask.cpp)
