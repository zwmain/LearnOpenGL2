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
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* vidMode = glfwGetVideoMode(primaryMonitor);
    int xPos = (vidMode->width - width_) / 2;
    int yPos = (vidMode->height - height_) / 2;
    glfwSetWindowPos(window_, xPos, yPos);
    glfwMakeContextCurrent(window_);

    // 将 this 绑定到 GLFW 窗口，回调中可以访问当前对象
    glfwSetWindowUserPointer(window_, this);
    glfwSetFramebufferSizeCallback(window_, FrameBufferSizeCallback);
    glfwSetKeyCallback(window_, KeyCallBack);
    if (isFocus_) {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    } else {
        glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
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

    InitImgui();
}

MainWindow::~MainWindow()
{
    DestroyImgui();
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

        BeforeRender();

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Render(); // opengl图形渲染

        AfterRender();

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
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureKeyboard)
        return;
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
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return; // ImGui 正在使用鼠标，忽略给应用的处理
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
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;
    if (!isFocus_) {
        return;
    }
    // 顺时针旋转，yoffset < 0，需要做的是缩小
    // 逆时针旋转，yoffset > 0，需要做的是放大
    camera_->ProcessMouseScroll(yoffset);
}

void MainWindow::OnMouseClick(int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse)
        return;
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

    auto cubeMesh = std::make_shared<Mesh>();
    cubeMesh->SetIndexBuffer(eleIndex);
    cubeMesh->AddVertexBuffer(vertices);
    int location = shader_["cube"]->GetAttrLocation("aPos");
    cubeMesh->AddVertexAttribute(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    mesh_["cube"] = cubeMesh;

    auto lampMesh = std::make_shared<Mesh>();
    lampMesh->SetIndexBuffer(eleIndex);
    lampMesh->AddVertexBuffer(vertices);
    location = shader_["lamp"]->GetAttrLocation("aPos");
    lampMesh->AddVertexAttribute(location, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    mesh_["lamp"] = lampMesh;
}

void MainWindow::PrepareShader()
{
    // 获取当前执行目录（当前工作目录）
    std::string currentDir = fs::current_path().string();

    // 使用绝对路径（相对于 exe 所在目录的 assets 文件夹）
    std::string vertexPath = currentDir + R"(/assets/shader/vertex/CubeVertex.glsl)";
    std::string fragmentPath = currentDir + R"(/assets/shader/fragment/CubeFragment.glsl)";
    auto cubeShader = std::make_shared<Shader>(vertexPath, fragmentPath);
    shader_["cube"] = cubeShader;

    vertexPath = currentDir + R"(/assets/shader/vertex/LampVertex.glsl)";
    fragmentPath = currentDir + R"(/assets/shader/fragment/LampFragment.glsl)";
    auto lampShader = std::make_shared<Shader>(vertexPath, fragmentPath);
    shader_["lamp"] = lampShader;
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
        //glm::vec3(2.0f, 5.0f, -7.0f),
        //glm::vec3(-1.5f, -2.2f, -2.5f),
        //glm::vec3(-3.8f, -2.0f, -12.3f),
        //glm::vec3(2.4f, -0.4f, -3.5f),
        //glm::vec3(-1.7f, 3.0f, -7.5f),
        //glm::vec3(1.3f, -2.0f, -2.5f),
        //glm::vec3(1.5f, 2.0f, -2.5f),
        //glm::vec3(1.5f, 0.2f, -1.5f),
        //glm::vec3(-1.3f, 1.0f, -1.5f)
    };

    auto uniformSetter = [this](RenderContext& context, float aspectRatio) {
        auto shader = context.GetShader();
        auto transform = context.GetTransform();
        auto camera = context.GetCamera();

        glm::mat4 model = transform->GetModelMatrix();
        shader->SetUniformMat4f("model", glm::value_ptr(model));
        glm::mat4 view = camera->GetViewMatrix();
        glm::mat4 projection = camera->GetProjectionMatrix(aspectRatio);
        shader->SetUniformMat4f("view", glm::value_ptr(view));
        shader->SetUniformMat4f("projection", glm::value_ptr(projection));
        shader->SetUniformVec3f("lightColor", glm::value_ptr(lightColor_));
        shader->SetUniformVec3f("objectColor", glm::value_ptr(objectColor_));
        shader->SetUniform1f("ambientStrength", ambientStrength_);
    };

    for (int i = 0; i < cubePositions.size(); ++i) {
        RenderContext ctx;
        ctx.SetMesh(mesh_["cube"]);
        ctx.SetCamera(camera_);
        ctx.SetShader(shader_["cube"]);
        ctx.SetUniformSetter(uniformSetter);

        auto transform = std::make_shared<Transform>();
        transform->SetPosition(cubePositions[i]);
        transform->SetRotation(glm::vec3(-45.0f, 0.0f, 45.0f));
        if (i > 0) {
            transform->Rotate(glm::vec3(0.0f, i * 10.0f, 0.0f));
        }
        ctx.SetTransform(transform);

        scenes_.push_back(std::move(ctx));
    }

    {
        auto lampUniformSetter = [this](RenderContext& context, float aspectRatio) {
            auto shader = context.GetShader();
            auto transform = context.GetTransform();
            auto camera = context.GetCamera();

            glm::mat4 model = transform->GetModelMatrix();
            shader->SetUniformMat4f("model", glm::value_ptr(model));
            glm::mat4 view = camera->GetViewMatrix();
            glm::mat4 projection = camera->GetProjectionMatrix(aspectRatio);
            shader->SetUniformMat4f("view", glm::value_ptr(view));
            shader->SetUniformMat4f("projection", glm::value_ptr(projection));
            shader->SetUniformVec3f("lightColor", glm::value_ptr(lightColor_));
        };
        RenderContext ctx;
        ctx.SetMesh(mesh_["lamp"]);
        ctx.SetCamera(camera_);
        ctx.SetShader(shader_["lamp"]);
        ctx.SetUniformSetter(lampUniformSetter);

        auto transform = std::make_shared<Transform>();
        transform->SetPosition({ 1.2, 1.1, 1.2});
        transform->SetScale({ 0.2, 0.2, 0.2 });
        ctx.SetTransform(transform);

        scenes_.push_back(std::move(ctx));
    }
}

void MainWindow::InitImgui()
{
    // 创建上下文
    IMGUI_CHECKVERSION();
    imguiContext_ = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // 开启 Docking 和 多视口支持
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // 启用键盘控制
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; // 启用 Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // 启用多视口（允许窗口拖出主窗口）
    // 设置主题
    ImGui::StyleColorsDark();

    // 绑定 GLFW 和 OpenGL 后端
    ImGui_ImplGlfw_InitForOpenGL(window_, true);
    ImGui_ImplOpenGL3_Init("#version 460"); // 根据你的 OpenGL 版本调整
}

void MainWindow::DestroyImgui()
{
    // 1. 清理并销毁 ImGui 的 OpenGL 3 渲染后端
    ImGui_ImplOpenGL3_Shutdown();

    // 2. 清理并销毁 ImGui 的 GLFW 输入后端
    ImGui_ImplGlfw_Shutdown();

    // 3. 销毁 ImGui 的主上下文
    ImGui::DestroyContext(imguiContext_);

    imguiContext_ = nullptr;
}

void MainWindow::BeforeRender()
{
    // 1. 开始 ImGui 的新帧 (必须在处理输入事件后，绘制任何东西之前调用)
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // 2. ImGui UI 逻辑 (可以放在自定义 Render 之前或之后，但通常放在这里)
    {
        ImGuiViewport* vp = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(vp->WorkPos);
        ImGui::SetNextWindowSize(vp->WorkSize);
        ImGui::SetNextWindowViewport(vp->ID);

        ImGuiWindowFlags host_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove
            | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // 让宿主窗口完全透明，去掉边框圆角和边框宽度
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0)); // 关键：透明背景

        ImGui::Begin("DockHost", nullptr, host_flags);
        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor();

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);

        ImGui::End();
    }

    // Create gui component
    {

        ImGui::Begin("Parameters");
        ImGui::Text("Modify params:");

        // 光源颜色 (Light Color)
        static float lightColor[3] = { 1.0f, 1.0f, 1.0f };
        if (ImGui::ColorEdit3("Light Color", lightColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar)) {
            lightColor_[0] = static_cast<float>(lightColor[0]);
            lightColor_[1] = static_cast<float>(lightColor[1]);
            lightColor_[2] = static_cast<float>(lightColor[2]);
        }

        // 物体颜色 (Object Color)
        static float objectColor[3] = { 1.0f, 0.5f, 0.31f };
        if (ImGui::ColorEdit3("Object Color", objectColor, ImGuiColorEditFlags_Float | ImGuiColorEditFlags_AlphaBar)) {
            objectColor_[0] = static_cast<float>(objectColor[0]);
            objectColor_[1] = static_cast<float>(objectColor[1]);
            objectColor_[2] = static_cast<float>(objectColor[2]);
        }

        // 重置按钮
        if (ImGui::Button("Reset Colors")) {
            lightColor_[0] = 1.0f;
            lightColor_[1] = 1.0f;
            lightColor_[2] = 1.0f;
            objectColor_[0] = 1.0f;
            objectColor_[1] = 0.5f;
            objectColor_[2] = 0.31f;
        }

        // Ambient Strength 调节
        {
            static float ambientStrength = 0.1f;
            ImGui::SliderFloat("Ambient Strength", &ambientStrength, 0.0f, 1.0f);
            ambientStrength_ = ambientStrength;
        }

        ImGui::End();

        // 这里可以创建其他 ImGui 窗口...
    }
}

void MainWindow::AfterRender()
{
    // 4. 渲染 ImGui (必须在您自己的渲染之后，交换缓冲区之前)
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // 5. 处理多视口 (如果开启了 ImGuiConfigFlags_ViewportsEnable)
    //    必须在 ImGui::Render() 之后调用
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
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
