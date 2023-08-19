cmake_minimum_required(VERSION 3.25)
project(SimpleJsonParser)

set(CMAKE_CXX_STANDARD 17)

include_directories(json parser)

add_executable(SimpleJsonParser
        json/json.cpp
        json/json.h
        main.cpp
        parser/parser.h
        parser/parser.cpp)
