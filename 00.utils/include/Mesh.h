#pragma once

#include <glad/gl.h>
#include <vector>

class Mesh {
public:
    Mesh();
    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void AddVertexBuffer(const std::vector<float>& data, GLenum usage = GL_STATIC_DRAW);

    void AddVertexAttribute(unsigned int location, int size, GLenum type,
        GLboolean normalized, GLsizei stride, const void* offset);

    void SetIndexBuffer(const std::vector<unsigned int>& indices, GLenum usage = GL_STATIC_DRAW);

    void Bind() const;
    void Unbind() const;

    void Draw(GLenum mode = GL_TRIANGLES) const;

    unsigned int GetIndexCount() const { return indexCount_; }

private:
    void Destroy();

    unsigned int vao_ { 0 };
    std::vector<unsigned int> vbos_;
    unsigned int ebo_ { 0 };

    unsigned int indexCount_ { 0 };
};