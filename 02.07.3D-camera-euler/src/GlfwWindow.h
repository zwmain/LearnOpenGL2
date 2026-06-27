#pragma once

#include <glad/gl.h> // glad must be included before GLFW
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include <Shader.h>
#include <Texture.h>
#include <functional>
#include <unordered_map>
#include <glm/glm.hpp>

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
    void OnMouseMove(double xpos, double ypos);
    void OnMouseScroll(double xoffset, double yoffset);

    // 注册按键处理器：返回用于注销的 token（非 0 有效）
    // 回调签名：void(int key, int action, int mods)
    using KeyHandler = std::function<void(int key, int action, int mods)>;
    void RegisterKeyHandler(int key, int mods, KeyHandler handler);

private:
    // 渲染当前帧内容
    void Render();
    // 创建并上传顶点数据到 GPU
    void PrepareData();
    // 创建、编译并链接顶点/片段着色器
    void PrepareShader();
    // 创建纹理
    void PrepareTexture();

    void OnCtrlP(int key, int action, int mods);
    void OnCtrlR(int key, int action, int mods);
    void OnW(int key, int action, int mods);
    void OnA(int key, int action, int mods);
    void OnS(int key, int action, int mods);
    void OnD(int key, int action, int mods);
    void OnEscape(int key, int action, int mods);

private:
    GLFWwindow* window_ = nullptr; // GLFW 窗口对象指针
    int width_ = 800; // 窗口宽度
    int height_ = 600; // 窗口高度
    bool isPerspective_ = true; // 是否使用透视投影
    std::vector<unsigned int> vaoIds_; // 存储创建的 VAO ID
    std::vector<Shader> shaderPrograms_; // 存储创建的 Shader Program
    Texture texture_; // 存储创建的纹理对象
    glm::vec3 cameraPos_ = { 0.0f, 0.0f, 3.0f }; // 相机位置
    glm::vec3 cameraFront_ = { 0.0f, 0.0f, -1.0f }; // 相机朝向，pos + front = 观察点
    glm::vec3 cameraUp_ = { 0.0f, 1.0f, 0.0f }; // 参考上方向
    float cameraSpeed_ = 0.05f;
    float deltaTime_ = 0.0f; // 当前帧与上一帧的时间差
    float lastFrameTime_ = 0.0f; // 上一帧的时间戳
    float lastX_ = width_ / 2; // 鼠标上一次的 x 坐标
    float lastY_ = height_ / 2; // 鼠标上一次的 y 坐标
    float sensitivity_ = 0.1f; // 鼠标灵敏度
    float yaw_ = -90.0f; // 水平旋转角度，初始值为 -90 度，使得相机朝向负 z 轴
    float pitch_ = 0.0f; // 垂直旋转角度，初始值为 0 度
    bool firstMouse_ = true; // 是否是第一次鼠标移动事件，用于初始化 lastX_ 和 lastY_
    float fov_ = 45.0f; // 视野角度，初始值为 45 度

    // 可扩展的按键处理器支持
    struct KeyCombo {
        int key;
        int mods;
        bool operator==(KeyCombo const& o) const noexcept { return key == o.key && mods == o.mods; }
    };
    struct KeyComboHash {
        std::size_t operator()(KeyCombo const& k) const noexcept
        {
            // 将 int 位模式安全转为 uint32_t（保留负数补码）
            uint32_t key_u32 = static_cast<uint32_t>(k.key);
            uint32_t mods_u32 = static_cast<uint32_t>(k.mods);
            // 用 uint64_t 中间计算，再转为 size_t（避免 32 位移位问题）
            uint64_t hash64 = (static_cast<uint64_t>(key_u32) << 32) | mods_u32;
            return std::hash<uint64_t> { }(hash64);
        }
    };

    std::unordered_map<KeyCombo, KeyHandler, KeyComboHash> handlers_;
};
