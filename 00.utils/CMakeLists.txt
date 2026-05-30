cmake_minimum_required(VERSION 3.18)
project(utils LANGUAGES CXX)

if(MSVC)
    add_compile_options("/source-charset:utf-8")
endif()

# 使用 glob 获取所有头文件和源文件
file(GLOB SOURCE_FILES "src/*.cpp")
file(GLOB HEADER_FILES "include/*.h")

add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC glad)
