#include "Shader.h"
#include <fstream>
#include <glad/gl.h>
#include <iostream>
#include <sstream>

// 从文件路径加载并初始化 Shader 类
Shader::Shader(const std::string& vpath, const std::string& fpath)
{
    try {
        LoadShader(vpath, fpath);
    } catch (const std::exception& e) {
        std::cerr << "[Shader] Error loading shader: " << e.what() << std::endl;
        programId_ = 0;
    }
}

void Shader::LoadShader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 读取顶点着色器文件
    std::ifstream vertexFile(vertexPath);
    if (!vertexFile.is_open()) {
        std::cerr << "[LoadShader] Vertex shader file not found: " << vertexPath << std::endl;
        return;
    }
    std::stringstream vertexStream;
    vertexStream << vertexFile.rdbuf();
    vertexFile.close();

    // 读取片段着色器文件
    std::ifstream fragmentFile(fragmentPath);
    if (!fragmentFile.is_open()) {
        std::cerr << "[LoadShader] Fragment shader file not found: " << fragmentPath << std::endl;
        return;
    }
    std::stringstream fragmentStream;
    fragmentStream << fragmentFile.rdbuf();
    fragmentFile.close();

    // 获取源码字符串（去除可能的 BOM）
    std::string vertexCode = vertexStream.str();
    if (vertexCode.length() > 0 && vertexCode[0] == '\xef') {
        vertexCode = vertexCode.substr(1);
    }

    std::string fragmentCode = fragmentStream.str();
    if (fragmentCode.length() > 0 && fragmentCode[0] == '\xef') {
        fragmentCode = fragmentCode.substr(1);
    }

    // 创建顶点着色器对象并编译
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vertexCodeArr[] = { vertexCode.c_str() };
    glShaderSource(vertexShader, 1, vertexCodeArr, nullptr);
    glCompileShader(vertexShader);

    // 检查编译状态
    int success = 0;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(vertexShader, 1024, nullptr, infoLog);
        std::cerr << "[LoadShader] Vertex shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(vertexShader);
        return;
    }

    // 创建片段着色器对象并编译
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fragmentCodeArr[] = { fragmentCode.c_str() };
    glShaderSource(fragmentShader, 1, fragmentCodeArr, nullptr);
    glCompileShader(fragmentShader);

    // 检查编译状态
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(fragmentShader, 1024, nullptr, infoLog);
        std::cerr << "[LoadShader] Fragment shader compilation failed: " << infoLog << std::endl;
        glDeleteShader(fragmentShader);
        return;
    }

    // 创建程序对象并链接
    programId_ = glCreateProgram();
    glAttachShader(programId_, vertexShader);
    glAttachShader(programId_, fragmentShader);
    glLinkProgram(programId_);

    // 检查链接状态
    glGetProgramiv(programId_, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(programId_, 1024, nullptr, infoLog);
        std::cerr << "[LoadShader] Shader program linking failed: " << infoLog << std::endl;
        glDeleteProgram(programId_);
        programId_ = 0;
        return;
    }

    // 删除临时着色器对象
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::UseProgram() const
{
    glUseProgram(programId_);
}

void Shader::ClearProgram() const
{
    glUseProgram(0);
}
