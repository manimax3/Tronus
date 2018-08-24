#include "DeferredRenderer.h"
#include "../core/Engine.h"
#include "../gameobject/CameraComponent.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"
#include "glad/glad.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/component_wise.hpp"

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
        mNullShader = ResCast<GLSLShader>(
            tr::Engine::Get().sResourceManager->LoadResource("df_null.json"));
    } catch (const ResourceNotLoadedError &e) {
        Log().error(
            "Couldnt get the Shader Resource for the DeferredRenderer | {}",
            e.what());
        return;
    }

    try {
        mLightSphere = ResCast<StaticMesh>(
            gfx.GetEngine().sResourceManager->LoadResource(
                "df_lightsphere.trb"));
    } catch (const ResourceNotLoadedError &e) {
        Log().error("Couldnt get df_lightsphere.trb for DeferredRenderer | {}",
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

    mLightSphere->UploadToGpu();

    mRenderables.reserve(1000);
    mPointLights.reserve(50);
}

void tr::DeferredRenderer::GeometryPass()
{
    mGeometryShader->Bind();
    mGBuffer.PrepareGeometryPass();

    Call(glDepthMask(GL_TRUE));
    Call(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    Call(glEnable(GL_DEPTH_TEST));

    mGeometryShader->Set("view", mCamera->GetView());
    mGeometryShader->Set("projection", mCamera->GetProjection());

    for (auto &[mesh, mat, model] : mRenderables) {
        if (!mesh->IsOnGpu())
            continue;
        mGeometryShader->Set("model", model);
        mat->Bind(*mGeometryShader);
        mesh->GetBufferStore().Bind();
        Call(glDrawElements(GL_TRIANGLES, mesh->GetIndexCount(),
                            GL_UNSIGNED_INT, (void *)0));
    }

    Call(glDepthMask(GL_FALSE));
}

void tr::DeferredRenderer::StencilPass(const PointLight &light, const Mat4 &mvp)
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
    mLightSphere->GetBufferStore().Bind();
    Call(glDrawElements(GL_TRIANGLES, mLightSphere->GetIndexCount(),
                        GL_UNSIGNED_INT, (void *)0));
}

void tr::DeferredRenderer::LightingPass(const PointLight &light,
                                        const Mat4 &      mvp)
{
    mLightingPoint->Bind();
    mGBuffer.PrepareLightPass();

    mLightingPoint->Set("viewpos", mCamera->GetAbsoluteTranslation());
    mLightingPoint->Set("screensize",
                        tr::Engine::Get().sGraphicsHandler->GetWindowSize());

    mLightingPoint->Set("position", 0);
    mLightingPoint->Set("normal", 1);
    mLightingPoint->Set("diffuse", 2);

    mLightingPoint->Set("mvp", mvp);

    mLightingPoint->Set("light.Position", light.position);
    mLightingPoint->Set("light.Color", light.color);
    mLightingPoint->Set("light.Constant", light.Constant);
    mLightingPoint->Set("light.Linear", light.Linear);
    mLightingPoint->Set("light.Quadratic", light.Quadratic);

    Call(glStencilFunc(GL_NOTEQUAL, 0, 0xFF));
    Call(glDisable(GL_DEPTH_TEST));
    Call(glEnable(GL_BLEND));
    Call(glBlendEquation(GL_FUNC_ADD));
    Call(glBlendFunc(GL_ONE, GL_ONE));
    Call(glEnable(GL_CULL_FACE));
    Call(glCullFace(GL_FRONT));

    mLightSphere->GetBufferStore().Bind();
    Call(glDrawElements(GL_TRIANGLES, mLightSphere->GetIndexCount(),
                        GL_UNSIGNED_INT, (void *)0));

    Call(glCullFace(GL_BACK));
    Call(glDisable(GL_BLEND));
}
void tr::DeferredRenderer::FinalPass()
{
    mGBuffer.PrepareFinalPass();
    const auto size = tr::Engine::Get().sGraphicsHandler->GetWindowSize();
    Call(glBlitFramebuffer(0, 0, size.x, size.y, 0, 0, size.x, size.y,
                           GL_COLOR_BUFFER_BIT, GL_NEAREST));
}

void tr::DeferredRenderer::Render()
{
    if (!mCamera)
        return;

    mGBuffer.PrepareFrame();
    GeometryPass();
    Call(glEnable(GL_STENCIL_TEST));

    Mat4 vp = mCamera->GetProjection() * mCamera->GetView();

    for (auto &light : mPointLights) {
        Mat4 m = glm::translate(Mat4(1.f), light.position);

        // TODO: Do we need to do this eveytime?
        const float scale = CalcPointLightScale(light);

        m = glm::scale(m, Vec3(scale, scale, scale));

        m = vp * m;

        StencilPass(light, m);
        LightingPass(light, m);
    }

    Call(glDisable(GL_STENCIL_TEST));
    Call(glDisable(GL_DEPTH_TEST));
    FinalPass();
}

void tr::DeferredRenderer::AddMesh(ResourcePtr<StaticMesh>    mesh,
                                   ResourcePtr<PhongMaterial> material,
                                   Mat4                       model)
{
    if (!mesh->IsOnGpu())
        mesh->UploadToGpu();

    mRenderables.emplace_back(std::move(mesh), std::move(material), model);
}

void tr::DeferredRenderer::RemoveMesh(const ResourcePtr<StaticMesh> &mesh)
{
    const auto it = std::remove_if(std::begin(mRenderables),
                                   std::end(mRenderables), [&](const auto &r) {
                                       const auto &[_mesh, mat, model] = r;
                                       return _mesh == mesh;
                                   });
    if (it != std::end(mRenderables))
        mRenderables.erase(it);
}

void tr::DeferredRenderer::AddPointLight(const PointLight &light)
{
    mPointLights.push_back(light);
}

void tr::DeferredRenderer::RemovePointLight(const PointLight &light)
{
    const auto it
        = std::remove(std::begin(mPointLights), std::end(mPointLights), light);
    if (it != std::end(mPointLights))
        mPointLights.erase(it);
}

float tr::DeferredRenderer::CalcPointLightScale(const PointLight &light)
{
    const float max = math::compMax(light.color);

    const float ret
        = (-light.Linear
           + sqrtf(light.Linear * light.Linear
                   - 4 * light.Quadratic * (light.Quadratic - 256 * max * 0.1)))
        / (2 * light.Quadratic);

    return ret;
}
