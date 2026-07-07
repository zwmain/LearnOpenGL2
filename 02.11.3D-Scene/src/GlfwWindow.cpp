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

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto* obj = static_cast<MainWindow*>(glfwGetWindowUserPointer(window));
    obj->OnMouseClick(button, action, mods);
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
    glfwSetMouseButtonCallback(window_, MouseButtonCallback);

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
    PrepareScene();

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
    if (!isFocus_) {
        return;
    }
    if (isFirstMouse_) {
        lastX_ = xpos;
        lastY_ = ypos;
        isFirstMouse_ = false;
    }
    float xoffset = xpos - lastX_;
    float yoffset = lastY_ - ypos; // 注意这里是相反的，因为y坐标是从底部往顶部依次增大的
    lastX_ = xpos;
    lastY_ = ypos;

    camera_->ProcessMouseMovement(xoffset, yoffset);
}

void MainWindow::OnMouseScroll(double xoffset, double yoffset)
{
    if (!isFocus_) {
        return;
    }
    // 顺时针旋转，yoffset < 0，需要做的是缩小
    // 逆时针旋转，yoffset > 0，需要做的是放大
    camera_->ProcessMouseScroll(yoffset);
}

void MainWindow::OnMouseClick(int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        isFocus_ = true;  // 获取焦点
        isFirstMouse_ = true; // 重置鼠标首次移动标志
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // 隐藏鼠标光标
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

    for (auto& scene : scenes_) {
        scene.Render(static_cast<float>(width_) / height_);
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

    mesh_ = std::make_shared<Mesh>();
    mesh_->SetIndexBuffer(eleIndex);
    mesh_->AddVertexBuffer(vertices);
    int location = shader_->GetAttrLocation("aPos");
    mesh_->AddVertexAttribute(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    mesh_->AddVertexBuffer(uvs);
    location = shader_->GetAttrLocation("aUv");
    mesh_->AddVertexAttribute(location, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
}

void MainWindow::PrepareShader()
{
    // 获取当前执行目录（当前工作目录）
    std::string currentDir = fs::current_path().string();

    // 使用绝对路径（相对于 exe 所在目录的 assets 文件夹）
    std::string vertexPath = currentDir + R"(/assets/shader/vertex/vertex.glsl)";
    std::string fragmentPath = currentDir + R"(/assets/shader/fragment/fragment.glsl)";

    shader_ = std::make_shared<Shader>(vertexPath, fragmentPath);
}

void MainWindow::PrepareTexture()
{
    texture_ = std::make_shared<Texture>();
    texture_->LoadFromFile("assets/image/gugugaga.jpeg", 0);
}

void MainWindow::PrepareScene()
{
    camera_ = std::make_shared<QuateCamera>();

    std::vector<glm::vec3> cubePositions = {
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

    auto uniformSetter = [](RenderContext& context, float aspectRatio) {
        auto texture = context.GetTexture();
        auto shader = context.GetShader();
        auto transform = context.GetTransform();
        auto camera = context.GetCamera();

        shader->SetUniform1i("sampler", static_cast<int>(texture->GetTextureUnit()));
        glm::mat4 model = transform->GetModelMatrix();
        shader->SetUniformMat4f("model", glm::value_ptr(model));
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix(aspectRatio);
        shader->SetUniformMat4f("view", glm::value_ptr(view));
        shader->SetUniformMat4f("projection", glm::value_ptr(projection));
    };

    for (int i = 0; i < cubePositions.size(); ++i) {
        RenderContext scene;
        scene.SetMesh(mesh_);
        scene.SetCamera(camera_);
        scene.SetShader(shader_);
        scene.SetTexture(texture_);
        scene.SetUniformSetter(uniformSetter);

        auto transform = std::make_shared<Transform>();
        transform->SetRotation(glm::vec3(-45.0f, 0.0f, 45.0f));
        if (i > 0) {
            transform->SetPosition(cubePositions[i]);
            transform->Rotate(glm::vec3(0.0f, i * 10.0f, 0.0f));
        }
        scene.SetTransform(transform);

        scenes_.push_back(std::move(scene));
    }
}

void MainWindow::OnCtrlP(int key, int action, int mods)
{
    if (action == GLFW_PRESS) {
        camera_->ToggleProjectionMode();
        std::cout << "Projection mode switched to: " << (camera_->IsPerspectiveProjection() ? "Perspective" : "Orthographic") << std::endl;
    }
}

void MainWindow::OnCtrlR(int key, int action, int mods)
{
    camera_->Reset();
}

void MainWindow::OnW(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        camera_->ProcessKeyboard(CameraDirection::FORWARD, deltaTime_);
    }
}

void MainWindow::OnA(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        camera_->ProcessKeyboard(CameraDirection::LEFT, deltaTime_);
    }
}

void MainWindow::OnS(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        camera_->ProcessKeyboard(CameraDirection::BACKWARD, deltaTime_);
    }
}

void MainWindow::OnD(int key, int action, int mods)
{
    if (action == GLFW_REPEAT) {
        camera_->ProcessKeyboard(CameraDirection::RIGHT, deltaTime_);
    }
}

void MainWindow::OnEscape(int key, int action, int mods) {
    if (action == GLFW_PRESS) {
        // glfwSetWindowShouldClose(window_, GLFW_TRUE);
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        isFocus_ = false;
    }
}
