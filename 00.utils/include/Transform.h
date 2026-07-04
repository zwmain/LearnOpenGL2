#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Transform {
public:
    glm::vec3 position { 0.0f, 0.0f, 0.0f };
    glm::vec3 rotation { 0.0f, 0.0f, 0.0f };
    glm::vec3 scale { 1.0f, 1.0f, 1.0f };

    glm::mat4 GetModelMatrix() const;

    void Reset();
    void SetPosition(float x, float y, float z);
    void SetRotation(float x, float y, float z);
    void SetScale(float x, float y, float z);
    void Translate(float x, float y, float z);
    void Rotate(float x, float y, float z);
};