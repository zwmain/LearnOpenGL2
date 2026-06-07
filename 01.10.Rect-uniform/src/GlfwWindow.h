#pragma once

#include <glad/gl.h> // glad must be included before GLFW
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <Shader.h>

// MainWindow 封装了 GLFW 窗口创建、输入处理和基本 OpenGL 渲染流程
class MainWindow {
public:
    // 构造函数：创建 GLFW 窗口并初始化上下文
    MainWindow(const char* title, int width, int height);
    // 析构函数：释放 GLFW 资源
    ~MainWindow();

    // 运行主循环，加载 OpenGL 函数，设置视口并进行渲染
    void Run();

    // GLFW 回调触发时调用：处理窗口大小变化
    void OnFrambufferSize(int width, int height);
    // GLFW 回调触发时调用：处理键盘输入事件
    void OnKey(int key, int scanmode, int action, int mods);

private:
    // 渲染当前帧内容
    void Render();
    // 创建并上传顶点数据到 GPU
    void PrepareData();
    // 创建、编译并链接顶点/片段着色器
    void PrepareShader();

private:
    GLFWwindow* window_ = nullptr;                    // GLFW 窗口对象指针
    std::vector<unsigned int> vaoIds_;                // 存储创建的 VAO ID
    std::vector<Shader> shaderPrograms_;              // 存储创建的 Shader Program

    float timeVal_ = 0.0f; // 用于传递给着色器的时间变量
};
