#include "Texture.h"
#include "../core/Engine.h"
#include "GLCheck.h"
#include "Image.h"
#include "glad/glad.h"
#include "nlohmann/json.hpp"

GLenum                   ScaleFilterToEnum(tr::Texture::ScaleFilter f);
GLenum                   WrapModeToEnum(tr::Texture::WrapMode w);

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
    if (mTextureHandle)
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
    using json   = nlohmann::json;
    json jhandle = json::parse(handle);

    std::string image_dep_handle;
    WrapMode    s, t;
    ScaleFilter mag, min;
    bool        compression, generate_mipmaps;

    try {
        mag              = static_cast<ScaleFilter>(jhandle["mag_filter"]);
        min              = static_cast<ScaleFilter>(jhandle["min_filter"]);
        s                = static_cast<WrapMode>(jhandle["wrap_s"]);
        t                = static_cast<WrapMode>(jhandle["wrap_t"]);
        compression      = jhandle["compression"];
        generate_mipmaps = jhandle["generate_mipmaps"];
        image_dep_handle = jhandle["dependencies"][0];
    } catch (json::out_of_range e) {
        rm->GetEngine().Logger().log(
            "Error Loading values from Texture Handle: "s + handle + " | "
                + e.what(),
            LogLevel::WARNING);
        return nullptr;
    } catch (...) {
        rm->GetEngine().Logger().log("See the doctor!", LogLevel::ERROR);
        return nullptr;
    }

    Image *im = rm->GetRes<Image>(image_dep_handle);

    if (!im) {
        rm->GetEngine().Logger().log(
            "Couldnt find corresponding image to load Texture",
            LogLevel::WARNING);
        return nullptr;
    }

    return new Texture(*im, s, t, mag, min, generate_mipmaps, compression);
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

