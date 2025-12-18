cmake_minimum_required(VERSION 3.10)

project(Theme6_v1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

include_directories(
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
)

add_executable(Theme6_v1 main.cpp)

target_link_libraries(Theme6_v1
        ${SDL2_LIBRARIES}
        SDL2_ttf
)
