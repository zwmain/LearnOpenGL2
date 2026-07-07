#include "RenderContext.h"

#include <glm/gtc/type_ptr.hpp>

void RenderContext::Render(float aspectRatio)
{
    if (shader_) {
        shader_->UseProgram();
    }

    if (texture_ && texture_->IsValid()) {
        texture_->Bind();
        if (shader_) {
            shader_->SetUniform1i("sampler", static_cast<int>(texture_->GetTextureUnit()));
        }
    }

    if (shader_ && transform_) {
        glm::mat4 model = transform_->GetModelMatrix();
        shader_->SetUniformMat4f("model", glm::value_ptr(model));
    }

    if (shader_ && camera_) {
        glm::mat4 view = camera_->GetViewMatrix();
        glm::mat4 projection = camera_->GetProjectionMatrix(aspectRatio);
        shader_->SetUniformMat4f("view", glm::value_ptr(view));
        shader_->SetUniformMat4f("projection", glm::value_ptr(projection));
    }

    if (mesh_) {
        mesh_->Draw();
    }

    if (shader_) {
        shader_->ClearProgram();
    }

    if (texture_ && texture_->IsValid()) {
        texture_->Unbind();
    }

    if (mesh_) {
        mesh_->Unbind();
    }
}
