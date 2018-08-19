#include "GLSLShader.h"
#include "../core/Engine.h"
#include "../filesystem/Filesystem.h"
#include "../filesystem/ResourceManager.h"
#include "../util/Exceptions.h"
#include "GLCheck.h"
#include "glad/glad.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <tuple>

using json = nlohmann::json;

namespace tr::detail {
std::tuple<std::string, std::string>
     SplitShaders(const std::string &shaders,
                  const std::string &vertex_sep,
                  const std::string &fragment_sep);
void checkCompileErrors(uint shader, const std::string &type);
}

tr::uint tr::GLSLShader::CompileShader(const std::string &vertex,
                                       const std::string &fragment)
{
    GLuint s_vertex, s_fragment;

    const GLchar *c_vertex   = vertex.c_str();
    const GLchar *c_fragment = fragment.c_str();

    const GLint v_size = vertex.size();
    const GLint f_size = fragment.size();

    GLuint program = 0;

    // Compile vertex shader
    s_vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(s_vertex, 1, &c_vertex, &v_size);
    glCompileShader(s_vertex);
    detail::checkCompileErrors(s_vertex, "Vertex");

    // Compile fragment shader
    s_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(s_fragment, 1, &c_fragment, &f_size);
    glCompileShader(s_fragment);
    detail::checkCompileErrors(s_fragment, "Fragment");

    program = glCreateProgram();

    glAttachShader(program, s_vertex);
    glAttachShader(program, s_fragment);

    glLinkProgram(program);
    detail::checkCompileErrors(program, "PROGRAM");

    glDeleteShader(s_vertex);
    glDeleteShader(s_fragment);

    return static_cast<uint>(program);
}

tr::ResourcePtr<>
tr::GLSLShaderLoader::LoadResource(ResourceLoadingInformation info,
                                   const ResourceType &       type,
                                   ResourceManager &          rm,
                                   ResourceLoadingContext     context)
{
    uint64      program;
    const json &jhandle = *info;

    std::string shader_file, vertex_sep, fragment_sep;

    try {
        shader_file  = jhandle.at("shader_source");
        vertex_sep   = jhandle.at("vertex_seperator");
        fragment_sep = jhandle.at("fragment_seperator");
    } catch (const json::out_of_range &e) {
        Log().error("Loading shader | {}", e.what());
        return nullptr;
    }

    shader_file = fs::GetExecutablePath() + shader_file;
    shader_file = rm.ResolvePath(shader_file);

    // Load the file
    if (fs::FileExists(shader_file)) {
        std::ifstream     ifs(shader_file, std::ios::in);
        std::stringstream ss;
        ss << ifs.rdbuf();
        shader_file = ss.str();
    } else {
        Log().warn("Couldnt open file: {}", shader_file);
        return nullptr;
    }

    try {
        auto [vertex, fragment]
            = detail::SplitShaders(shader_file, vertex_sep, fragment_sep);
        program = GLSLShader::CompileShader(vertex, fragment);
    } catch (const std::runtime_error &e) {
        Log().error("Shader parsing | ", e.what());
    }

    std::optional<ShaderInterface> siv, sif;

    try {
        siv = jhandle.at("vertex_interface").get<ShaderInterface>();
    } catch (const json::out_of_range &) {
        Log().warn("No vertex_interface specified for shader | {}",
                   jhandle.dump());
    } catch (const json::parse_error &e) {
        Log().warn("Couldnt parse the vertex_interface of a shader | {}",
                   e.what());
    }

    try {
        sif = jhandle.at("fragment_interface").get<ShaderInterface>();
    } catch (const json::out_of_range &) {
        Log().warn("No fragment_interface specified for shader | {}",
                   jhandle.dump());
    } catch (const json::parse_error &e) {
        Log().warn("Couldnt parse the fragment_interface of a shader | {}",
                   e.what());
    }

    return ResourcePtr<>(
        program ? new GLSLShader(program, std::move(siv), std::move(sif))
                : nullptr);
}

tr::GLSLShader::GLSLShader(uint                           program,
                           std::optional<ShaderInterface> vertex_interface,
                           std::optional<ShaderInterface> fragment_interface)
    : mShader(static_cast<GLuint>(program))
    , mShaderInterfaceVertex(std::move(vertex_interface))
    , mShaderInterfaceFragment(std::move(fragment_interface))
{
}

tr::GLSLShader::~GLSLShader()
{
    if (mShader)
        Call(glDeleteProgram(mShader));
}

int tr::GLSLShader::GetUniformLocation(const std::string &name)
{
    if (const auto loc = mUnifoms.find(name); loc != mUnifoms.end()) {
        return loc->second;
    } else {
        const int location = Call(glGetUniformLocation(mShader, name.c_str()));
        mUnifoms[name]     = location;
        return location;
    }
}

void tr::GLSLShader::Bind() const { Call(glUseProgram(mShader)); }

void tr::GLSLShader::Set(const std::string &name, float value)
{
    Call(glUniform1f(GetUniformLocation(name), value));
}
void tr::GLSLShader::Set(const std::string &name, float *values, int count)
{
    Call(glUniform1fv(GetUniformLocation(name), count, values));
}
void tr::GLSLShader::Set(const std::string &name, int value)
{
    Call(glUniform1i(GetUniformLocation(name), value));
}
void tr::GLSLShader::Set(const std::string &name, int *value, int count)
{
    Call(glUniform1iv(GetUniformLocation(name), count, value));
}
void tr::GLSLShader::Set(const std::string &name, const Vec2 &value)
{
    Call(glUniform2f(GetUniformLocation(name), value.x, value.y));
}
void tr::GLSLShader::Set(const std::string &name, const Vec3 &value)
{
    Call(glUniform3f(GetUniformLocation(name), value.x, value.y, value.z));
}
void tr::GLSLShader::Set(const std::string &name, const Vec4 &value)
{
    Call(glUniform4f(GetUniformLocation(name), value.x, value.y, value.z,
                     value.w));
}
void tr::GLSLShader::Set(const std::string &name, const Mat4 &value)
{
    Call(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE,
                            &value[0][0]));
}

std::tuple<std::string, std::string>
tr::detail::SplitShaders(const std::string &shaders,
                         const std::string &vertex_sep,
                         const std::string &fragment_sep)

{
    const std::string::size_type vpos = shaders.find(vertex_sep);
    const std::string::size_type fpos = shaders.find(fragment_sep);

    std::string vertex, fragment;

    if (vpos == shaders.npos)
        throw std::runtime_error("Unable to find a Vertex Seperator");

    if (fpos == shaders.npos)
        throw std::runtime_error("Unable to find a Fragment Seperator");

    // Determin which one comes first
    if (vpos < fpos) {
        fragment = shaders.substr(fpos + fragment_sep.size());
        vertex   = shaders.substr(vpos + vertex_sep.size(),
                                fpos - 1 - (vpos + vertex_sep.size()));
    } else if (vpos > fpos) {
        vertex   = shaders.substr(vpos + vertex_sep.size());
        fragment = shaders.substr(fpos + fragment_sep.size(),
                                  vpos - 1 - (fpos + fragment_sep.size()));
    } else
        throw std::runtime_error("Possible Parsing Bug?");

    return std::make_tuple<std::string, std::string>(std::move(vertex),
                                                     std::move(fragment));
}

void tr::detail::checkCompileErrors(uint shader, const std::string &type)
{
    using namespace tr;
    GLint  success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            Log().error("Error Compiling a {} Shader | {}", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            Log().error("Error Linking a {} Shader | {}", type, infoLog);
        }
    }
}

void tr::ShaderInterface::AddAttribute(ElementType type, int location)
{
    throw NotImplementedError("ShaderInterface AddAttribute");
}

void tr::ShaderInterface::AddUniform(ElementType type, std::string name)
{
    throw NotImplementedError("ShaderInterface AddUniform");
}

bool tr::ShaderInterface::HasAttribute(int location, ElementType type) const
{
    return std::find_if(std::begin(mAttributes), std::end(mAttributes),
                        [&](const auto &a) {
                            return location == a.location && type == a.type;
                        })
        != std::end(mAttributes);
}

bool tr::ShaderInterface::HasUniform(const std::string &name,
                                     ElementType        type) const
{
    return std::find_if(
               std::begin(mUniforms), std::end(mUniforms),
               [&](const auto &a) { return type == a.type && name == a.name; })
        != std::end(mUniforms);
}

bool tr::ShaderInterface::IsCompatibleWith(const ShaderInterface &other) const
{
    for (const auto &a : mAttributes) {
        if (!other.HasAttribute(a.location, a.type))
            return false;
    }

    for (const auto &u : mUniforms) {
        if (!other.HasUniform(u.name, u.type))
            return false;
    }

    // Empty interfaces are always compatible
    return true;
}

namespace tr {
static void from_json(const json &j, ShaderElement &e)
{
    std::string v = j.get<std::string>();

    try {
        e = ShaderElement::_from_string_nocase(v.c_str());
    } catch (const std::runtime_error &e) {
        throw json::parse_error::create(
            101, 0, fmt::format("Couldnt parse {} to a ShaderElement enum", v));
    }
}

static void from_json(const json &j, ShaderElementType &type)
{
    std::string v = j.get<std::string>();

    try {
        type = ShaderElementType::_from_string_nocase(v.c_str());
    } catch (const std::runtime_error &e) {
        throw json::parse_error::create(
            101, 0,
            fmt::format("Couldnt parse {} to a ShaderElementType enum", v));
    }
}

static void from_json(const json &j, ShaderInterface::Uniform &uniform)
{
    uniform.name = j.at("name");
    uniform.type = j.at("type").get<ShaderElementType>();
}

static void from_json(const json &j, ShaderInterface::Attribute &attribute)
{
    attribute.location = j.at("location");
    attribute.type     = j.at("type").get<ShaderElementType>();

    if (attribute.type == +ShaderElementType::Sampler2D_Diffuse
        || attribute.type == +ShaderElementType::Sampler2D_Albedo
        || attribute.type == +ShaderElementType::Sampler2D_Specular
        || attribute.type == +ShaderElementType::Sampler2D_Other
        || attribute.type == +ShaderElementType::Struct)
        throw json::parse_error::create(
            101, 0,
            fmt::format(
                "{} Is not a supported type for ShaderInterface::Attribute",
                j.at("type").get<std::string>()));
}
}

void tr::from_json(const json &j, ShaderInterface &i)
{
    const json &attributes = j.at("attributes");
    const json &uniforms   = j.at("uniforms");

    if (!attributes.is_array() || !uniforms.is_array())
        throw json::parse_error::create(
            104, 0, "attributes or uniforms is not an array");

    for (auto it = attributes.begin(); it != attributes.end(); it++) {
        ShaderInterface::Attribute a = it->get<ShaderInterface::Attribute>();
        i.mAttributes.push_back(std::move(a));
    }

    for (auto it = uniforms.begin(); it != uniforms.end(); it++) {
        ShaderInterface::Uniform u = it->get<ShaderInterface::Uniform>();
        i.mUniforms.push_back(std::move(u));
    }
}
