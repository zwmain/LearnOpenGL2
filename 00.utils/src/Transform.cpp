#include "Transform.h"

glm::mat4 Transform::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, scale);
    return model;
}

void Transform::Reset() {
    position = glm::vec3(0.0f);
    rotation = glm::vec3(0.0f);
    scale = glm::vec3(1.0f);
}

void Transform::SetPosition(float x, float y, float z) {
    position = glm::vec3(x, y, z);
}

void Transform::SetRotation(float x, float y, float z) {
    rotation = glm::vec3(x, y, z);
}

void Transform::SetScale(float x, float y, float z) {
    scale = glm::vec3(x, y, z);
}

void Transform::Translate(float x, float y, float z) {
    position += glm::vec3(x, y, z);
}

void Transform::Rotate(float x, float y, float z) {
    rotation += glm::vec3(x, y, z);
}