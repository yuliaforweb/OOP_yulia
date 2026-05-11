cmake_minimum_required(VERSION 4.0)
project(hotel)

set(CMAKE_CXX_STANDARD 20)

add_executable(hotel main.cpp
        person.h
        guest.h
        guest.cpp
        room.h
        rooms.h
        rooms.cpp
        booking.h
        booking.cpp
        hotel.h
        hotel.cpp)
