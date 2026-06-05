project("01.4.Triangle-multi-singlebuffer" LANGUAGES CXX)

file(GLOB SOURCES "src/*.cpp")
file(GLOB HEADERS "src/*.h")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC glad glfw)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
    COMMENT "Syncing assets to output directory"
)

if(MSVC)
    file(GLOB_RECURSE ASSETS_FILES "${CMAKE_CURRENT_SOURCE_DIR}/assets/*")
    target_sources(${PROJECT_NAME} PUBLIC ${ASSETS_FILES})
    source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/assets/"
                 PREFIX "assets"
                 FILES ${ASSETS_FILES})
endif()
