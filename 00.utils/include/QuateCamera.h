#pragma once
#include "Camera.h"
#include <algorithm> // For std::clamp
#include <glm/gtc/quaternion.hpp> // 包含角度轴旋转支持

/**
 * @brief 基于四元数的相机实现
 */
class QuateCamera : public Camera {
public:
    // 构造函数
    QuateCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
        : Camera(position, worldUp)
    {
        // 初始化四元数方向
        UpdateOrientationFromEuler();
    }

    // 析构函数
    ~QuateCamera() override = default;

    // 1. 获取视图矩阵
    glm::mat4 GetViewMatrix() const override;

    // 2. 处理键盘输入 (移动)
    void ProcessKeyboard(CameraDirection direction, float deltaTime) override;

    // 3. 处理鼠标移动 (旋转)
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) override;

    // 5. 重置相机
    void Reset() override;

private:
    glm::quat orientation_ { 1.0f, 0.0f, 0.0f, 0.0f }; // w, x, y, z
    // 四元数虽不依赖欧拉角，用欧拉角可以方便的描述物体状态，以及处理鼠标输入
    float yaw_ { -90.0f }; // 初始偏航角
    float pitch_ { 0.0f }; // 当前俯仰角 (度)
    float roll_ { 0.0f }; // 当前滚转角 (度)
    glm::vec3 front_ { 0.0f, 0.0f, -1.0f };

    // 辅助函数：仅用于构造函数初始化
    void UpdateOrientationFromEuler();
};
