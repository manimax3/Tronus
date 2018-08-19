#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"

namespace tr {

/**
 * See ShaderInterface for Info about this
 */
namespace detail::ShaderInterfaceTypes {

    enum class Element { UNIFORM, ATTRIBUTE };
    enum class ElementType {
        SAMPLER2D,
        MAT4,
        VEC4,
        VEC3,
        VEC2,
        FLOAT,
        BOOL,
        STRUCT
    };
    enum class Sampler2DType { ALBEDO, DIFFUSE, SPECULAR, OTHER };

    struct Attribute {
        int         location;
        ElementType type;
    };

    struct Uniform {
        std::string   name;
        ElementType   type;
        Sampler2DType sampler_type;
    };

    void from_json(const json &j, Element &e);
    void from_json(const json &j, ElementType &type);
    void from_json(const json &j, Sampler2DType &type);
    void from_json(const json &j, Uniform &uniform);
    void from_json(const json &j, Attribute &attribute);
}

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

    using ElementType   = detail::ShaderInterfaceTypes::ElementType;
    using Element       = detail::ShaderInterfaceTypes::Element;
    using Sampler2DType = detail::ShaderInterfaceTypes::Sampler2DType;
    using Attribute     = detail::ShaderInterfaceTypes::Attribute;
    using Uniform       = detail::ShaderInterfaceTypes::Uniform;

    void AddAttribute(ElementType type, int location);

    void AddUniform(ElementType   type,
                    std::string   name,
                    Sampler2DType opt_sampler_type = Sampler2DType::OTHER);

    bool HasAttribute(int location, ElementType type) const;

    bool HasUniform(const std::string &name,
                    ElementType        type,
                    Sampler2DType      opt_sampler_type
                    = Sampler2DType::OTHER) const;

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
