#pragma once
#include <iostream>
#include <vector>

// 包含GLM库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// 相机移动方向枚举
enum class CameraDirection {
    FORWARD = 0,
    BACKWARD = 1,
    LEFT = 2,
    RIGHT = 3,
    UP = 4,
    DOWN = 5
};

// 相机抽象基类
class Camera {
public:
    // 构造函数
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f))
    {
        position_ = position;
        worldUp_ = worldUp;
    }

    // 析构函数
    virtual ~Camera() { }

    // 获取视角矩阵 - 纯虚函数
    virtual glm::mat4 GetViewMatrix() const = 0;

    // 获取投影矩阵
    glm::mat4 GetProjectionMatrix(float aspectRatio) const
    {
        float near = 0.1f;
        float far = 1000.0f;
        float fov = glm::radians(45.0f * zoom_); // 视野角度
        if (isPerspective_) {
            return glm::perspective(fov, aspectRatio, near, far);
        } else {
            float top = tan(fov / 2.0f) * ((near + far) / 200.0f); // 这里除以200是尝试出来的
            float right = top * aspectRatio;
            return glm::ortho(-right, right, -top, top, near, far);
        }
    }

    // 处理键盘输入
    virtual void ProcessKeyboard(CameraDirection direction, float deltaTime) = 0;

    // 处理鼠标移动
    virtual void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true) = 0;

    // 处理鼠标滚轮
    void ProcessMouseScroll(float yoffset)
    {
        // 顺时针旋转，yoffset < 0，需要做的是缩小
        // 逆时针旋转，yoffset > 0，需要做的是放大
        zoom_ -= yoffset * 0.05;
        if (zoom_ < 0.0f)
            zoom_ = 0.0f;
        if (zoom_ > 1.3f)
            zoom_ = 1.3f;
    }

    // 获取各种相机参数
    const glm::vec3& GetPosition() const { return position_; }
    const glm::vec3& GetWorldUp() const { return worldUp_; }
    float GetZoom() const { return zoom_; }
    virtual void Reset() = 0;

    void SetPerspectiveProjection(bool perspective) { isPerspective_ = perspective; }
    bool IsPerspectiveProjection() const { return isPerspective_; }
    void ToggleProjectionMode() { isPerspective_ = !isPerspective_; }

protected:
    // 相机属性
    glm::vec3 position_ { 0.0f, 0.0f, 3.0f };
    glm::vec3 worldUp_ { 0.0f, 1.0f, 0.0f };

    // 相机选项
    float moveSpeed_ = 2.5f;
    float mouseSensitivity_ = 0.1f;
    float zoom_ = 1.0f; // 缩放倍数

    // 是否使用透视投影
    bool isPerspective_ = true;
};
