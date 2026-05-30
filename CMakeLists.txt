cmake_minimum_required(VERSION 4.3)
project(LearnOpenGL2)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif(MSVC)


set(GLFW_LIBRARY_TYPE STATIC)

add_subdirectory("00.3rd/glad")
add_subdirectory("00.3rd/glfw")

add_subdirectory("01.1.Triangle")
add_subdirectory("01.2.Triangle-color-singlebuffer")
add_subdirectory("01.3.Triangle-color-interleavebuffer")
add_subdirectory("01.4.Triangle-multi-singlebuffer")
add_subdirectory("01.5.Triangle-drawmode")
add_subdirectory("01.6.Rect")
add_subdirectory("01.6.Rect-ebo")
