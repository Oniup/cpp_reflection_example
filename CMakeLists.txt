cmake_minimum_required(VERSION 3.2)
project(reflection_example)

add_executable(reflection_example 
    main.cpp
)

set_property(TARGET reflection_example PROPERTY CXX_STANDARD 20)