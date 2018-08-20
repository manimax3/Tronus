#include "BufferHelper.h"
#include "../util/Log.h"
#include "GLCheck.h"
#include "glad/glad.h"

namespace {
GLenum ToEnum(tr::BufferType t)
{
    switch (t) {
    case tr::BufferType::Vertex:
        return GL_ARRAY_BUFFER;
    case tr::BufferType::Index:
        return GL_ELEMENT_ARRAY_BUFFER;
    default:
        tr::Log().critical(
            "Couldnt resolve BufferType to corresponding GLenum");
        return GL_NONE;
    }
}
GLenum ToEnum(tr::detail::Buffer::Locality m)
{

    switch (m) {
    case tr::BufferLocality::Static:
        return GL_STATIC_DRAW;
    case tr::BufferLocality::Dynamic:
        return GL_DYNAMIC_DRAW;
    case tr::BufferLocality::Stream:
        return GL_STREAM_DRAW;

    default:
        tr::Log().critical(
            "Couldnt resolve BufferLocality to corresponding GLenum");
        return GL_NONE;
    }
}
std::tuple<GLenum, int> ToEnum(tr::detail::BufferLayout::ElementType e)
{
    switch (e) {
    case tr::ShaderElementType::Float:
        return { GL_FLOAT, 1 };
    case tr::ShaderElementType::Vec2:
        return { GL_FLOAT, 2 };
    case tr::ShaderElementType::Vec3:
        return { GL_FLOAT, 3 };
    case tr::ShaderElementType::Vec4:
        return { GL_FLOAT, 4 };
    default:
        tr::Log().critical(
            "Other ShaderElementTypes apart from floats and Vec2-4 are not "
            "supported in BufferLayouts");
        return { GL_NONE, 0 };
    }
}
}

void tr::detail::Buffer::Create(size_t element_size, size_t count, void *data)
{
    if (*this) {
        Log().warn("Tried to create a graphics buffer twice! For resizing "
                   "create a new Buffer");
        return;
    }

    GLuint v;
    Call(glGenBuffers(1, &v));
    mHandle = v;

    Bind();

    Call(glBufferData(ToEnum(mType), element_size * count, data,
                      ToEnum(mLocality)));
    mDataSize = element_size * count;
}

void tr::detail::Buffer::Destroy()
{
    if (!*this)
        return;
    Call(glDeleteBuffers(1, std::any_cast<GLuint>(&mHandle)));
}

tr::detail::Buffer::operator bool() const
{
    auto i = std::any_cast<GLuint>(mHandle);
    return i != 0;
}

void tr::detail::Buffer::Bind()
{
    if (!*this)
        return;

    Call(glBindBuffer(ToEnum(mType), std::any_cast<GLuint>(mHandle)));
}
void tr::detail::Buffer::Unbind() { Call(glBindBuffer(ToEnum(mType), 0)); }

void tr::detail::BufferLayout::Add(uint        index,
                                   ElementType type,
                                   void *      offset,
                                   bool        normalized)
{
    // TODO: Check the index
    mAttribPointers.push_back(std::tuple(index, type, normalized, offset));
}

void tr::detail::BufferLayout::ApplyTo(Buffer &buffer)
{
    if (!buffer)
        return;

    buffer.Bind();

    for (auto [index, type, normalized, offset] : mAttribPointers) {
        auto [e, c] = ToEnum(type);
        Call(glVertexAttribPointer(index, c, e, normalized, mSize, offset));
        Call(glEnableVertexAttribArray(index));
    }
}

void tr::detail::AttributBufferStore::Create()
{
    if (*this) {
        Log().warn("Tried to create a graphics attribut buffer twice!");
        return;
    }

    GLuint handle;
    Call(glGenVertexArrays(1, &handle));
    mHandle = handle;
}

void tr::detail::AttributBufferStore::Destroy(bool destory_buffers)
{

    if (!*this)
        return;

    if (destory_buffers) {
        for (auto &b : mBuffers)
            b.Destroy();
    }

    Call(glDeleteVertexArrays(1, std::any_cast<GLuint>(&mHandle)));
}

void tr::detail::AttributBufferStore::Bind()
{
    if (!*this)
        return;

    Call(glBindVertexArray(std::any_cast<GLuint>(mHandle)));
}

void tr::detail::AttributBufferStore::Unbind() { Call(glBindVertexArray(0)); }

void tr::detail::AttributBufferStore::AddBuffer(const Buffer &b)
{
    mBuffers.push_back(b);
}

void tr::detail::BufferLayout::AddAttributesToShaderInterface(
    ShaderInterface &interface) const
{
    for (auto &[index, type, n, o] : mAttribPointers) {
        interface.AddAttribute(type, index);
    }
}

tr::detail::AttributBufferStore::operator bool() const
{
    return std::any_cast<GLuint>(mHandle) != 0;
}
