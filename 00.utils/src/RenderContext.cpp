#include "RenderContext.h"

#include <glm/gtc/type_ptr.hpp>

void RenderContext::Render(float aspectRatio)
{
    if (!shader_) {
        std::cerr << "No shader set for RenderContext." << std::endl;
    }
    if (!mesh_) {
        std::cerr << "No mesh set for RenderContext." << std::endl;
    }
    shader_->UseProgram();

    if (texture_ && texture_->IsValid()) {
        texture_->Bind();
    }

    if (uniformSetter_) {
        uniformSetter_(*this, aspectRatio);
    }

    mesh_->Draw();

    shader_->ClearProgram();

    if (texture_ && texture_->IsValid()) {
        texture_->Unbind();
    }

    mesh_->Unbind();
}
