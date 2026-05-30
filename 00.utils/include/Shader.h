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

private:
    unsigned int programId_ {};
};
