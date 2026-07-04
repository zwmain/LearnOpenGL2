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
    UpdateOrientationFromIncrement(-xoffset, yoffset);
}

// ============================================================================
// 方法 7: Reset() - 重置相机到初始状态
// ============================================================================
void QuateCamera::Reset()
{
    // 重置位置
    position_ = glm::vec3(0.0f, 0.0f, 3.0f);
    orientation_ = { 1.0f, 0.0f, 0.0f, 0.0f };
    UpdateOrientationFromIncrement(0.0f, 0.0f); // 重置四元数方向
}

void QuateCamera::UpdateOrientationFromIncrement(float yawOffset, float pitchOffset)
{
    glm::mat3 rotationMatrix = glm::mat3_cast(orientation_);
    // 1. 创建绕Y轴（Yaw）的增量旋转四元数
    // 注意：glm::angleAxis 接受的是弧度
    glm::vec3 localUp = rotationMatrix * glm::vec3(0.0f, 1.0f, 0.0f); // 相机的局部上向量
    glm::quat qYawIncrement = glm::angleAxis(glm::radians(yawOffset), localUp);

    // 2. 创建绕X轴（Pitch）的增量旋转四元数
    // 获取当前相机局部右向量
    glm::vec3 localRight = rotationMatrix * glm::vec3(1, 0, 0);
    glm::quat qPitchIncrement = glm::angleAxis(glm::radians(pitchOffset), localRight);

    // 3. 组合增量旋转
    // 顺序很重要。通常先处理Yaw（左右看），再处理Pitch（上下看）。
    // 这意味着 qPitchIncrement 是在 qYawIncrement 旋转后的坐标系中进行的。
    glm::quat qIncrement = qPitchIncrement * qYawIncrement;

    // 4. 将增量旋转应用到当前朝向
    // 新的朝向 = 增量旋转 * 当前朝向
    orientation_ = qIncrement * orientation_;

    // 5. 归一化，防止浮点数误差累积
    orientation_ = glm::normalize(orientation_);

    // 6. (可选) 俯仰角限制
    // 增量旋转本身不会自动限制俯仰角，如果需要限制，需要更复杂的逻辑，
    // 例如：将当前四元数转回欧拉角，检查并修正pitch，再转回四元数。
    // 为了保持纯粹的增量旋转，这里暂时省略此步骤。
}

