# CMake configuration for glad OpenGL loader as a static library
project(glad VERSION 1.0 LANGUAGES C)

# Build as static library
add_library(glad STATIC src/gl.c)

# Include directories
target_include_directories(glad PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Optional: Enable warnings for MSVC and GCC/Clang
if(MSVC)
    target_compile_options(glad PRIVATE /W4)
elseif(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(glad PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Install targets (optional, for package distribution)
install(DIRECTORY include/ DESTINATION include FILES_MATCHING PATTERN "*.h")
install(TARGETS glad EXPORT gladTargets
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

# Create export file for find_package support
install(EXPORT gladTargets
    NAMESPACE glad::
    DESTINATION lib/cmake/glad
    FILE gladTargets.cmake
)
