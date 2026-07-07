#pragma once

#include <memory>
#include <functional>

#include "Camera.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "Transform.h"

class RenderContext {
public:
    RenderContext() = default;

    void SetMesh(const std::shared_ptr<Mesh>& mesh) { mesh_ = mesh; }
    void SetCamera(const std::shared_ptr<Camera>& camera) { camera_ = camera; }
    void SetShader(const std::shared_ptr<Shader>& shader) { shader_ = shader; }
    void SetTexture(const std::shared_ptr<Texture>& texture) { texture_ = texture; }
    void SetTransform(const std::shared_ptr<Transform>& transform) { transform_ = transform; }
    void SetUniformSetter(const std::function<void(RenderContext&, float)>& setter) { uniformSetter_ = setter; }

    const std::shared_ptr<Mesh>& GetMesh() const { return mesh_; }
    const std::shared_ptr<Camera>& GetCamera() const { return camera_; }
    const std::shared_ptr<Shader>& GetShader() const { return shader_; }
    const std::shared_ptr<Texture>& GetTexture() const { return texture_; }
    const std::shared_ptr<Transform>& GetTransform() const { return transform_; }

    void Render(float aspectRatio = 1.0f);

    void Clear()
    {
        mesh_.reset();
        camera_.reset();
        shader_.reset();
        texture_.reset();
        transform_.reset();
    }

private:
    std::shared_ptr<Mesh> mesh_;
    std::shared_ptr<Camera> camera_;
    std::shared_ptr<Shader> shader_;
    std::shared_ptr<Texture> texture_;
    std::shared_ptr<Transform> transform_;
    std::function<void(RenderContext&, float)> uniformSetter_;
};
