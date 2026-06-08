cmake_minimum_required(VERSION 4.3)
project(LearnOpenGL2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif(MSVC)


set(GLFW_LIBRARY_TYPE STATIC)

set(STB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/00.3rd/stb")

add_subdirectory("00.3rd/glad")
add_subdirectory("00.3rd/glfw")

add_subdirectory("00.utils")

add_subdirectory("01.01.Triangle")
add_subdirectory("01.02.Triangle-color-singlebuffer")
add_subdirectory("01.03.Triangle-color-interleavebuffer")
add_subdirectory("01.04.Triangle-multi-singlebuffer")
add_subdirectory("01.05.Triangle-drawmode")
add_subdirectory("01.06.Rect")
add_subdirectory("01.07.Rect-ebo")
add_subdirectory("01.08.Rect-use-utils")
add_subdirectory("01.09.Rect-glsl")
add_subdirectory("01.10.Rect-uniform")
add_subdirectory("01.11.Texture")
