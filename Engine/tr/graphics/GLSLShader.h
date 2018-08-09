#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"

namespace tr {
class Log;
class GLSLShader : public Resource {
public:
    static uint CompileShader(const std::string &vertex,
                              const std::string &fragment);

    explicit GLSLShader(uint program);
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

private:
    int GetUniformLocation(const std::string &name);

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
