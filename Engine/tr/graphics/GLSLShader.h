#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"
#include "InterfaceTypes.h"

namespace tr {

/**
 * Represents a interface to a shader
 */
class ShaderInterface {
public:
    /**
     * Default Constructor.
     * Constructs a empty layout.
     */
    ShaderInterface() = default;

    using ElementType = ShaderElementType;

    struct Attribute {
        int         location;
        ElementType type;
    };

    struct Uniform {
        std::string   name;
        ElementType   type;
    };
    void AddAttribute(ElementType type, int location);

    void AddUniform(ElementType type, std::string name);

    bool HasAttribute(int location, ElementType type) const;

    bool HasUniform(const std::string &name, ElementType type) const;

    /**
     * Check if other supporst all of our elements
     */
    bool IsCompatibleWith(const ShaderInterface &other) const;

    friend void from_json(const json &j, ShaderInterface &i);

private:
    std::list<Attribute> mAttributes;
    std::list<Uniform>   mUniforms;
};

void from_json(const json &j, ShaderInterface &i);

class GLSLShader : public Resource {
public:
    static uint CompileShader(const std::string &vertex,
                              const std::string &fragment);

    explicit GLSLShader(uint                           program,
                        std::optional<ShaderInterface> vertex_interface
                        = std::nullopt,
                        std::optional<ShaderInterface> fragment_interface
                        = std::nullopt);

    ~GLSLShader() override;

    void Bind() const;

    void Set(const std::string &name, float value);
    void Set(const std::string &name, float *values, int count);
    void Set(const std::string &name, int value);
    void Set(const std::string &name, int *value, int count);
    void Set(const std::string &name, const Vec2 &value);
    void Set(const std::string &name, const Vec3 &value);
    void Set(const std::string &name, const Vec4 &value);
    void Set(const std::string &name, const Mat4 &value);

    inline uint GetProgram() const { return mShader; }

    std::optional<ShaderInterface> GetInterfaceVertex()
    {
        return mShaderInterfaceVertex;
    }

    std::optional<ShaderInterface> GetInterfaceFragment()
    {
        return mShaderInterfaceFragment;
    }

private:
    int GetUniformLocation(const std::string &name);

    std::optional<ShaderInterface> mShaderInterfaceVertex;
    std::optional<ShaderInterface> mShaderInterfaceFragment;

    uint                       mShader;
    std::map<std::string, int> mUnifoms;
};

class GLSLShaderLoader : public ResourceLoadHandler {
public:
    ResourcePtr<> LoadResource(ResourceLoadingInformation info,
                               const ResourceType &       type,
                               ResourceManager &          rm,
                               ResourceLoadingContext     context) override;
    // TODO: Implement this
    /* std::string */
    /* ResourceName(const ResourceLoadingInformation &info) const override; */

    inline std::list<ResourceType> GetSupportedTypes() const override
    {
        return { "GLSLShader" };
    }
};
}
