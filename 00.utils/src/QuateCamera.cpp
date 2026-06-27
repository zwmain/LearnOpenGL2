// 相机实现文件
#include "QuateCamera.h"
#include <algorithm> // For std::clamp

// QuaternionCamera 实现文件

/*
 * @brief: 基于四元数的相机类实现
 *
 * 特点：
 * - 使用四元数表示相机的朝向，避免万向节死锁问题
 * - 支持平滑的鼠标旋转和键盘移动
 * - 俯仰角限制在 -89° 到 +89° 之间，防止相机翻转
 */

// ============================================================================
// 方法 1: GetViewMatrix() - 获取视图矩阵
// ============================================================================
glm::mat4 QuaternionCamera::GetViewMatrix() const
{
    // 四元数不使用 glm::lookAt 构造view矩阵\

    // 将四元数转为旋转矩阵
    glm::mat4 rotationMatrix = glm::mat4_cast(orientation_);
    // 相机坐标在pos，相当于物体反向移动pos向量，构造平移矩阵
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -position_);
    // 组合：先旋转，后平移
    glm::mat4 view = translationMatrix * rotationMatrix;

    return view;
}

// ============================================================================
// 方法 2: ProcessKeyboard() - 处理键盘输入 (移动)
// ============================================================================
void QuaternionCamera::ProcessKeyboard(CameraDirection direction, float deltaTime)
{
    // 相机移动也不使用局部坐标系，也不需要右向量/上向量
    // 计算移动速度
    float velocity = moveSpeed_ * deltaTime;

    glm::vec3 moveDirection { 0, 0, 0 };
    switch (direction) {
    case CameraDirection::FORWARD: {
        moveDirection = { 0, 0, -1 }; // 前向
        break;
    }
    case CameraDirection::BACKWARD: {
        moveDirection = { 0, 0, 1 }; // 后向
        break;
    }
    case CameraDirection::LEFT: {
        moveDirection = { -1, 0, 0 }; // 左
        break;
    }
    case CameraDirection::RIGHT: {
        moveDirection = { 1, 0, 0 }; // 右
        break;
    }
    case CameraDirection::UP: {
        moveDirection = { 0, 1, 0 }; // 上
        break;
    }
    case CameraDirection::DOWN: {
        moveDirection = { 0, -1, 0 }; // 下
        break;
    }
    default:
        break;
    }

    moveDirection *= velocity;

    // 获取当前相机旋转矩阵
    glm::mat3 rotationMatrix = glm::mat3_cast(orientation_);
    // 将相机坐标系下的移动方向转换为世界坐标系下的移动方向
    glm::vec3 worldMoveDirection = rotationMatrix * moveDirection;

    position_ += worldMoveDirection;
}

// ============================================================================
// 方法 3: ProcessMouseMovement() - 处理鼠标移动 (旋转)
// ============================================================================
void QuaternionCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;

    yaw_ += xoffset;
    pitch_ += yoffset;

    // 限制俯仰角，防止翻转
    // if (constrainPitch) {
    //    if (pitch_ > 89.0f)
    //        pitch_ = 89.0f;
    //    if (pitch_ < -89.0f)
    //        pitch_ = -89.0f;
    //}

    UpdateOrientationFromEuler();
}

// ============================================================================
// 方法 7: Reset() - 重置相机到初始状态
// ============================================================================
void QuaternionCamera::Reset()
{
    // 重置位置
    position_ = glm::vec3(0.0f, 0.0f, 3.0f);

    // 重置俯仰角
    yaw_ = -90.0f; // 朝向 -Z 轴
    pitch_ = 0.0f;
    roll_ = 0.0f;

    // 重置到初始朝向 (yaw=-90°, pitch=0°)
    UpdateOrientationFromEuler();
}

// ============================================================================
// private 方法：UpdateOrientationFromEuler() - 从欧拉角更新四元数方向
// ============================================================================
void QuaternionCamera::UpdateOrientationFromEuler()
{
    // 1. 绕 Y 轴旋转 (Yaw)转为四元数
    glm::quat qYaw = glm::angleAxis(glm::radians(yaw_), glm::vec3(0, 1, 0));

    // 2. 绕 X 轴旋转 (Pitch) 转为四元数
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch_), glm::vec3(1, 0, 0));

    // 3. 绕 Z 轴旋转 (Roll) 转为四元数
    glm::quat qRoll = glm::angleAxis(glm::radians(roll_), glm::vec3(0, 0, 1));

    // 组合四元数
    orientation_ = qRoll * qPitch * qYaw;

    // 归一化防止数值误差累积
    orientation_ = glm::normalize(orientation_);

    front_ = orientation_ * front_;
}
