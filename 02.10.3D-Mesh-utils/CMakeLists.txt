project("02.10.3D-Mesh-utils" LANGUAGES CXX)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw utils glm-header-only)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} PUBLIC "${STB_DIR}")

# 复制到二进制文件目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Syncing assets to output directory"
)

# 复制到二进制文件目录的上层目录，实际为执行目录
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/../assets
    COMMENT "Syncing assets to output directory"
)

if(MSVC)
    file(GLOB_RECURSE ASSETS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
    target_sources(${PROJECT_NAME} PUBLIC ${ASSETS_FILES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
                 PREFIX "assets"
                 FILES ${ASSETS_FILES})
endif()
