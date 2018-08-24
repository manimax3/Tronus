#include "GBuffer.h"
#include "GLCheck.h"
#include "glad/glad.h"

void tr::GBuffer::Create(uint width, uint height)
{
    static_assert(sizeof(uint) == sizeof(GLuint),
                  "uint and GLuint not the same size");

    Call(glGenFramebuffers(1, &mFrameBufferHandle));
    Call(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferHandle));

    Call(glGenTextures(TextureTypes::_size(), &mTextures[0]));

    // Position component
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Position]));
    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGB,
                      GL_FLOAT, nullptr));
    Call(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                GL_TEXTURE_2D,
                                mTextures[TextureTypes::Position], 0));

    // Normal component
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Normal]));
    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB,
                      GL_FLOAT, nullptr));
    Call(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                                GL_TEXTURE_2D, mTextures[TextureTypes::Normal],
                                0));

    // Diffuse + specular component
    Call(glBindTexture(GL_TEXTURE_2D,
                       mTextures[TextureTypes::Diffuse_Specular]));
    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                      GL_UNSIGNED_BYTE, nullptr));
    Call(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                GL_TEXTURE_2D,
                                mTextures[TextureTypes::Diffuse_Specular], 0));

    // Depth + Stencil component
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Depth_Stencil]));
    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0,
                      GL_RG, GL_FLOAT, nullptr));
    Call(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_TEXTURE_2D,
                                mTextures[TextureTypes::Depth_Stencil], 0));

    // final component
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Final]));
    Call(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB,
                      GL_FLOAT, nullptr));
    Call(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3,
                                GL_TEXTURE_2D, mTextures[TextureTypes::Final],
                                0));

    GLenum draw[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1,
                      GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };

    Call(glDrawBuffers(4, draw));
    Call(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void tr::GBuffer::PrepareFrame()
{
    Call(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferHandle));
    Call(glDrawBuffer(GL_COLOR_ATTACHMENT3));
    Call(glClear(GL_COLOR_BUFFER_BIT));
}

void tr::GBuffer::PrepareGeometryPass()
{
    Call(glBindFramebuffer(GL_FRAMEBUFFER, mFrameBufferHandle));
    GLenum draw[]
        = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    Call(glDrawBuffers(3, draw));
}

void tr::GBuffer::PrepareStencilPass() { Call(glDrawBuffer(GL_NONE)); }

void tr::GBuffer::PrepareLightPass()
{
    Call(glDrawBuffer(GL_COLOR_ATTACHMENT3));

    Call(glActiveTexture(GL_TEXTURE0 + 0));
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Position]));

    Call(glActiveTexture(GL_TEXTURE0 + 1));
    Call(glBindTexture(GL_TEXTURE_2D, mTextures[TextureTypes::Normal]));

    Call(glActiveTexture(GL_TEXTURE0 + 2));
    Call(glBindTexture(GL_TEXTURE_2D,
                       mTextures[TextureTypes::Diffuse_Specular]));
}

void tr::GBuffer::PrepareFinalPass()
{
    Call(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0));
    Call(glBindFramebuffer(GL_READ_FRAMEBUFFER, mFrameBufferHandle));
    Call(glReadBuffer(GL_COLOR_ATTACHMENT3));
}
