#include <iostream>
#include "GlfwWindow.h"

int main() {
    // 创建 GlfwWindow 实例（构造函数会初始化 GLFW 并创建窗口）
    MainWindow window("LearnOpenGL", 1440, 810);
    window.Run();
    return 0;
}
