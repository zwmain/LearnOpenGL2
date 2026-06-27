#include "EulerCamera.h"

// EulerCamera 构造函数实现
EulerCamera::EulerCamera(glm::vec3 position, glm::vec3 front, glm::vec3 up,
                         float yaw, float pitch)
    : Camera(position, front, up)
    , yaw_(yaw)
    , pitch_(pitch)
{
    UpdateCameraVectors();
}

// 获取视角矩阵实现
glm::mat4 EulerCamera::GetViewMatrix() const
{
    return glm::lookAt(position_, position_ + front_, up_);
}

// 处理键盘输入实现
void EulerCamera::ProcessKeyboard(CameraDirection direction, float deltaTime)
{
    float velocity = moveSpeed_ * deltaTime;
    if (direction == CameraDirection::FORWARD)
        position_ += front_ * velocity;
    if (direction == CameraDirection::BACKWARD)
        position_ -= front_ * velocity;
    if (direction == CameraDirection::LEFT)
        position_ -= right_ * velocity;
    if (direction == CameraDirection::RIGHT)
        position_ += right_ * velocity;
    if (direction == CameraDirection::UP)
        position_ += up_ * velocity;
    if (direction == CameraDirection::DOWN)
        position_ -= up_ * velocity;
}

// 处理鼠标移动实现
void EulerCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // 限制俯仰角，防止翻转
    if (constrainPitch) {
        if (pitch_ > 89.0f)
            pitch_ = 89.0f;
        if (pitch_ < -89.0f)
            pitch_ = -89.0f;
    }

    UpdateCameraVectors();
}

// 更新相机向量实现
void EulerCamera::UpdateCameraVectors()
{
    // 计算新的前向量
    glm::vec3 front { 0, 0, 0 };
    front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front.y = sin(glm::radians(pitch_));
    front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
    front_ = glm::normalize(front);

    // 重新计算右向量和上向量
    right_ = glm::normalize(glm::cross(front_, worldUp_));
    up_ = glm::normalize(glm::cross(right_, front_));
}
