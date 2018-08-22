#pragma once
#include <tr.h>

#include "../util/enum.h"
#include <array>

namespace tr {

/**
 * Different types of a gbuffers texutres
 */
BETTER_ENUM(GBuffer_Texture_Types,
            int,
            Position,
            Normal,
            Diffuse_Specular,
            Depth_Stencil,
            Final);

/*
 * GBuffer helper.
 * Used for deferred rendering
 */
class GBuffer {
public:
    using TextureTypes = GBuffer_Texture_Types;

    GBuffer() = default;
    ~GBuffer();
    void Create(uint width, uint height);
    void Resize(uint width, uint height);

    void PrepareFrame();
    void PrepareGeometryPass();
    /* void PrepareStenciPass(); */
    void PrepareLightPass();
    void PrepareFinalPass();

private:
    using TexArray = std::array<uint, TextureTypes::_size()>;

    uint     mFrameBufferHandle = 0;
    TexArray mTextures          = { 0 };
};
}
