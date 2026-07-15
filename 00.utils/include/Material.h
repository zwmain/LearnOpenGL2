#include <glm/glm.hpp>
#include <string>

// 假设这是你定义的结构体
struct Material {
    glm::vec3 ambient; // 环境光反射颜色
    glm::vec3 diffuse; // 漫反射颜色
    glm::vec3 specular; // 镜面反射颜色
    float shininess; // 高光指数

    // 构造函数方便初始化
    Material(const glm::vec3& amb, const glm::vec3& diff, const glm::vec3& spec, float shine)
        : ambient(amb)
        , diffuse(diff)
        , specular(spec)
        , shininess(shine)
    {
    }

    Material()
        : ambient(0.0f)
        , diffuse(0.0f)
        , specular(0.0f)
        , shininess(0.0f)
    {
    }
};

/**
 * @brief 材质工厂类，提供静态方法获取预定义的材质
 */
class MaterialFactory {
public:
    // --- Metals (金属类) ---
    static Material Emerald();
    static Material Jade();
    static Material Obsidian();
    static Material Pearl();
    static Material Ruby();
    static Material Turquoise();

    static Material Brass();
    static Material Bronze();
    static Material Chrome();
    static Material Copper();
    static Material Gold();
    static Material Silver();
    static Material Steel();

    // --- Plastics (塑料类) ---
    static Material BlackPlastic();
    static Material CyanPlastic();
    static Material GreenPlastic();
    static Material RedPlastic();
    static Material WhitePlastic();
    static Material YellowPlastic();

    /**
     * @brief 根据名称获取材质
     * @param name 材质名称 (不区分大小写)
     * @return Material 结构体，如果未找到则返回默认黑色材质
     */
    static Material GetMaterial(const std::string& name);
};
