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
        , orientation_{ 1.0f, 0.0f, 0.0f, 0.0f }
    {
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

    void UpdateOrientationFromIncrement(float yawOffset, float pitchOffset);
};
