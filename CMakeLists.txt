cmake_minimum_required(VERSION 3.2)
project(cpp_reflection_example)

add_executable(cpp_reflection_example 
    main.cpp
)

set_property(TARGET cpp_reflection_example PROPERTY CXX_STANDARD 20)