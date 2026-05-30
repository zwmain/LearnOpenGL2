#include "GlfwWindow.h"
#include <format>
#include <iostream>
#include <vector>

namespace {

// GLFW 回调：当窗口大小发生变化时，调用用户对象的成员函数处理
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* obj = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    obj->OnFrambufferSize(width, height);
}

// GLFW 回调：当键盘事件发生时，调用用户对象的成员函数处理
void KeyCallBack(GLFWwindow* window, int key, int scanmode, int action, int mods)
{
    auto* obj = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    obj->OnKey(key, scanmode, action, mods);
}

// 将 0-255 的整数颜色值归一化到 0.0-1.0 之间，方便 glClearColor 使用
float ToNormalizeColor(int v)
{
    v = std::max(std::min(v, 255), 0);
    return static_cast<float>(v) / 255.0f;
}

} // namespace

MainWindow::MainWindow(const char* title, int width, int height)
{
    // 初始化 GLFW 库
    glfwInit();

    // 告诉 GLFW 使用 OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口并设置当前 OpenGL 上下文
    window_ = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwMakeContextCurrent(window_);

    // 将 this 绑定到 GLFW 窗口，回调中可以访问当前对象
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FrameBufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallBack);
}

MainWindow::~MainWindow()
{
    // 关闭 GLFW 并释放其内部资源
    glfwTerminate();
    window_ = nullptr;
}

void MainWindow::Run()
{
    // 通过 glad 加载 OpenGL 函数指针
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "glad load gl fail" << std::endl;
        return;
    }

    // 获取当前窗口大小，并设置视口
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window_, &width, &height);
    glViewport(0, 0, width, height);

    // 设置屏幕背景颜色：深蓝/灰色调
    glClearColor(ToNormalizeColor(0x35), ToNormalizeColor(0x5c), ToNormalizeColor(0x7d), 1);

    // 准备顶点数据和着色器
    PrepareData();
    PrepareShader();

    // 主循环：处理事件、清屏、渲染、交换缓冲区
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT);
        Render();
        glfwSwapBuffers(window_);
    }
}

void MainWindow::OnFrambufferSize(int width, int height)
{
    // 当窗口大小变化时更新 OpenGL 视口
    glViewport(0, 0, width, height);
}

void MainWindow::OnKey(int key, int scanmode, int action, int mods)
{
    // 目前没有实现按键处理，可以在此处添加输入逻辑
}

void MainWindow::Render()
{
    // 启用当前着色器程序并绑定 VAO，然后绘制三角形
    glUseProgram(shaderProgramIds_[0]);
    glBindVertexArray(vaoIds_[0]);
    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void MainWindow::PrepareData()
{
    // 定义一个简单三角形的三个顶点位置
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, // 左下角顶点
        0.5f, -0.5f, 0.0f,  // 右下角顶点
        0.0f, 0.5f, 0.0f    // 顶部顶点
    };
    unsigned int VBO, VAO;

    // 创建 VBO（顶点缓冲对象）并上传顶点数据到 GPU
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // 创建 VAO（顶点数组对象）并记录顶点属性配置
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 告诉 OpenGL 顶点数据的布局：位置属性在 location 0，3 个 float，紧密排列
    glEnableVertexAttribArray(0); // 激活0号顶点属性
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 解绑 VBO 和 VAO，使状态不会意外影响后续操作
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // 保存 VAO ID，后续 Render() 中使用
    vaoIds_.push_back(VAO);
}

void MainWindow::PrepareShader() {
    // 顶点着色器源码：把输入顶点位置传给裁剪空间位置
    const char* vertexShaderSource = "#version 460 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "\n"
                                     "void main()\n"
                                     "{\n"
                                     "    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\n";
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // 片段着色器源码：将所有片段填充为固定颜色
    const char* fragmentShaderSource = "#version 460 core\n"
                                       "out vec4 FragColor;\n"
                                       "\n"
                                       "void main()\n"
                                       "{\n"
                                       "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                       "}\n";
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // 创建着色器程序并链接顶点/片段着色器
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // 保存程序 ID，后续 Render() 使用它来绘制
    shaderProgramIds_.push_back(shaderProgram);

    // 着色器对象不再需要，程序已链接，释放它们
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
