#include "Color.h"
#include <algorithm>

// 8bit 转归一化值
float Color::toNormalized(unsigned char value) {
    return static_cast<float>(value) / 255.0f;
}

// 归一化值钳制到有效范围
float Color::clamp(float value) {
    float result = std::max(static_cast<float>(0.0f), value);
    return std::min(result, static_cast<float>(1.0f));
}

// 构造函数：接收 8bit 参数并转换为归一化
Color::Color(unsigned char r, unsigned char g,
             unsigned char b, unsigned char a)
    : rgba_(toNormalized(r), toNormalized(g), toNormalized(b), toNormalized(a)) {}

// Setter: 归一化范围
void Color::setR(float r) { rgba_[0] = clamp(r); }
void Color::setG(float g) { rgba_[1] = clamp(g); }
void Color::setB(float b) { rgba_[2] = clamp(b); }
void Color::setA(float a) { rgba_[3] = clamp(a); }

// Setter: 8bit 范围
void Color::set8BitR(unsigned char r) { rgba_[0] = toNormalized(r); }
void Color::set8BitG(unsigned char g) { rgba_[1] = toNormalized(g); }
void Color::set8BitB(unsigned char b) { rgba_[2] = toNormalized(b); }
void Color::set8BitA(unsigned char a) { rgba_[3] = toNormalized(a); }

// 批量设置：归一化范围
void Color::set(float r, float g, float b, float a) {
    rgba_[0] = clamp(r);
    rgba_[1] = clamp(g);
    rgba_[2] = clamp(b);
    rgba_[3] = clamp(a);
}

// 批量设置：8bit 范围
void Color::set8Bit(unsigned char r, unsigned char g,
                    unsigned char b, unsigned char a) {
    rgba_[0] = toNormalized(r);
    rgba_[1] = toNormalized(g);
    rgba_[2] = toNormalized(b);
    rgba_[3] = toNormalized(a);
}
