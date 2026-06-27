#include "GlfwWindow.h"
#include <Color.h>
#include <filesystem>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>

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

void MouseCallBack(GLFWwindow* window, double xpos, double ypos)
{
    auto* obj = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    obj->OnMouseMove(xpos, ypos);
}

void MouseScrollCallBack(GLFWwindow* window, double xoffset, double yoffset)
{
    auto* obj = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    obj->OnMouseScroll(xoffset, yoffset);
}

} // namespace

MainWindow::MainWindow(const char* title, int width, int height)
    : width_(width)
    , height_(height)
{
    // 初始化 GLFW 库
    glfwInit();

    // 告诉 GLFW 使用 OpenGL 4.6 Core Profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // 创建窗口并设置当前 OpenGL 上下文
    window_ = glfwCreateWindow(width_, height_, title, nullptr, nullptr);
    glfwMakeContextCurrent(window_);

    // 将 this 绑定到 GLFW 窗口，回调中可以访问当前对象
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FrameBufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallBack);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window_, MouseCallBack);
    glfwSetScrollCallback(window_, MouseScrollCallBack);

    // 注册默认的 Ctrl+P 快捷键处理器（在初始化阶段注册）
    RegisterKeyHandler(GLFW_KEY_P, GLFW_MOD_CONTROL,
        std::bind(&MainWindow::OnCtrlP, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    RegisterKeyHandler(GLFW_KEY_R, GLFW_MOD_CONTROL,
        std::bind(&MainWindow::OnCtrlR, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    RegisterKeyHandler(GLFW_KEY_W, 0,
        std::bind(&MainWindow::OnW, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    RegisterKeyHandler(GLFW_KEY_A, 0,
        std::bind(&MainWindow::OnA, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    RegisterKeyHandler(GLFW_KEY_S, 0,
        std::bind(&MainWindow::OnS, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    RegisterKeyHandler(GLFW_KEY_D, 0,
        std::bind(&MainWindow::OnD, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    RegisterKeyHandler(GLFW_KEY_ESCAPE, 0,
        std::bind(&MainWindow::OnEscape, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
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
    glEnable(GL_DEPTH_TEST); // 必须在 gladLoadGL 之后调用，否则会报错

    // 获取当前窗口大小，并设置视口
    glViewport(0, 0, width_, height_);

    // 设置屏幕背景颜色：深蓝/灰色调
    Color clearColor(0x35, 0x5c, 0x7d);
    glClearColor(clearColor.R(), clearColor.G(), clearColor.B(), clearColor.A());

    // 先准备着色器，再准备顶点数据
    PrepareShader();
    PrepareData();
    PrepareTexture();

    // 主循环：处理事件、清屏、渲染、交换缓冲区
    while (!glfwWindowShouldClose(window_)) {
        glfwPollEvents();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render();
        glfwSwapBuffers(window_);
    }
}

void MainWindow::OnFrambufferSize(int width, int height)
{
    // 当窗口大小变化时更新 OpenGL 视口
    width_ = width;
    height_ = height;
    glViewport(0, 0, width_, height_);
}

void MainWindow::OnKey(int key, int scanmode, int action, int mods)
{
    // 构造 KeyCombo 并分发给注册的处理器
    KeyCombo combo { key, mods };
    auto it = handlers_.find(combo);
    if (it != handlers_.end()) {
        auto handler = it->second;
        if (handler) {
            handler(key, action, mods);
        }
    }
}

void MainWindow::OnMouseMove(double xpos, double ypos)
{
    if (firstMouse_) {
        lastX_ = xpos;
        lastY_ = ypos;
        firstMouse_ = false;
    }
    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    lastX_ = xpos;
    lastY_ = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw_ += xoffset;
    pitch_ += yoffset;

    if (pitch_ > 85.0f) {
        pitch_ = 85.0f;
    }
    if (pitch_ < -85.0f) {
        pitch_ = -85.0f;
    }

    glm::vec3 front = cameraFront_;
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    cameraFront_ = glm::normalize(front);
}

void MainWindow::OnMouseScroll(double xoffset, double yoffset)
{
    fov_ -= static_cast<float>(yoffset);
    if (fov_ <= 1.0f) {
        fov_ = 1.0f;
    }
    if (fov_ >= 50.0f) {
        fov_ = 50.0f;
    }
}

void MainWindow::RegisterKeyHandler(int key, int mods, KeyHandler handler)
{
    KeyCombo combo { key, mods };
    handlers_.emplace(combo, std::move(handler));
}

void MainWindow::Render()
{
    float curFrameTime = static_cast<float>(glfwGetTime());
    deltaTime_ = curFrameTime - lastFrameTime_;
    lastFrameTime_ = curFrameTime;
    cameraSpeed_ = 2.5f * deltaTime_;
    // 启用当前着色器程序并绑定 VAO，然后绘制三角形
    shaderPrograms_[0].UseProgram();
    shaderPrograms_[0].SetUniform1i("sampler", 0);

    // 创建基础模型矩阵，把模型绕x/z轴旋转-45/45度，放到世界坐标中
    glm::mat4 baseModel = glm::mat4(1.0f);
    baseModel = glm::rotate(baseModel, glm::radians(-45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    baseModel = glm::rotate(baseModel, glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // 通过相机观察图形
    float radius = 10.0f;
    float camX = sin(glfwGetTime()) * radius;
    float camZ = cos(glfwGetTime()) * radius;
    glm::mat4 view = glm::lookAt(cameraPos_, cameraPos_ + cameraFront_, cameraUp_);

    // 创建投影矩阵
    glm::mat4 projection = glm::mat4(1.0f);
    if (isPerspective_) {
        // 使用透视投影，近裁剪面距离为0.1，远裁剪面距离为100
        projection = glm::perspective(glm::radians(fov_), static_cast<float>(width_) / height_, 0.1f, 100.0f);
    } else {
        float orthoSize = 5.0f;
        float aspect = width_ / static_cast<float>(height_); // 计算窗口宽高比
        float halfWidth = orthoSize * aspect; // X 轴范围按宽高比扩展

        projection = glm::ortho(
            -halfWidth, halfWidth, // X: [-50*aspect, 50*aspect]
            -orthoSize, orthoSize, // Y: 保持 [-50, 50]
            0.1f, 100.0f);
    }

    // 将视图和投影矩阵传入顶点着色器
    shaderPrograms_[0].SetUniformMat4f("view", glm::value_ptr(view));
    shaderPrograms_[0].SetUniformMat4f("projection", glm::value_ptr(projection));

    glBindVertexArray(vaoIds_[0]);

    std::vector <glm::vec3> cubePositions = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(2.0f, 5.0f, -7.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f, 3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f, 2.0f, -2.5f),
        glm::vec3(1.5f, 0.2f, -1.5f),
        glm::vec3(-1.3f, 1.0f, -1.5f)
    };
    for (int i = 0; i < cubePositions.size(); ++i) {
        glm::mat4 model = baseModel;
        if (i > 0) {
            // 除第一次之外，其它9次均叠加一个平移和旋转
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(i * 10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }

        shaderPrograms_[0].SetUniformMat4f("model", glm::value_ptr(model));
        // 这里不再是 glDrawArrays，而是 glDrawElements，因为我们使用了索引缓冲对象
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, nullptr);
    }
}

void MainWindow::PrepareData()
{
    // 准备立方体的数据：每个面4个顶点（24个顶点），每面独立UV
    // 立方体中心在原点，边长为1.0（范围[-0.5, 0.5]）
    std::vector<float> vertices = {
        // Front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        // Back face
         0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        // Left face
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        // Right face
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
        // Top face
        -0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        // Bottom face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f
    };

    // 每个面独立的 UV（相同的四个角）
    std::vector<float> uvs = {
        // Front
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Back
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Left
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Right
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Top
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        // Bottom
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };

    // 索引（每个面两个三角形，共12个三角形，36个索引）
    std::vector<unsigned int> eleIndex;
    eleIndex.reserve(36);
    for (unsigned int face = 0; face < 6; ++face) {
        unsigned int base = face * 4;
        eleIndex.push_back(base + 0);
        eleIndex.push_back(base + 1);
        eleIndex.push_back(base + 2);
        eleIndex.push_back(base + 2);
        eleIndex.push_back(base + 3);
        eleIndex.push_back(base + 0);
    }

    unsigned int vaoId = 0;
    // 创建 VAO
    glGenVertexArrays(1, &vaoId);
    glBindVertexArray(vaoId);

    unsigned int ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleIndex.size() * sizeof(unsigned int), eleIndex.data(), GL_STATIC_DRAW);

    unsigned int vertexVboId = 0;
    glGenBuffers(1, &vertexVboId);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVboId);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned int uvVboId = 0;
    glGenBuffers(1, &uvVboId);
    glBindBuffer(GL_ARRAY_BUFFER, uvVboId);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(float), uvs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 设置顶点属性
    int location = shaderPrograms_[0].GetAttrLocation("aPos");
    glBindBuffer(GL_ARRAY_BUFFER, vertexVboId);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    location = shaderPrograms_[0].GetAttrLocation("aUv");
    glBindBuffer(GL_ARRAY_BUFFER, uvVboId);
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // 解绑 VAO（注意：在解绑 VAO 之后不要解绑 ELEMENT_ARRAY_BUFFER，否则 VAO 不会保存 EBO）
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

void MainWindow::PrepareTexture()
{
    texture_.LoadFromFile("assets/image/gugugaga.jpeg", 0);
}

void MainWindow::OnCtrlP(int key, int action, int mods)
{
    if (action == GLFW_PRESS) {
        isPerspective_ = !isPerspective_;
        std::cout << "Projection mode switched to: " << (isPerspective_ ? "Perspective" : "Orthographic") << std::endl;
    }
}

void MainWindow::OnCtrlR(int key, int action, int mods)
{
    cameraPos_ = { 0.0f, 0.0f, 3.0f };
    cameraFront_ = { 0.0f, 0.0f, -1.0f };
    cameraUp_ = { 0.0f, 1.0f, 0.0f };
}

void MainWindow::OnW(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        cameraPos_ += cameraSpeed_ * cameraFront_;
    }
}

void MainWindow::OnA(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        cameraPos_ -= glm::normalize(glm::cross(cameraFront_, cameraUp_)) * cameraSpeed_;
    }
}

void MainWindow::OnS(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        cameraPos_ -= cameraSpeed_ * cameraFront_;
    }
}

void MainWindow::OnD(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        cameraPos_ += glm::normalize(glm::cross(cameraFront_, cameraUp_)) * cameraSpeed_;
    }
}

void MainWindow::OnEscape(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}
