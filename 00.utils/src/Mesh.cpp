#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
{
    glGenVertexArrays(1, &vao_);
}

Mesh::~Mesh()
{
    Destroy();
}

Mesh::Mesh(Mesh&& other) noexcept
    : vao_(other.vao_)
    , vbos_(std::move(other.vbos_))
    , ebo_(other.ebo_)
    , indexCount_(other.indexCount_)
{
    other.vao_ = 0;
    other.ebo_ = 0;
    other.indexCount_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other) {
        Destroy();
        vao_ = other.vao_;
        vbos_ = std::move(other.vbos_);
        ebo_ = other.ebo_;
        indexCount_ = other.indexCount_;
        other.vao_ = 0;
        other.ebo_ = 0;
        other.indexCount_ = 0;
    }
    return *this;
}

void Mesh::Destroy()
{
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }

    for (unsigned int vbo : vbos_) {
        glDeleteBuffers(1, &vbo);
    }
    vbos_.clear();

    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }

    indexCount_ = 0;
}

void Mesh::AddVertexBuffer(const std::vector<float>& data, GLenum usage)
{
    if (data.empty()) {
        std::cerr << "[Mesh] Vertex buffer data is empty" << std::endl;
        return;
    }

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    vbos_.push_back(vbo);
}

void Mesh::AddVertexAttribute(unsigned int location, int size, GLenum type,
    GLboolean normalized, GLsizei stride, const void* offset)
{
    if (vbos_.empty()) {
        std::cerr << "[Mesh] No vertex buffer added before adding attribute" << std::endl;
        return;
    }

    unsigned int lastVbo = vbos_.back();
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, lastVbo);
    glVertexAttribPointer(location, size, type, normalized, stride, offset);
    glEnableVertexAttribArray(location);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::SetIndexBuffer(const std::vector<unsigned int>& indices, GLenum usage)
{
    if (indices.empty()) {
        std::cerr << "[Mesh] Index buffer data is empty" << std::endl;
        return;
    }

    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
    }

    glGenBuffers(1, &ebo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), usage);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    indexCount_ = static_cast<unsigned int>(indices.size());
}

void Mesh::Bind() const
{
    glBindVertexArray(vao_);
}

void Mesh::Unbind() const
{
    glBindVertexArray(0);
}

void Mesh::Draw(GLenum mode) const
{
    if (vao_ == 0) {
        std::cerr << "[Mesh] VAO not initialized" << std::endl;
        return;
    }

    if (ebo_ == 0) {
        std::cerr << "[Mesh] Index buffer not set (EBO required)" << std::endl;
        return;
    }

    glBindVertexArray(vao_);
    glDrawElements(mode, static_cast<GLsizei>(indexCount_), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}