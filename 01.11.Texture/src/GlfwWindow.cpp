#include "GlfwWindow.h"
#include <filesystem>
#include <format>
#include <iostream>
#include <vector>
#include <Color.h>

namespace fs = std::filesystem;

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
    Color clearColor(0x35, 0x5c, 0x7d);
    glClearColor(clearColor.R(), clearColor.G(), clearColor.B(), clearColor.A());

    // 先准备着色器，再准备顶点数据
    PrepareShader();
    PrepareData();

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
    shaderPrograms_[0].UseProgram();
    glBindVertexArray(vaoIds_[0]);
    // 这里不再是 glDrawArrays，而是 glDrawElements，因为我们使用了索引缓冲对象
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);
}

void MainWindow::PrepareData()
{
    // 定义一个简单三角形的三个顶点位置
    std::vector<float> vertices = {
        -0.5f, -0.5f, 0.0f, // 左下
        0.5f, -0.5f, 0.0f, // 右下
        0.0f, 0.5f, 0.0f, // 上
    };
    Color c1(210, 79, 75);
    Color c2(138, 210, 75);
    Color c3(75, 206, 210);
    std::vector<float> colors = {
        c1.R(), c1.G(), c1.B(), // 第一个顶点rgb
        c2.R(), c2.G(), c2.B(), // 第二个顶点rgb
        c3.R(), c3.G(), c3.B(), // 第三个顶点rgb
    };
    std::vector<unsigned int> eleIndex = {
        0, 1, 2,
    };
    unsigned int vaoId = 0;
    // 创建 VAO（顶点数组对象）并记录顶点属性配置
    glGenVertexArrays(1, &vaoId);
    // 由于vao描述了整个图形，后面的多个vbo都由vao描述，因此整个过程vao一直保持激活状态
    glBindVertexArray(vaoId);

    unsigned int ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo); // 绑定ebo的时候，vao一定要处于绑定状态，vao会记录ebo的绑定状态
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleIndex.size() * sizeof(float), eleIndex.data(), GL_STATIC_DRAW);

    unsigned int vertexVboId = 0;
    // 创建 VBO（顶点缓冲对象）并上传顶点数据到 GPU
    glGenBuffers(1, &vertexVboId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑顶点vbo，为颜色vbo做准备，vao仍然保持激活状态

    unsigned int colorVboId = 0;
    // 创建另一个 VBO 用于颜色数据，并上传到 GPU
    glGenBuffers(1, &colorVboId);
    glBindBuffer(GL_ARRAY_BUFFER, colorVboId);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 解绑颜色vbo，为后续添加描述做准备，vao仍然保持激活状态

    // 此时没有任何激活的vbo，因此后面每次添加描述的时候都要绑定对应的vbo

    // 动态获取location
    int location = shaderPrograms_[0].GetAttrLocation("aPos");
    // 告诉 OpenGL 顶点数据的布局：位置属性在 location 0，3 个 float，紧密排列
    glBindBuffer(GL_ARRAY_BUFFER, vertexVboId); // 绑定顶点vbo，描述才会生效
    glEnableVertexAttribArray(location); // 启用属性 location 0，顶点数据将会放置到0位置
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 顶点vbo已经描述完毕，解绑顶点vbo，为颜色vbo做准备，vao仍然保持激活状态

    // 动态获取location
    location = shaderPrograms_[0].GetAttrLocation("aColor");
    // 告诉 OpenGL 颜色数据的布局：颜色属性在 location 1，3 个 float，紧密排列
    glBindBuffer(GL_ARRAY_BUFFER, colorVboId); // 绑定颜色vbo，描述才会生效
    glEnableVertexAttribArray(location); // 启用属性 location 1，颜色数据将会放置到1位置
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0); // 颜色vbo已经描述完毕，解绑颜色vbo，vao仍然保持激活状态

    // vbo以全部描述完毕，vao可以解绑了；解绑VAO，使状态不会意外影响后续操作
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // 解绑ebo，一定要在vao解绑之后解绑ebo，否则vao会记录ebo的解绑状态，导致后续渲染无法使用ebo

    // 保存 VAO ID，后续 Render() 中使用
    vaoIds_.push_back(vaoId);
}

void MainWindow::PrepareShader()
{
    // 获取当前执行目录（当前工作目录）
    std::string currentDir = fs::current_path().string();

    // 使用绝对路径（相对于 exe 所在目录的 assets 文件夹）
    std::string vertexPath = currentDir + R"(/assets/shader/vertex/vertex.glsl)";
    std::string fragmentPath = currentDir + R"(/assets/shader/fragment/fragment.glsl)";

    Shader shader(vertexPath, fragmentPath);
    shaderPrograms_.push_back(shader);
}
