project(imgui LANGUAGES CXX)

# 设置 imgui 源码目录（指向现有的 imgui）
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../00.3rd/imgui")


# 只编译必要的 imgui 源文件（GLFW 后端）
file(GLOB IMGUI_CORE_FILES
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
)

# 添加 OpenGL 3 后端（自动渲染）
file(GLOB IMGUI_IMPL_FILES
    "${IMGUI_DIR}/backends/imgui_impl_*.{cpp,h}"
)

# 过滤掉不需要编译的后端文件
file(REMOVE IMGUI_IMPL_FILES
    "${IMGUI_DIR}/backends/imgui_impl_android.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_sdl2.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_sdl3.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_dx*.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_win32.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_vulkan.{cpp,h}"
    "${IMGUI_DIR}/backends/imgui_impl_wgpu.{cpp,h}"
)

# 过滤掉 glut 后端（除非使用 GL/freeglut）
file(REMOVE IMGUI_IMPL_FILES
    "${IMGUI_DIR}/backends/imgui_impl_glut.{cpp,h}"
)

# 过滤掉 allegro5 后端
file(REMOVE IMGUI_IMPL_FILES
    "${IMGUI_DIR}/backends/imgui_impl_allegro5.{cpp,h}"
)


set(IMGUI_SRC
    ${IMGUI_CORE_FILES}
    ${IMGUI_IMPL_FILES}
)

add_library(imgui STATIC ${IMGUI_SRC})

target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

# imgui 是纯 GUI 库，不需要链接 glad/glfw（这些由使用方自己处理）

set_target_properties(imgui PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    target_compile_options(imgui PRIVATE "/source-charset:utf-8")
endif()
