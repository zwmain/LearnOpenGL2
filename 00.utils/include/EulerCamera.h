#pragma once
#include "Camera.h"

// 基于欧拉角的相机类
class EulerCamera : public Camera {
public:
    // 构造函数
    EulerCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f,
        float pitch = 0.0f);

    // 获取视角矩阵
    glm::mat4 GetViewMatrix() const override;

    // 处理键盘输入
    void ProcessKeyboard(CameraDirection direction, float deltaTime) override;

    // 处理鼠标移动
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) override;

    void Reset() override;

protected:
    // 更新相机向量
    void UpdateCameraVectors();

private:
    // 欧拉角
    float yaw_;
    float pitch_;
};
