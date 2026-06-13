#pragma once

#include <glad/gl.h>
#include <string>

class Texture {
public:
    Texture() = default;
    explicit Texture(const std::string& filePath,
        unsigned int textureUnit);
    ~Texture();

    // 从文件加载纹理数据，并绑定到指定纹理单元
    bool LoadFromFile(const std::string& filePath,
        unsigned int textureUnit,
        bool flipY = true);

    // 绑定到指定纹理单元；如果未传入参数，则使用上次加载时记录的纹理单元
    void Bind(unsigned int unit);
    void Bind() const;
    void Unbind() const;

    // 设置纹理参数
    void SetWrapMode(GLenum wrapS = GL_REPEAT, GLenum wrapT = GL_REPEAT) const;
    void SetFilterMode(GLenum minFilter = GL_LINEAR_MIPMAP_LINEAR,
        GLenum magFilter = GL_LINEAR) const;

    unsigned int GetTextureUnit() const noexcept { return textureUnit_; }

    unsigned int GetId() const noexcept { return id_; }
    int GetWidth() const noexcept { return width_; }
    int GetHeight() const noexcept { return height_; }
    int GetChannels() const noexcept { return channels_; }
    bool IsValid() const noexcept { return id_ != 0; }

private:
    void Destroy();

    unsigned int id_ { };
    int width_ { };
    int height_ { };
    int channels_ { };
    unsigned int textureUnit_ { };
    GLenum target_ { GL_TEXTURE_2D };
};
