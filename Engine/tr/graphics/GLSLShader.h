#pragma once
#include <tr.h>

#include "../filesystem/ResourceManager.h"
#include "../math/Math.h"

namespace tr {
class Log;
class GLSLShader : public Resource {
public:
    static uint64 CompileShader(const std::string &vertex,
                                const std::string &fragment,
                                Log &              log);

    static Resource *Loader(ResourceManager::ResHandle handle,
                            ResourceManager *          rm);

    inline static ResourceManager::ResType GetType() { return "GLSLShader"; }

    explicit GLSLShader(uint64 program);
    ~GLSLShader();

    void Bind() const;

    void Set(const std::string &name, float value);
    void Set(const std::string &name, float *values, int count);
    void Set(const std::string &name, int value);
    void Set(const std::string &name, int *value, int count);
    void Set(const std::string &name, const Vec2 &value);
    void Set(const std::string &name, const Vec3 &value);
    void Set(const std::string &name, const Vec4 &value);
    void Set(const std::string &name, const Mat4 &value);

    inline uint64 GetProgram() const { return mShader; }

private:
    int GetUniformLocation(const std::string &name);

    uint64                     mShader;
    std::map<std::string, int> mUnifoms;
};
}
