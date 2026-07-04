#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
private:
    glm::vec3 position_ { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation_ { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale_ { 1.0f, 1.0f, 1.0f };

public:
    glm::mat4 GetModelMatrix() const;

    void Reset();
    void SetPosition(const glm::vec3& pos);
    void SetRotation(const glm::vec3& rot);
    void SetScale(const glm::vec3& scale);

    // Getter 方法
    const glm::vec3& GetPosition() const { return position_; }
    const glm::vec3& GetRotation() const { return rotation_; }
    const glm::vec3& GetScale() const { return scale_; }

    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& rotation);
};