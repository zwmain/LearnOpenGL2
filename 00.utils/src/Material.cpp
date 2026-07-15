#include "Material.h"

// ==================== Metals (金属类) ====================

Material MaterialFactory::Emerald()
{
    return Material(
        glm::vec3(0.0215f, 0.1745f, 0.0215f), // Ambient
        glm::vec3(0.07568f, 0.61424f, 0.07568f), // Diffuse
        glm::vec3(0.633f, 0.727811f, 0.633f), // Specular
        0.6f * 128 // Shininess
    );
}

Material MaterialFactory::Jade()
{
    return Material(
        glm::vec3(0.135f, 0.2225f, 0.1575f),
        glm::vec3(0.54f, 0.89f, 0.63f),
        glm::vec3(0.316228f, 0.316228f, 0.316228f),
        0.1f * 128);
}

Material MaterialFactory::Obsidian()
{
    return Material(
        glm::vec3(0.05375f, 0.05f, 0.06625f),
        glm::vec3(0.18275f, 0.17f, 0.22525f),
        glm::vec3(0.332741f, 0.328634f, 0.346435f),
        0.3f * 128);
}

Material MaterialFactory::Pearl()
{
    return Material(
        glm::vec3(0.25f, 0.20725f, 0.20725f),
        glm::vec3(1.0f, 0.829f, 0.829f),
        glm::vec3(0.296648f, 0.296648f, 0.296648f),
        0.088f * 128);
}

Material MaterialFactory::Ruby()
{
    return Material(
        glm::vec3(0.1745f, 0.01175f, 0.01175f),
        glm::vec3(0.61424f, 0.04136f, 0.04136f),
        glm::vec3(0.727811f, 0.626959f, 0.626959f),
        0.6f * 128);
}

Material MaterialFactory::Turquoise()
{
    return Material(
        glm::vec3(0.1f, 0.18725f, 0.1745f),
        glm::vec3(0.396f, 0.74151f, 0.69102f),
        glm::vec3(0.297254f, 0.30829f, 0.306678f),
        0.1f * 128);
}

// ==================== Metals (金属类) ====================

Material MaterialFactory::Brass()
{
    return Material(
        glm::vec3(0.329412f, 0.223529f, 0.027451f),
        glm::vec3(0.780392f, 0.568627f, 0.113725f),
        glm::vec3(0.992157f, 0.941176f, 0.807843f),
        0.21794872f * 128);
}

Material MaterialFactory::Bronze()
{
    return Material(
        glm::vec3(0.2125f, 0.1275f, 0.054f),
        glm::vec3(0.714f, 0.4284f, 0.18144f),
        glm::vec3(0.393548f, 0.271906f, 0.166721f),
        0.2f * 128);
}

Material MaterialFactory::Chrome()
{
    return Material(
        glm::vec3(0.25f, 0.25f, 0.25f),
        glm::vec3(0.4f, 0.4f, 0.4f),
        glm::vec3(0.774597f, 0.774597f, 0.774597f),
        0.4f * 128);
}

Material MaterialFactory::Copper()
{
    return Material(
        glm::vec3(0.19125f, 0.0735f, 0.0225f),
        glm::vec3(0.7038f, 0.27048f, 0.0828f),
        glm::vec3(0.256777f, 0.137622f, 0.086014f),
        0.1f * 128);
}

Material MaterialFactory::Gold()
{
    return Material(
        glm::vec3(0.24725f, 0.1995f, 0.0745f),
        glm::vec3(0.75164f, 0.60648f, 0.22648f),
        glm::vec3(0.628281f, 0.555802f, 0.366065f),
        0.4f * 128);
}

Material MaterialFactory::Silver()
{
    return Material(
        glm::vec3(0.19225f, 0.19225f, 0.19225f),
        glm::vec3(0.50754f, 0.50754f, 0.50754f),
        glm::vec3(0.508273f, 0.508273f, 0.508273f),
        0.4f * 128);
}

Material MaterialFactory::Steel()
{
    return Material(
        glm::vec3(0.13f, 0.13f, 0.13f),
        glm::vec3(0.55f, 0.55f, 0.55f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        0.6f * 128);
}

// ==================== Plastics (塑料类) ====================

Material MaterialFactory::BlackPlastic()
{
    return Material(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.01f, 0.01f, 0.01f),
        glm::vec3(0.50f, 0.50f, 0.50f),
        0.25f * 128);
}

Material MaterialFactory::CyanPlastic()
{
    return Material(
        glm::vec3(0.0f, 0.1f, 0.06f),
        glm::vec3(0.0f, 0.50980392f, 0.50980392f),
        glm::vec3(0.50196078f, 0.50196078f, 0.50196078f),
        0.25f * 128);
}

Material MaterialFactory::GreenPlastic()
{
    return Material(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.1f, 0.35f, 0.1f),
        glm::vec3(0.45f, 0.55f, 0.45f),
        0.25f * 128);
}

Material MaterialFactory::RedPlastic()
{
    return Material(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.0f, 0.0f),
        glm::vec3(0.7f, 0.6f, 0.6f),
        0.25f * 128);
}

Material MaterialFactory::WhitePlastic()
{
    return Material(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.55f, 0.55f, 0.55f),
        glm::vec3(0.70f, 0.70f, 0.70f),
        0.25f * 128);
}

Material MaterialFactory::YellowPlastic()
{
    return Material(
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.5f, 0.5f, 0.0f),
        glm::vec3(0.60f, 0.60f, 0.50f),
        0.25f * 128);
}

/**
 * @brief 根据名称获取材质
 * @param name 材质名称 (不区分大小写)
 * @return Material 结构体，如果未找到则返回默认黑色材质
 */
Material MaterialFactory::GetMaterial(const std::string& name)
{
    // 简单转换为小写以便比较
    std::string lowerName = name;
    for (auto& c : lowerName)
        c = tolower(c);

    if (lowerName == "emerald")
        return Emerald();
    if (lowerName == "jade")
        return Jade();
    if (lowerName == "obsidian")
        return Obsidian();
    if (lowerName == "pearl")
        return Pearl();
    if (lowerName == "ruby")
        return Ruby();
    if (lowerName == "turquoise")
        return Turquoise();

    if (lowerName == "brass")
        return Brass();
    if (lowerName == "bronze")
        return Bronze();
    if (lowerName == "chrome")
        return Chrome();
    if (lowerName == "copper")
        return Copper();
    if (lowerName == "gold")
        return Gold();
    if (lowerName == "silver")
        return Silver();
    if (lowerName == "steel")
        return Steel();

    if (lowerName == "black_plastic" || lowerName == "blackplastic")
        return BlackPlastic();
    if (lowerName == "cyan_plastic" || lowerName == "cyanplastic")
        return CyanPlastic();
    if (lowerName == "green_plastic" || lowerName == "greenplastic")
        return GreenPlastic();
    if (lowerName == "red_plastic" || lowerName == "redplastic")
        return RedPlastic();
    if (lowerName == "white_plastic" || lowerName == "whiteplastic")
        return WhitePlastic();
    if (lowerName == "yellow_plastic" || lowerName == "yellowplastic")
        return YellowPlastic();

    // 默认返回黑色塑料
    return BlackPlastic();
}
