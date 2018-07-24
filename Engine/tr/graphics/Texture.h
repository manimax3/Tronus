#pragma once
#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"
#include <tr.h>

namespace tr {

class Image;

class Texture : public Resource {
public:
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

    ~Texture() override;

    void Bind(int slot = 0);

    Vec2 GetSize() const { return mSize; };

private:
    uint mTextureHandle = 0;
    Vec2 mSize          = Vec2(0);
};

class TextureLoader : public ResourceLoadHandler {
public:
    ResourcePtr<> LoadResource(ResourceLoadingInformation info,
                               const ResourceType &       type,
                               ResourceManager &          rm,
                               ResourceLoadingContext     context) override;
    // TODO: Implement this
    /* std::string */
    /* ResourceName(const ResourceLoadingInformation &info) const override; */

    inline std::list<ResourceType> GetSupportedTypes() const override
    {
        return { "Texture" };
    }
};
}
