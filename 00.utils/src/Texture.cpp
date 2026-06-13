#include "Texture.h"
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(const std::string& filePath,
    unsigned int textureUnit)
{
    try {
        if (!LoadFromFile(filePath, textureUnit)) {
            std::cerr << "Failed to load texture from file: " << filePath << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while loading texture: " << e.what() << std::endl;
    }
}

Texture::~Texture()
{
    Destroy();
}

bool Texture::LoadFromFile(const std::string& filePath,
    unsigned int textureUnit,
    bool flipY)
{
    Destroy();
    textureUnit_ = textureUnit;

    if (flipY) {
        stbi_set_flip_vertically_on_load(true);
    }

    unsigned char* data = stbi_load(filePath.c_str(), &width_, &height_, &channels_, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "[Texture] Failed to load image: " << filePath << std::endl;
        width_ = 0;
        height_ = 0;
        channels_ = 0;
        return false;
    }

    channels_ = 4; // STBI_rgb_alpha 强制输出 RGBA

    glGenTextures(1, &id_);
    glActiveTexture(GL_TEXTURE0 + textureUnit_);
    glBindTexture(target_, id_);

    glTexImage2D(target_, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(target_);

    SetWrapMode();
    SetFilterMode();

    stbi_image_free(data);
    //glBindTexture(target_, 0);

    return true;
}

void Texture::Bind(unsigned int unit)
{
    if (!IsValid()) {
        return;
    }

    textureUnit_ = unit;

    Bind();
}

void Texture::Bind() const
{
    if (!IsValid()) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit_);
    glBindTexture(target_, id_);
}

void Texture::Unbind() const
{
    if (!IsValid()) {
        return;
    }
    glActiveTexture(GL_TEXTURE0 + textureUnit_);
    glBindTexture(target_, 0);
}

void Texture::SetWrapMode(GLenum wrapS, GLenum wrapT) const
{
    if (!IsValid()) {
        return;
    }
    Bind();
    glTexParameteri(target_, GL_TEXTURE_WRAP_S, wrapS);
    glTexParameteri(target_, GL_TEXTURE_WRAP_T, wrapT);
}

void Texture::SetFilterMode(GLenum minFilter, GLenum magFilter) const
{
    if (!IsValid()) {
        return;
    }
    Bind();
    glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, magFilter);
}

void Texture::Destroy()
{
    if (id_ != 0) {
        Unbind();
        glDeleteTextures(1, &id_);
        id_ = 0;
        width_ = 0;
        height_ = 0;
        channels_ = 0;
        textureUnit_ = 0;
    }
}
