#pragma once
#include "../event/CommonEvents.h"
#include "ForwardRenderer.h"
#include "GBuffer.h"
#include "GLSLShader.h"

namespace tr {
class CameraComponent;
class DeferredRenderer {
public:
    void Init(class GraphicsHandler &gfx);
    void Render();
    void Tick();
    void OnEvent(const WindowEvent &e);

    using RenderInfo       = ForwardRenderer::RenderInfo;
    using RenderInfoHandle = ForwardRenderer::RenderInfoHandle;

    RenderInfoHandle &CreateRenderInfo(RenderInfo info);

private:
    void GeometryPass();
    void StencilPass(const Mat4 &mvp);
    void LightingPass(const Mat4 &mvp);
    void FinalPass();

    GBuffer mGBuffer;

    ResourcePtr<GLSLShader> mGeometryShader;
    ResourcePtr<GLSLShader> mLightingPoint;
    ResourcePtr<GLSLShader> mNullShader;
    ResourcePtr<GLSLShader> mFinalShader;

    CameraComponent *mCamera = nullptr;
};
}
