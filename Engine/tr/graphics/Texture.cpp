#include "Texture.h"
#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "Image.h"
#include "glad/glad.h"
#include "nlohmann/json.hpp"

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

    mSize = Vec2(im.GetSizeX(), im.GetSizeY());
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

tr::ResourcePtr<>
tr::TextureLoader::LoadResource(ResourceLoadingInformation info,
                                const ResourceType &       type,
                                ResourceManager &          rm,
                                ResourceLoadingContext     context)
{
    using json    = nlohmann::json;
    json &jhandle = *info;

    if (!(rm.GetEngine().sGraphicsHandler->Valid())) {
        Log().error("Tried to create a Texture without valid render Context");
        return nullptr;
    }

    /* std::string image_dep_handle; */
    Texture::WrapMode    s, t;
    Texture::ScaleFilter mag, min;
    bool                 compression, generate_mipmaps;

    try {
        mag         = static_cast<Texture::ScaleFilter>(jhandle["mag_filter"]);
        min         = static_cast<Texture::ScaleFilter>(jhandle["min_filter"]);
        s           = static_cast<Texture::WrapMode>(jhandle["wrap_s"]);
        t           = static_cast<Texture::WrapMode>(jhandle["wrap_t"]);
        compression = jhandle["compression"];
        generate_mipmaps = jhandle["generate_mipmaps"];
        /* image_dep_handle = !jhandle["dependencies"][0].is_object() */
        /*     ? jhandle["dependencies"][0].get<std::string>() */
        /*     : jhandle["dependencies"][0]["id"].get<std::string>(); */
    } catch (json::out_of_range e) {
        Log().warn("Error Loading values from Texture Handle: <IMPLEMNT "
                   "TEXTTURE NAME RESOLVING> | {}",
                   e.what());
        return nullptr;
    } catch (...) {
        Log().error("Unknown Error occurred in Tetxutre.cpp");
        return nullptr;
    }

    /* ResHandle<Image> im = rm->GetRes<Image>(image_dep_handle); */
    auto beg = context.dependencies.begin();

    if (beg == std::end(context.dependencies)) {
        Log().warn("No dependency image was specified for texture");
        return nullptr;
    }

    auto name = std::get<0>(*beg);
    auto tup  = std::get<1>(*beg);

    if (std::get<0>(tup) == ResourceLoadingContext::NotFound) {
        Log().warn("Couldnt find corresponding image to load "
                   "Texture with provided name: {}",
                   name);
        return nullptr;
    }

    auto image = ResCast<Image>(std::get<1>(tup).value().lock());

    if (!image) {
        return nullptr;
    }

    return ResourcePtr<>(
        new Texture(*image, s, t, mag, min, generate_mipmaps, compression));
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

