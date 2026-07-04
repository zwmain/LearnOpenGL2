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
add_subdirectory("00.3rd/glm")

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
add_subdirectory("01.12.Texture-rect")
add_subdirectory("01.13.Texture-wrap")
add_subdirectory("01.14.Texture-dynamic")
add_subdirectory("01.15.Texture-dynamic-utils")
add_subdirectory("01.16.Texture-mix")

add_subdirectory("02.01.3D-transform")
add_subdirectory("02.02.3D-rect")
add_subdirectory("02.03.3D-box")
add_subdirectory("02.04.3D-multi-box")
add_subdirectory("02.05.3D-camera")
add_subdirectory("02.06.3D-camera-input")
add_subdirectory("02.07.3D-camera-euler")
add_subdirectory("02.08.3D-camera-utils")
add_subdirectory("02.09.3D-camera-quat")
add_subdirectory("02.10.3D-Mesh-utils")
add_subdirectory("02.11.3D-Scene")
