cmake_minimum_required(VERSION 3.20)

project(TestProjGL
    VERSION 1.0.0 
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(OpenGL-Core)
add_subdirectory(OpenGL-Examples)