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

    enum WrapMode {
        REPEAT          = 1,
        MIRRORED_REPEAT = 2,
        CLAMP_EDGE      = 3,
        CLAMP_BORDER    = 4
    };

    enum ScaleFilter {
        LINEAR                 = 1,
        NEAREST                = 2,
        MIPMAP_NEAREST_NEAREST = 3,
        MIPMAP_LINEAR_NEAREST  = 4,
        MIPMAP_NEAREST_LINEAR  = 5,
        MIPMAP_LINEAR_LINEAR   = 6
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
