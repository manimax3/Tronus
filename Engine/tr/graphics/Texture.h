#pragma once
#include "../filesystem/ResourceManager.h"
#include <tr.h>

namespace tr {

class Image;

class Texture : public Resource {
public:
    static constexpr const char *GetType() { return "Texture"; }
    static Resource *            Loader(ResourceManager::ResHandle handle,
                                        ResourceManager *          rm);

    enum WrapMode { REPEAT, MIRRORED_REPEAT, CLAMP_EDGE, CLAMP_BORDER };

    enum ScaleFilter {
        LINEAR,
        NEAREST,
        MIPMAP_NEAREST_NEAREST,
        MIPMAP_LINEAR_NEAREST,
        MIPMAP_NEAREST_LINEAR,
        MIPMAP_LINEAR_LINEAR
    };

    Texture(const Image &im,
            WrapMode     s,
            WrapMode     t,
            ScaleFilter  mag,
            ScaleFilter  min,
            bool         generate_mipmaps = false,
            bool         compressed       = false);

    ~Texture();

    void Bind(int slot = 0);

private:
    uint mTextureHandle = 0;
};
}
