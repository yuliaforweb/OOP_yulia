cmake_minimum_required(VERSION 4.1)
project(sqrt)

set(CMAKE_CXX_STANDARD 20)

add_executable(sqrt main.cpp
        root.cpp
        root.h
        print_name.cpp
        print_age.cpp
)
