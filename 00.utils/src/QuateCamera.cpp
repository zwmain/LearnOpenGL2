// 相机实现文件
#include "QuateCamera.h"
#include <algorithm> // For std::clamp

// QuateCamera 实现文件

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
glm::mat4 QuateCamera::GetViewMatrix() const
{

    //换个视角看世界，那么就要挪动相机；以世界为参考系，过程是：

    // 1. 相机本体就和普通物体一样，绕世界坐标系旋转
    // 2. 在以世界坐标系为参考平移到相机位置

    // 那么站在相机的视角，以相机坐标系为参考系，过程就变为了

    // 1. 将整个世界延相机位置相反的方向平移
    // 2. 再根据相机坐标系进行旋转，旋转的是整个世界


    // 使用四元数的共轭（逆）作为视图的旋转部分
    glm::mat4 rotationMatrix = glm::mat4_cast(glm::conjugate(orientation_));
    // 平移矩阵：将世界反向平移到相机位置
    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), -position_);
    // 视图矩阵 = 逆旋转 * 平移（相当于把世界先平移，再旋转）
    glm::mat4 view = rotationMatrix * translationMatrix;

    // 由于是反过程/逆过程，因此旋转矩阵是逆矩阵，平移矩阵也是取负数

    return view;
}


// ============================================================================
// 方法 2: ProcessKeyboard() - 处理键盘输入 (移动)
// ============================================================================
void QuateCamera::ProcessKeyboard(CameraDirection direction, float deltaTime)
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
void QuateCamera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    //std::cout << xoffset << " " << yoffset << std::endl;
    xoffset *= mouseSensitivity_;
    yoffset *= mouseSensitivity_;

    // 在下面的更新四元数中，yaw代表绕y轴正方向旋转，也就是逆时针旋转
    // 当xoffset为正时，鼠标向右移动，期望的应该是相机本身顺时针旋转，也就是yaw减小
    // 因此这里应该取反
    yaw_ -= xoffset;
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
void QuateCamera::Reset()
{
    // 重置位置
    position_ = glm::vec3(0.0f, 0.0f, 3.0f);

    // 重置俯仰角
    yaw_ = 0.0f; // 朝向 -Z 轴
    pitch_ = 0.0f;
    roll_ = 0.0f;

    // 重置到初始朝向 (yaw=-90°, pitch=0°)
    UpdateOrientationFromEuler();
}

// ============================================================================
// private 方法：UpdateOrientationFromEuler() - 从欧拉角更新四元数方向
// ============================================================================
void QuateCamera::UpdateOrientationFromEuler()
{
    // 1. 绕 Y 轴旋转 (Yaw)转为四元数
    // 当yaw为0时，四元数表示的方向是指向Z负方向
    glm::quat qYaw = glm::angleAxis(glm::radians(yaw_), glm::vec3(0, 1, 0));

    // 2. 绕 X 轴旋转 (Pitch) 转为四元数
    glm::quat qPitch = glm::angleAxis(glm::radians(pitch_), glm::vec3(1, 0, 0));

    // 3. 绕 Z 轴旋转 (Roll) 转为四元数
    glm::quat qRoll = glm::angleAxis(glm::radians(roll_), glm::vec3(0, 0, 1));

    // 组合四元数
    orientation_ = qRoll * qPitch * qYaw;

    // 归一化防止数值误差累积
    orientation_ = glm::normalize(orientation_);
}
