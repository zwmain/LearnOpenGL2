#include "Transform.h"

glm::mat4 Transform::GetModelMatrix() const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, GetPosition());
    model = glm::rotate(model, glm::radians(GetRotation().x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(GetRotation().y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(GetRotation().z), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, GetScale());
    return model;
}

void Transform::Reset() {
    SetPosition({ 0.0f, 0.0f, 0.0f });
    SetRotation({ 0.0f, 0.0f, 0.0f });
    SetScale({ 1.0f, 1.0f, 1.0f });
}

void Transform::SetPosition(const glm::vec3& pos) {
    position_ = pos;
}

void Transform::SetRotation(const glm::vec3& rot) {
    rotation_ = rot;
}

void Transform::SetScale(const glm::vec3& scale) {
    scale_ = scale;
}

void Transform::Translate(const glm::vec3& translation) {
    position_ += translation;
}

void Transform::Rotate(const glm::vec3& rotation) {
    rotation_ += rotation;
}