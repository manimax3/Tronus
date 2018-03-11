#include "Texture.h"
#include "GLCheck.h"
#include "Image.h"
#include "glad/glad.h"

GLenum ScaleFilterToEnum(tr::Texture::ScaleFilter f);
GLenum WrapModeToEnum(tr::Texture::WrapMode w);

tr::Texture::Texture(const Image &im,
                     WrapMode     s,
                     WrapMode     t,
                     ScaleFilter  mag,
                     ScaleFilter  min,
                     bool         generate_mipmaps,
                     bool         compressed)
{
    Call(glGenTextures(1, &mTextureHandle));
    Call(glBindTexture(GL_TEXTURE_2D, mTextureHandle));

    Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapModeToEnum(s)));
    Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapModeToEnum(t)));

    Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                         ScaleFilterToEnum(mag)));
    Call(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                         ScaleFilterToEnum(min)));

    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, im.GetSizeX(), im.GetSizeY(),
                      0, GL_RGBA, GL_UNSIGNED_BYTE, (void *)im.GetPixels()));

    if (generate_mipmaps) {
        Call(glGenerateMipmap(GL_TEXTURE_2D));
    }

    if (compressed)
        throw std::logic_error("Compressed Textures not supported");
}

tr::Texture::~Texture()
{
    if(mTextureHandle)
        Call(glDeleteTextures(1, &mTextureHandle));
}

void tr::Texture::Bind(int slot)
{
    Call(glActiveTexture(GL_TEXTURE0 + slot));
    Call(glBindTexture(GL_TEXTURE_2D, mTextureHandle));
}

tr::Resource *tr::Texture::Loader(ResourceManager::ResHandle handle,
                                  ResourceManager *          rm)
{
    return nullptr;
}

GLenum ScaleFilterToEnum(tr::Texture::ScaleFilter f)
{
    using namespace tr;
    switch (f) {
    case Texture::LINEAR:
        return GL_LINEAR;

    case Texture::NEAREST:
        return GL_NEAREST;

    case Texture::MIPMAP_NEAREST_NEAREST:
        return GL_NEAREST_MIPMAP_NEAREST;

    case Texture::MIPMAP_NEAREST_LINEAR:
        return GL_NEAREST_MIPMAP_LINEAR;

    case Texture::MIPMAP_LINEAR_LINEAR:
        return GL_LINEAR_MIPMAP_LINEAR;

    case Texture::MIPMAP_LINEAR_NEAREST:
        return GL_LINEAR_MIPMAP_NEAREST;

    default:
        throw std::logic_error("Unsupporetd ScaleFilterEnum");
    }
}

GLenum WrapModeToEnum(tr::Texture::WrapMode w)
{
    using namespace tr;
    switch (w) {
    case Texture::REPEAT:
        return GL_REPEAT;

    case Texture::MIRRORED_REPEAT:
        return GL_MIRRORED_REPEAT;

    case Texture::CLAMP_EDGE:
        return GL_CLAMP_TO_EDGE;

    case Texture::CLAMP_BORDER:
        return GL_CLAMP_TO_BORDER;

    default:
        throw std::logic_error("Unsupporetd WrapModeEnum");
    }
}
