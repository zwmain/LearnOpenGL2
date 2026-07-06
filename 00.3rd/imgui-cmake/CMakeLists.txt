project(imgui LANGUAGES CXX)

# 设置 imgui 源码目录（指向现有的 imgui）
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../imgui")

# 核心文件
set(IMGUI_CORE_FILES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/imgui_demo.cpp
)

# OpenGL 后端实现
set(IMGUI_IMPL_FILES
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# 合并所有源文件
set(IMGUI_SRC
    ${IMGUI_CORE_FILES}
    ${IMGUI_IMPL_FILES}
)

# 创建库
add_library(imgui STATIC ${IMGUI_SRC})

# 包含头文件目录
target_include_directories(imgui PUBLIC
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw glad)

set_target_properties(imgui PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

if(MSVC)
    target_compile_options(imgui PRIVATE "/source-charset:utf-8")
endif()


