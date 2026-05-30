#pragma once

#include <array>

class Color {
public:
    // 构造函数：接收 8bit (0-255) 范围参数
    explicit Color(unsigned char r, unsigned char g,
                   unsigned char b, unsigned char a = 255);

    // Getter 接口（归一化范围）
    float R() const { return rgba_[0]; }
    float G() const { return rgba_[1]; }
    float B() const { return rgba_[2]; }
    float A() const { return rgba_[3]; }

    // Setter 接口：归一化范围 (0.0 - 1.0)
    void setR(float r);
    void setG(float g);
    void setB(float b);
    void setA(float a);

    // Setter 接口：8bit 范围 (0 - 255)
    void set8BitR(unsigned char r);
    void set8BitG(unsigned char g);
    void set8BitB(unsigned char b);
    void set8BitA(unsigned char a);

    // 批量设置接口
    void set(float r, float g, float b, float a = 1.0);          // 归一化范围
    void set8Bit(unsigned char r, unsigned char g,
                 unsigned char b, unsigned char a = 255);  // 8bit 范围

private:
    // 内部存储：归一化的 rgba 值 (0.0 - 1.0)
    std::array<float, 4> rgba_;

    // 辅助函数：8bit 转归一化
    static float toNormalized(unsigned char value);

    // 辅助函数：归一化值钳制到有效范围
    static float clamp(float value);
};
