#include "DeferredRenderer.h"
#include "../core/Engine.h"
#include "../gameobject/CameraComponent.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "glad/glad.h"

void tr::DeferredRenderer::Init(GraphicsHandler &gfx)
{
    if (!gfx.Context().valid) {
        Log().warn("Couldnt init the DeferredRenderer because of a missing "
                   "rendering context");
        return;
    }

    try {
        mGeometryShader = ResCast<GLSLShader>(
            tr::Engine::Get().sResourceManager->LoadResource(
                "df_geometry.json"));
        mLightingPoint = ResCast<GLSLShader>(
            tr::Engine::Get().sResourceManager->LoadResource(
                "df_lightning.json"));
        mLightingPoint = ResCast<GLSLShader>(
            tr::Engine::Get().sResourceManager->LoadResource("df_null.json"));
    } catch (const ResourceNotLoadedError &e) {
        Log().error(
            "Couldnt get the Shader Resource for the ForwardRenderer | {}",
            e.what());
        return;
    }

    gfx.CameraUpdate.connect([&](CameraComponent *cam, bool reset) {
        if (mCamera == cam && reset) {
            mCamera = nullptr;
        }
        mCamera = cam;
    });

    mGBuffer.Create(gfx.GetWindowSize().x, gfx.GetWindowSize().y);
}

void tr::DeferredRenderer::GeometryPass()
{
    mGeometryShader->Bind();
    mGBuffer.PrepareGeometryPass();

    Call(glDepthMask(GL_TRUE));
    Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    Call(glEnable(GL_DEPTH_TEST));

    mGeometryShader->Set("view", mCamera->GetView());
    mGeometryShader->Set("projcetion", mCamera->GetProjection());

    // Render meshes
    {
        mGeometryShader->Set("model", Mat4(1.f)); // TODO: Model matrix
    }

    Call(glDepthMask(GL_FALSE));
}

void tr::DeferredRenderer::StencilPass(const Mat4 &mvp)
{
    mNullShader->Bind();
    mGBuffer.PrepareStencilPass();

    Call(glEnable(GL_DEPTH_TEST));
    Call(glDisable(GL_CULL_FACE));
    Call(glClear(GL_STENCIL_BUFFER_BIT));
    Call(glStencilFunc(GL_ALWAYS, 0, 0));
    Call(glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP));
    Call(glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP));

    mNullShader->Set("mvp", mvp);
    {
        /* render sphere */
    }
}
void tr::DeferredRenderer::LightingPass(const Mat4 &mvp)
{
    mLightingPoint->Bind();
    mGBuffer.PrepareLightPass();

    mLightingPoint->Set("viewpos", mCamera->GetAbsoluteTranslation());
    mLightingPoint->Set("screensize",
                        tr::Engine::Get().sGraphicsHandler->GetWindowSize());

    mLightingPoint->Set("position", 0);
    mLightingPoint->Set("normal", 1);
    mLightingPoint->Set("specular", 2);

    mLightingPoint->Set("mvp", mvp);
    {
        // Set light data
    }

    Call(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
    Call(glEnable(GL_BLEND));
    Call(glBlendEquation(GL_FUNC_ADD));
    Call(glBlendFunc(GL_ONE, GL_ONE));
    Call(glEnable(GL_CULL_FACE));
    Call(glCullFace(GL_FRONT));

    // Render sphere

    Call(glCullFace(GL_BACK));
    Call(glDisable(GL_BLEND));
}
void tr::DeferredRenderer::FinalPass()
{
    mGBuffer.PrepareFinalPass();
    const auto size = tr::Engine::Get().sGraphicsHandler->GetWindowSize();
    Call(glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y,
                           GL_COLOR_BUFFER_BIT, GL_LINEAR));
}

void tr::DeferredRenderer::Render()
{
    mGBuffer.PrepareFrame();
    GeometryPass();
    Call(glEnable(GL_STENCIL_TEST));

    // for all lights
    {
        Mat4 m;
        StencilPass(m);
        LightingPass(m);
    }

    Call(glDisable(GL_STENCIL_TEST));
    FinalPass();
}
