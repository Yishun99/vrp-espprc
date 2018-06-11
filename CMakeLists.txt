cmake_minimum_required(VERSION 2.8)
project(vrp_espprc)

set(CMAKE_CXX_STANDARD 11)

add_executable(vrp_espprc
        espprc.cpp
        espprc.h
        graph.cpp
        graph.h
        main.cpp)
