cmake_minimum_required(VERSION 3.25)
project(SimpleJsonParser)

set(CMAKE_CXX_STANDARD 17)

include_directories(json)

add_executable(SimpleJsonParser
        json/json.cpp
        json/json.h
        main.cpp)
