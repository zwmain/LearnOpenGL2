#pragma once

#include <string>

class Shader {
public:
    // 从文件路径加载着色器程序
    explicit Shader(const std::string& vpath, const std::string& fpath);

    void LoadShader(const std::string& vertexPath, const std::string& fragmentPath);

    // 标记当前程序已注册为可用
    void UseProgram() const;

    // 清除当前绑定的程序
    void ClearProgram() const;

    // 获取属性位置
    int GetAttrLocation(const std::string& attrName) const;

    void SetUniform1f(const std::string& name, float value) const;
    void SetUniform1i(const std::string& name, int value) const;
    void SetUniformMat4f(const std::string& name, const float* value) const;

private:
    unsigned int programId_ {};
};
