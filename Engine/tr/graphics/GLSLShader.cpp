#include "GLSLShader.h"
#include "../core/Engine.h"
#include "../filesystem/Filesystem.h"
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
void checkCompileErrors(uint shader, const std::string &type, Log &log);
}

tr::uint64 tr::GLSLShader::CompileShader(const std::string &vertex,
                                         const std::string &fragment,
                                         Log &              log)
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
    detail::checkCompileErrors(s_vertex, "Vertex", log);

    // Compile fragment shader
    s_fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(s_fragment, 1, &c_fragment, &f_size);
    glCompileShader(s_fragment);
    detail::checkCompileErrors(s_fragment, "Fragment", log);

    program = glCreateProgram();

    glAttachShader(program, s_vertex);
    glAttachShader(program, s_fragment);

    glLinkProgram(program);
    detail::checkCompileErrors(program, "PROGRAM", log);

    glDeleteShader(s_vertex);
    glDeleteShader(s_fragment);

    return static_cast<tr::uint64>(program);
}

tr::Resource *tr::GLSLShader::Loader(ResourceManager::ResHandle handle,
                                     ResourceManager *          rm)
{
    uint64     program;
    const json jhandle = json::parse(handle);

    std::string shader_file, vertex_sep, fragment_sep;

    try {
        shader_file  = jhandle.at("shader_source");
        vertex_sep   = jhandle.at("vertex_seperator");
        fragment_sep = jhandle.at("fragment_seperator");
    } catch (json::out_of_range e) {
        rm->GetEngine().Logger().log(
            "Loading shader: "s + handle + " | " + e.what(), LogLevel::ERROR);
        return nullptr;
    }

    shader_file = fs::GetExecutablePath() + shader_file;

    // Load the file
    if (fs::FileExists(shader_file)) {
        std::ifstream     ifs(shader_file, std::ios::in);
        std::stringstream ss;
        ss << ifs.rdbuf();
        shader_file = ss.str();
    } else {
        rm->GetEngine().Logger().log("Could not open file: "s + shader_file,
                                     LogLevel::WARNING);
        return nullptr;
    }

    try {
        auto[vertex, fragment]
            = detail::SplitShaders(shader_file, vertex_sep, fragment_sep);
        program = CompileShader(vertex, fragment, rm->GetEngine().Logger());
    } catch (std::runtime_error e) {
        rm->GetEngine().Logger().log("Shader parsing: "s + e.what(),
                                     LogLevel::ERROR);
    }

    return program ? new GLSLShader(program) : nullptr;
}

tr::GLSLShader::GLSLShader(uint64 program)
    : mShader(static_cast<GLuint>(program))
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
    Call(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE,
                            value.m_Values));
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

void tr::detail::checkCompileErrors(uint               shader,
                                    const std::string &type,
                                    tr::Log &          log)
{
    using namespace tr;
    GLint  success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            log.log("Error Compiling a "s + type + " Shader | " + infoLog,
                    LogLevel::ERROR);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            log.log("Error Linking a "s + type + " Shader | " + infoLog,
                    LogLevel::ERROR);
        }
    }
}
