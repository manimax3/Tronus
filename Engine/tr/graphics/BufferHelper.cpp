#include "BufferHelper.h"
#include "../util/Log.h"
#include "GLCheck.h"
#include "glad/glad.h"

namespace {
GLenum ToEnum(tr::detail::BufferType t)
{
    using tr::detail::BufferType;
    if (t == BufferType::VERTEX)
        return GL_ARRAY_BUFFER;
    else if (t == BufferType::INDEX)
        return GL_ELEMENT_ARRAY_BUFFER;
    else {
        tr::Log().critical(
            "Couldnt resolve BufferType to corresponding GLenum");
        return GL_NONE;
    }
}
GLenum ToEnum(tr::detail::Buffer::Locality m)
{
    using tr::detail::Buffer;
    if (m == Buffer::STATIC)
        return GL_STATIC_DRAW;
    else if (m == Buffer::DYNAMIC)
        return GL_DYNAMIC_DRAW;
    else if (m == Buffer::STREAM)
        return GL_STREAM_DRAW;
    else {
        tr::Log().critical(
            "Couldnt resolve BufferLocality to corresponding GLenum");
        return GL_NONE;
    }
}
GLenum ToEnum(tr::detail::BufferLayout::ElementType e)
{
    using tr::detail::BufferLayout;
    if (e == BufferLayout::FLOAT) {
        return GL_FLOAT;
    } else {
        tr::Log().critical(
            "Couldnt resolve BufferElementType to corresponding GLenum");
        return GL_NONE;
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
                                   int         components,
                                   ElementType type,
                                   void *      offset,
                                   bool        normalized)
{
    // TODO: Check the index
    mAttribPointers.push_back(
        std::tuple(index, components, type, normalized, offset));
}

void tr::detail::BufferLayout::ApplyTo(Buffer &buffer)
{
    if (!buffer)
        return;

    buffer.Bind();

    for (auto [index, components, type, normalized, offset] : mAttribPointers) {
        Call(glVertexAttribPointer(index, components, ToEnum(type), normalized,
                                   mSize, offset));
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
